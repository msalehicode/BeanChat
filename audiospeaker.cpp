#include "audiospeaker.h"


AudioSpeaker::AudioSpeaker(QObject *parent)
    : QObject(parent)
{
    //first fetch inputs..
    refreshAudioOutputs();


    //connect for later changes
    connect(&m_mediaDevices, &QMediaDevices::audioOutputsChanged, this, [this]()
            {
                qDebug() << "Speaker hardware change detected!";

                // 1. Refresh your local list
                refreshAudioOutputs();

                // 2. Decide what to do if the active mic was removed
                if (m_currentAudioOutput >= m_audioOutputs.size())
                {
                    // If the current speaker is gone, reset to default 0
                    setCurrentAudioOutput(0);
                }
            });

}

AudioSpeaker::~AudioSpeaker()
{
    stop();
}

QString AudioSpeaker::audioOutputId(int index) const
{
    if (index < 0 || index >= m_audioOutputs.size())
        return {};

    return QString::fromUtf8(m_audioOutputs[index].id());
}

int AudioSpeaker::audioOutputIndexFromId(const QString &id) const
{
    for (int i = 0; i < m_audioOutputs.size(); ++i)
    {
        if (QString::fromUtf8(m_audioOutputs[i].id()) == id)
            return i;
    }

    return -1;
}

bool AudioSpeaker::start()
{
    // Check if the current index is actually valid
    if (m_audioOutputs.isEmpty() || m_currentAudioOutput >= m_audioOutputs.size()) {
        qWarning() << "Invalid audio output index!";
        return false;
    }

    // Clean up existing capture before starting new one
    if (m_sink)
    {
        stop();
    }


    m_format.setSampleRate(48000);
    m_format.setChannelCount(1);
    m_format.setSampleFormat(QAudioFormat::Int16);

    if (!m_format.isValid())
    {
        qDebug() << "Invalid audio format";
        return false;
    }

    m_sink = new QAudioSink(m_audioOutputs[m_currentAudioOutput], m_format, this);
    m_device = m_sink->start();

    m_started=true;

    if (!m_device)
    {
        qDebug() << "Failed to start audio sink";
        return false;
    }

    emit currentAudioOutputChanged(&m_audioOutputs[m_currentAudioOutput]);
    return true;
}


void AudioSpeaker::stop()
{
    if (m_sink) {
        m_sink->stop();
        delete m_sink;
        m_sink = nullptr;
    }

    m_started=false;

    m_device = nullptr;
}


void AudioSpeaker::playPcm(const QByteArray &pcm)
{
    if (!m_device)
        return;

    m_device->write(pcm);
}

bool AudioSpeaker::started() const
{
    return m_started;
}

int AudioSpeaker::devicesCount() const
{
    return m_audioOutputs.count();
}




QStringList AudioSpeaker::audioOutputNames() const
{
    // 1. Create a local list (this is NOT a class member, so it's allowed in const)
    QStringList names;

    // 2. Fill it with the descriptions
    for (const auto &device : m_audioOutputs)
    {
        names << device.description();
    }

    // 3. Return the local list
    return names;

}





int AudioSpeaker::currentAudioOutput() const
{
    return m_currentAudioOutput;
}

void AudioSpeaker::setCurrentAudioOutput(int newCurrentAudioOutput)
{
    if (m_currentAudioOutput == newCurrentAudioOutput)
        return;

    m_currentAudioOutput = newCurrentAudioOutput;
    emit currentAudioOutputChanged();

    if (m_sink)
    {
        qDebug() << "Switching output device to:" << m_audioOutputs[m_currentAudioOutput].description();
        start(); // This calls the start() logic which handles stopping the old one
    }
}

void AudioSpeaker::setAudioOutputs(QList<QAudioDevice> newList)
{
    if(m_audioOutputs == newList)
        return;

    m_audioOutputs=newList;

    emit audioOutputsChanged();
}

void AudioSpeaker::refreshAudioOutputs()
{
    qDebug() << "=== OUTPUT DEVICES ===";
    QList<QAudioDevice> inputs = QMediaDevices::audioOutputs();
    for (int i = 0; i < inputs.size(); i++)
    {
        qDebug() << i
                 << inputs[i].description();
    }

    setAudioOutputs(inputs);
}

