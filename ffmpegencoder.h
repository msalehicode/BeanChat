#pragma once

#include <QObject>
#include <QImage>
#include <QByteArray>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}

class FFmpegEncoder : public QObject
{
    Q_OBJECT

public:
    explicit FFmpegEncoder(QObject *parent = nullptr);
    ~FFmpegEncoder();


    bool open(
        int width,
        int height,
        int fps = 30,
        int bitrate = 500000,
        int gopSize = 30);



    void close();

    void encode(const QImage &image);

signals:
    void packetReady(const QByteArray &packet,
                     bool keyFrame);

private:
    AVCodecContext *m_codec = nullptr;
    AVFrame *m_frame = nullptr;
    AVPacket *m_packet = nullptr;
    SwsContext *m_sws = nullptr;

    int m_width = 0;
    int m_height = 0;

    qint64 m_pts = 0;
};
