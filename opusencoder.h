#pragma once

#include <opus/opus.h>
#include <QByteArray>

class OpusEncoderWrapper
{
public:
    OpusEncoderWrapper();
    ~OpusEncoderWrapper();

    QByteArray encode(const short* pcm, int samples);

private:
    OpusEncoder *encoder = nullptr;
};
