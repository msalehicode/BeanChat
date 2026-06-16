#include "audiospeaker.h"

#include <QDebug>

AudioSpeaker::AudioSpeaker(QObject *parent)
    : QObject(parent)
{
}

AudioSpeaker::~AudioSpeaker()
{
    stop();
}

bool AudioSpeaker::start()
{
    QMediaDevices devices;

    m_format.setSampleRate(48000);
    m_format.setChannelCount(1);
    m_format.setSampleFormat(QAudioFormat::Int16);

    if (!m_format.isValid()) {
        qDebug() << "Invalid audio format";
        return false;
    }

    m_sink = new QAudioSink(devices.defaultAudioOutput(), m_format, this);
    m_device = m_sink->start();

    if (!m_device) {
        qDebug() << "Failed to start audio sink";
        return false;
    }

    return true;
}


void AudioSpeaker::stop()
{
    if (m_sink) {
        m_sink->stop();
        delete m_sink;
        m_sink = nullptr;
    }

    m_device = nullptr;
}


void AudioSpeaker::playPcm(const QByteArray &pcm)
{
    if (!m_device)
        return;

    m_device->write(pcm);
}
