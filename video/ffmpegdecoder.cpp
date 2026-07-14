#include "ffmpegdecoder.h"

#include "logging/loggingcategories.h"

FFmpegDecoder::FFmpegDecoder(QObject *parent)
    : QObject(parent)
{
}

FFmpegDecoder::~FFmpegDecoder()
{
    close();
}

bool FFmpegDecoder::open()
{
    qCInfo(_ffmpeg) << "close old opened decoder if exists, then open decoder";
    close();

    const AVCodec *codec =
        avcodec_find_decoder(AV_CODEC_ID_H264);

    if (!codec)
    {
        qCCritical(_ffmpeg) << "Could not find H264 decoder.";
        return false;
    }

    m_codec = avcodec_alloc_context3(codec);

    if (!m_codec)
    {
        qCCritical(_ffmpeg) << "Could not allocate decoder context.";
        return false;
    }

    int ret = avcodec_open2(
        m_codec,
        codec,
        nullptr);

    if (ret < 0)
    {
        char err[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, err, sizeof(err));

        qCCritical(_ffmpeg)
            << "Failed to open decoder:"
            << err;

        close();

        return false;
    }

    m_frame = av_frame_alloc();

    if (!m_frame)
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

    qCInfo(_ffmpeg) << "FFmpeg decoder initialized.";

    qCInfo(_ffmpeg)
        << "Decoder info: "
        << m_codec->codec->name
        << m_codec->codec->long_name;

    return true;
}

void FFmpegDecoder::close()
{
    qCInfo(_ffmpeg) << "try to close decoder.";
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
}


void FFmpegDecoder::decode(const QByteArray &packetData)
{
    if (!m_codec)
        return;

    av_packet_unref(m_packet);

    m_packet->data =
        reinterpret_cast<uint8_t *>(
            const_cast<char *>(packetData.constData()));

    m_packet->size = packetData.size();

    // qCDebug(_ffmpeg)
    //     << "received packet"
    //     << packetData.size();

    int ret =
        avcodec_send_packet(
            m_codec,
            m_packet);

    if (ret < 0)
    {
        char err[256];
        av_strerror(ret, err, sizeof(err));

        qCCritical(_ffmpeg) << "avcodec_send_packet failed. send_packet:" << err;

        return;
    }

    while (true)
    {
        ret =
            avcodec_receive_frame(
                m_codec,
                m_frame);

        // qCDebug(_ffmpeg)
        //     << "Decoded frame"
        //     << m_frame->width
        //     << m_frame->height
        //     << "format"
        //     << m_frame->format
        ////     << "key"  << m_frame->key_frame; //this line doesnt exist on windows fmpeg api's verson

        if (ret == AVERROR(EAGAIN))
            break;

        if (ret == AVERROR_EOF)
            break;

        if (ret < 0)
        {
            char err[256];
            av_strerror(ret, err, sizeof(err));

            qCCritical(_ffmpeg) << "avcodec_receive_frame failed. receive_frame:" << err;

            break;
        }

        if (!m_sws)
        {
            m_sws = sws_getContext(
                m_frame->width,
                m_frame->height,
                (AVPixelFormat)m_frame->format,

                m_frame->width,
                m_frame->height,
                AV_PIX_FMT_RGB24,

                SWS_FAST_BILINEAR,

                nullptr,
                nullptr,
                nullptr);
        }

        QImage image(
            m_frame->width,
            m_frame->height,
            QImage::Format_RGB888);

        uint8_t *dstData[4] =
            {
                image.bits(),
                nullptr,
                nullptr,
                nullptr
            };

        int dstLinesize[4] =
            {
                image.bytesPerLine(),
                0,
                0,
                0
            };

        sws_scale(
            m_sws,

            m_frame->data,
            m_frame->linesize,

            0,
            m_frame->height,

            dstData,
            dstLinesize);

        emit imageReady(image.copy());
    }
}
