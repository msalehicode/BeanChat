#pragma once

#include <QObject>
#include <QByteArray>
#include <QImage>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
}

class FFmpegDecoder : public QObject
{
    Q_OBJECT

public:
    explicit FFmpegDecoder(QObject *parent = nullptr);
    ~FFmpegDecoder();

    bool open();
    void close();

    void decode(const QByteArray &packet);

signals:
    void imageReady(const QImage &image);

private:
    AVCodecContext *m_codec = nullptr;
    AVFrame *m_frame = nullptr;
    AVPacket *m_packet = nullptr;

    SwsContext *m_sws = nullptr;
};
