#include "voiceworker.h"

VoiceWorker::VoiceWorker(QObject *parent)
    : QObject(parent)
{
    m_encoder.initialize();
}

VoiceWorker::~VoiceWorker()
{
    clearDecoders();

    m_encoder.shutdown();
}


void VoiceWorker::decode(quint64 senderId,
                              const QByteArray &opusData)
{
    OpusCodec *decoder = m_decoders.value(senderId, nullptr);

    if (!decoder)
    {
        decoder = new OpusCodec();

        decoder->initialize();

        m_decoders.insert(senderId, decoder);
    }

    QByteArray pcm = decoder->decode(opusData);

    if (!pcm.isEmpty())
    {
        emit pcmDecoded(senderId, pcm);
    }
}

void VoiceWorker::removeDecoder(quint64 senderId)
{
    OpusCodec *decoder = m_decoders.take(senderId);

    if (decoder)
    {
        decoder->shutdown();
        delete decoder;
    }
}

void VoiceWorker::clearDecoders()
{
    for (OpusCodec *decoder : m_decoders)
    {
        if (decoder)
        {
            decoder->shutdown();
            delete decoder;
        }
    }

    m_decoders.clear();
}

void VoiceWorker::encode(const QByteArray &pcm)
{
    m_encodePcmBuffer.append(pcm);

    constexpr int FRAME_BYTES = 960 * sizeof(qint16);

    // Encode every complete 20ms frame
    while (m_encodePcmBuffer.size() >= FRAME_BYTES)
    {
        QByteArray frame = m_encodePcmBuffer.left(FRAME_BYTES);

        m_encodePcmBuffer.remove(0, FRAME_BYTES);

        QByteArray opus = m_encoder.encode(frame);

        if (opus.isEmpty())
            continue;

        emit opusEncoded(opus);
    }
}
