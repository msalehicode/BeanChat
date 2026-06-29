#pragma once

#include <QObject>
#include <QByteArray>

#include <opus/opus.h>

class OpusCodec : public QObject
{
    Q_OBJECT

public:
    explicit OpusCodec(QObject *parent = nullptr);
    ~OpusCodec();

    bool initialize(
        int sampleRate = OPUS_DEFAULT_SAMPLE_RATE,
        int channels = OPUS_DEFAULT_CHANNELS,
        int bitrate = OPUS_DEFAULT_BITRATE);

    void shutdown();

    bool isInitialized() const;

    QByteArray encode(const QByteArray &pcm);

    QByteArray decode(const QByteArray &opusData);

    int sampleRate() const;
    int channels() const;
    int frameSize() const;
    int bitrate() const;

    void setBitrate(int bitrate);

private:
    OpusEncoder *m_encoder = nullptr;
    OpusDecoder *m_decoder = nullptr;

    int m_sampleRate = OPUS_DEFAULT_SAMPLE_RATE;
    int m_channels = OPUS_DEFAULT_CHANNELS;

    // 20 ms @ 48kHz
    int m_frameSize = OPUS_DEFAULT_FRAMESIZE;

    int m_bitrate = OPUS_DEFAULT_BITRATE;
};
