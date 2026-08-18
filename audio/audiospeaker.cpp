#include "audiospeaker.h"
#include "logging/loggingcategories.h"

AudioSpeaker::AudioSpeaker(QObject *parent)
    : QObject(parent)
{
    //first fetch inputs..
    refreshAudioOutputs();


    //connect for later changes
    connect(&m_mediaDevices, &QMediaDevices::audioOutputsChanged, this, [this]()
            {
                qCInfo(_speaker) << "Speaker hardware change detected!";

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
        qCCritical(_speaker) <<  "Invalid audio output index!";
        return false;
    }

    // Clean up existing capture before starting new one
    if (m_sink)
    {
        qCInfo(_speaker) << "before start speaker, let's stop old one.";
        stop();
    }


    m_format.setSampleRate(SPEAKER_DEFAULT_SAMPLE_RATE);
    m_format.setChannelCount(SPEAKER_DEFAULT_CHANNEL);
    m_format.setSampleFormat(QAudioFormat::Int16);
    qCDebug(_speaker) << "speaker format smapleRate="
                     << m_format.sampleRate()
                     << " channel="
                     << m_format.channelCount();

    if (!m_format.isValid())
    {
        qCCritical(_speaker) << "Invalid audio format";
        return false;
    }

    m_sink = new QAudioSink(m_audioOutputs[m_currentAudioOutput], m_format, this);

    // m_sink->setBufferSize(16384);
    m_sink->setBufferSize(32768);
    qCInfo(_speaker) << "speaker sink buffer size= " << m_sink->bufferSize();

    m_device = m_sink->start();

    m_started=true;

    if (!m_device)
    {
        qCCritical(_speaker) <<  "Failed to start audio sink";
        return false;
    }

    emit currentAudioOutputChanged(&m_audioOutputs[m_currentAudioOutput]);
    return true;
}


void AudioSpeaker::stop()
{
    qCInfo(_speaker) << "stopping speaker.";
    if (m_sink)
    {
        m_sink->stop();
        delete m_sink;
        m_sink = nullptr;
    }

    m_started=false;

    m_device = nullptr;
}

int AudioSpeaker::volume() const
{
    return m_volume;
}

void AudioSpeaker::setVolume(int newVolume)
{
    if (m_volume == newVolume)
        return;
    m_volume = newVolume;

    qCInfo(_speaker) << "set overall volume to " << newVolume;
    emit volumeChanged();
}



// void AudioSpeaker::playPcm(const QByteArray &pcm)
// {
//     if (!m_device)
//         return;

//     qint64 written = m_device->write(pcm);
// #if D_PRINT_SPEAKER_INFO
//     if (written != pcm.size())
//     {
//         qDebug()
//         << "SHORT WRITE"
//         << written
//         << "/"
//         << pcm.size();
//     }
//     qDebug()
//         << "write"
//         << pcm.size()
//         << "returned"
//         << m_device->write(pcm);
// #endif
// }

void AudioSpeaker::playPcm(const QByteArray &pcm)
{
    if (!m_device || pcm.isEmpty())
        return;

    if (qFuzzyCompare(m_volume, 1.0f))
    {
        m_device->write(pcm);
        return;
    }

    QByteArray adjusted = pcm;

    qint16 *samples =
        reinterpret_cast<qint16 *>(adjusted.data());

    const int sampleCount =
        adjusted.size() / sizeof(qint16);

    for (int i = 0; i < sampleCount; ++i)
    {
        int sample = static_cast<int>(samples[i] * m_volume);

        sample = qBound(-32768, sample, 32767);

        samples[i] = static_cast<qint16>(sample);
    }

    m_device->write(adjusted);
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

int AudioSpeaker::defaultAudioOutputIndex() const
{
    QAudioDevice defaultDevice = QMediaDevices::defaultAudioOutput();

    for (int i = 0; i < m_audioOutputs.size(); ++i)
    {
        if (m_audioOutputs[i].id() == defaultDevice.id())
            return i;
    }

    return -1;
}

void AudioSpeaker::setCurrentAudioOutput(int newCurrentAudioOutput)
{
    if (m_currentAudioOutput == newCurrentAudioOutput)
        return;

    m_currentAudioOutput = newCurrentAudioOutput;
    emit currentAudioOutputChanged();

    if (m_sink)
    {
        qCInfo(_speaker) <<  "Switching output device to:" << m_audioOutputs[m_currentAudioOutput].description();
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
    qCInfo(_speaker) <<  "=== OUTPUT DEVICES ===";
    QList<QAudioDevice> inputs = QMediaDevices::audioOutputs();
    for (int i = 0; i < inputs.size(); i++)
    {
        qDebug() << i
                 << inputs[i].description();
    }

    setAudioOutputs(inputs);
}

