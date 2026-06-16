#include "opusencoder.h"

#include <stdexcept>

OpusEncoderWrapper::OpusEncoderWrapper()
{
    int error;

    encoder = opus_encoder_create(
        48000,      // sample rate
        1,          // mono
        OPUS_APPLICATION_VOIP,
        &error
        );

    if (error != OPUS_OK)
        throw std::runtime_error("Opus init failed");

    opus_encoder_ctl(encoder, OPUS_SET_BITRATE(24000));
}

OpusEncoderWrapper::~OpusEncoderWrapper()
{
    opus_encoder_destroy(encoder);
}

QByteArray OpusEncoderWrapper::encode(const short *pcm, int samples)
{
    unsigned char buffer[4000];

    int bytes = opus_encode(
        encoder,
        pcm,
        samples,
        buffer,
        sizeof(buffer)
        );

    if (bytes < 0)
        return {};

    return QByteArray((char*)buffer, bytes);
}
