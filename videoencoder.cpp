// #include "videoencoder.h"

// VideoEncoder::VideoEncoder(QObject *parent)
//     : QObject{parent}
// {}


// void VideoEncoder::encodeFrame(const QVideoFrame &frame)
// {
//     if (!frame.isValid())
//         return;

//     QVideoFrame f(frame);

//     if (!f.map(QVideoFrame::ReadOnly)) {
//         qDebug() << "encode map failed";
//         return;
//     }

//     // raw access
//     const uchar *data = f.bits(0);
//     int bytes = f.mappedBytes();

//     // OR convert if encoder needs RGB
//     QImage img = f.toImage();

//     f.unmap();

//     // send to encoder (FFmpeg / hardware encoder)
//     FFmpegEncoder encoder;
//     encoder.init(640, 480);
// }
