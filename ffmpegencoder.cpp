// #include "ffmpegencoder.h"

// FFmpegEncoder::FFmpegEncoder(QObject *parent)
//     : QObject{parent}
// {}

// bool FFmpegEncoder::init(int w, int h)
// {
//     avformat_alloc_output_context2(&fmtCtx, nullptr, nullptr, "out.mp4");

//     const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
//     codecCtx = avcodec_alloc_context3(codec);

//     codecCtx->width = w;
//     codecCtx->height = h;
//     codecCtx->time_base = {1, 30};
//     codecCtx->framerate = {30, 1};
//     codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
//     codecCtx->bit_rate = 2000000;

//     avcodec_open2(codecCtx, codec, nullptr);

//     avFrame = av_frame_alloc();
//     avFrame->format = codecCtx->pix_fmt;
//     avFrame->width = w;
//     avFrame->height = h;
//     av_frame_get_buffer(avFrame, 32);

//     swsCtx = sws_getContext(
//         w, h,
//         AV_PIX_FMT_RGB24,   // input from QImage
//         w, h,
//         AV_PIX_FMT_YUV420P,
//         SWS_BILINEAR,
//         nullptr, nullptr, nullptr
//         );

//     return true;
// }

// void FFmpegEncoder::encodeFrame(const QVideoFrame &frame)
// {
//     QVideoFrame f(frame);
//     if (!f.map(QVideoFrame::ReadOnly))
//         return;

//     QImage img = f.toImage().convertToFormat(QImage::Format_RGB888);

//     f.unmap();

//     const uint8_t *srcData[1] = { img.bits() };
//     int srcLinesize[1] = { static_cast<int>(img.bytesPerLine()) };

//     sws_scale(
//         swsCtx,
//         srcData,
//         srcLinesize,
//         0,
//         codecCtx->height,
//         avFrame->data,
//         avFrame->linesize
//         );

//     avFrame->pts = frameIndex++;

//     avcodec_send_frame(codecCtx, avFrame);

//     AVPacket pkt;
//     av_init_packet(&pkt);

//     while (avcodec_receive_packet(codecCtx, &pkt) == 0) {
//         av_write_frame(fmtCtx, &pkt);
//         av_packet_unref(&pkt);
//     }
// }

// void FFmpegEncoder::close()
// {
//     avcodec_send_frame(codecCtx, nullptr);

//     AVPacket pkt;
//     while (avcodec_receive_packet(codecCtx, &pkt) == 0) {
//         av_write_frame(fmtCtx, &pkt);
//         av_packet_unref(&pkt);
//     }

//     av_write_trailer(fmtCtx);
// }
