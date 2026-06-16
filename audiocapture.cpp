#include "audiocapture.h"

#include <QAudioFormat>
#include <QDebug>

AudioCapture::AudioCapture(QObject *parent)
    : QObject(parent)
{
}


void AudioCapture::start()
{
    auto inputDevice = QMediaDevices::defaultAudioInput();

    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    m_audioSource = new QAudioSource(inputDevice, format, this);
    m_device = m_audioSource->start();

    connect(m_device, &QIODevice::readyRead, this, [this]()
            {
                QByteArray pcm = m_device->readAll();

                // 1. Emit raw PCM (this is what we will send later)
                emit pcmReady(pcm);

                // 2. Calculate voice level
                const qint16 *samples =
                    reinterpret_cast<const qint16*>(pcm.constData());

                int count = pcm.size() / sizeof(qint16);

                qint64 sum = 0;
                for (int i = 0; i < count; i++)
                    sum += std::abs(samples[i]);

                double level = count ? double(sum) / count : 0;

                emit levelChanged(level);
            });
}
