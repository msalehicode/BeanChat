#pragma once

#include <QObject>
#include <QHash>

#include "audio/opuscodec.h"

class VoiceWorker : public QObject
{
    Q_OBJECT

public:
    explicit VoiceWorker(QObject *parent = nullptr);
    ~VoiceWorker();
public slots:

    // Remote voice
    void decode(quint64 senderId,
                const QByteArray &opusData);

    void removeDecoder(quint64 senderId);
    void clearDecoders();

    // Local microphone
    void encode(const QByteArray &pcm);

signals:

    void pcmDecoded(quint64 senderId,
                    QByteArray pcm);

    void opusEncoded(QByteArray opusData);

private:

    QHash<quint64, OpusCodec*> m_decoders;

    OpusCodec m_encoder;
    QByteArray m_encodePcmBuffer;

};
