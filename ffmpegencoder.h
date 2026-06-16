// #pragma once

// #include <QObject>

// extern "C" {
// #include <libavcodec/avcodec.h>
// #include <libavformat/avformat.h>
// #include <libswscale/swscale.h>
// }

// #include <QVideoFrame>


// class FFmpegEncoder : public QObject
// {
//     Q_OBJECT
// public:
//     explicit FFmpegEncoder(QObject *parent = nullptr);
// public:
//     bool init(int width, int height);
//     void encodeFrame(const QVideoFrame &frame);
//     void close();

// private:
//     AVCodecContext *codecCtx = nullptr;
//     AVFormatContext *fmtCtx = nullptr;
//     AVFrame *avFrame = nullptr;
//     SwsContext *swsCtx = nullptr;
//     int frameIndex = 0;

// signals:
// };
