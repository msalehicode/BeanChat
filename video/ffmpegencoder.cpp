#include "ffmpegencoder.h"

#include <QDebug>

FFmpegEncoder::FFmpegEncoder(QObject *parent)
    : QObject(parent)
{
}

FFmpegEncoder::~FFmpegEncoder()
{
    close();
}

bool FFmpegEncoder::open(
    int width,
    int height,
    int fps,
    int bitrate,
    int gopSize)
{
    close();

    const AVCodec *codec =
        avcodec_find_encoder(AV_CODEC_ID_H264);

    if (!codec)
    {
        qDebug() << "Could not find H264 encoder.";
        return false;
    }

    m_codec = avcodec_alloc_context3(codec);

    if (!m_codec)
    {
        qDebug() << "Could not allocate codec context.";
        return false;
    }

    m_width = width;
    m_height = height;

    m_codec->width = width;
    m_codec->height = height;

    m_codec->pix_fmt = AV_PIX_FMT_YUV420P;

    m_codec->time_base = AVRational{1, fps};
    m_codec->framerate = AVRational{fps, 1};

    m_codec->bit_rate = bitrate;

    m_codec->gop_size = gopSize;
    m_codec->max_b_frames = 0;

    // Very important for realtime video
    av_opt_set(m_codec->priv_data,
               "preset",
               "veryfast",
               0);

    av_opt_set(m_codec->priv_data,
               "tune",
               "zerolatency",
               0);

    if (avcodec_open2(
            m_codec,
            codec,
            nullptr) < 0)
    {
        qDebug() << "Failed to open H264 encoder.";

        close();

        return false;
    }

    m_frame = av_frame_alloc();

    if (!m_frame)
    {
        close();
        return false;
    }

    m_frame->format = AV_PIX_FMT_YUV420P;
    m_frame->width = width;
    m_frame->height = height;

    if (av_frame_get_buffer(
            m_frame,
            32) < 0)
    {
        close();
        return false;
    }

    m_packet = av_packet_alloc();

    if (!m_packet)
    {
        close();
        return false;
    }

    m_sws = sws_getContext(
        width,
        height,
        AV_PIX_FMT_RGB24,

        width,
        height,
        AV_PIX_FMT_YUV420P,

        SWS_FAST_BILINEAR,

        nullptr,
        nullptr,
        nullptr);

    if (!m_sws)
    {
        close();
        return false;
    }

    m_pts = 0;

    qDebug() << "FFmpeg encoder initialized.";

    return true;
}

void FFmpegEncoder::close()
{
    if (m_sws)
    {
        sws_freeContext(m_sws);
        m_sws = nullptr;
    }

    if (m_packet)
    {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }

    if (m_frame)
    {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }

    if (m_codec)
    {
        avcodec_free_context(&m_codec);
        m_codec = nullptr;
    }

    m_width = 0;
    m_height = 0;
    m_pts = 0;
}


void FFmpegEncoder::encode(const QImage &image)
{
    if (!m_codec)
        return;

    // Convert to RGB888 (what swscale expects)
    QImage rgb =
        image.convertToFormat(QImage::Format_RGB888);

    // Make sure sizes match
    if (rgb.width() != m_width ||
        rgb.height() != m_height)
    {
        rgb = rgb.scaled(
            m_width,
            m_height,
            Qt::IgnoreAspectRatio,
            Qt::FastTransformation);
    }

    if (av_frame_make_writable(m_frame) < 0)
        return;

    const uint8_t *srcSlice[1] =
    {
        rgb.constBits()
    };

    int srcStride[1] =
    {
        static_cast<int>(rgb.bytesPerLine())
    };

    sws_scale(
        m_sws,
        srcSlice,
        srcStride,
        0,
        m_height,
        m_frame->data,
        m_frame->linesize);

    m_frame->pts = m_pts++;

    if (avcodec_send_frame(
            m_codec,
            m_frame) < 0)
    {
        qDebug() << "avcodec_send_frame failed";
        return;
    }

    while (true)
    {
        int ret =
            avcodec_receive_packet(
                m_codec,
                m_packet);

        if (ret == AVERROR(EAGAIN) ||
            ret == AVERROR_EOF)
        {
            break;
        }

        if (ret < 0)
        {
            qDebug() << "avcodec_receive_packet failed";
            break;
        }

        QByteArray packet(
            reinterpret_cast<const char*>(m_packet->data),
            m_packet->size);

        bool keyFrame =
            (m_packet->flags & AV_PKT_FLAG_KEY);

        emit packetReady(
            packet,
            keyFrame);

        qDebug()
            << "PTS:" << m_packet->pts
            << "DTS:" << m_packet->dts
            << "size:" << m_packet->size
            << "key:" << keyFrame;

        av_packet_unref(
            m_packet);
    }
}


