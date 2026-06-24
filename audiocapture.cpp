#include "audiocapture.h"



AudioCapture::AudioCapture(QObject *parent)
    : QObject(parent)
{
    initialPushToTalkHotkey();

    // Initialize RNNoise
    m_rnnoiseState = rnnoise_create(nullptr);
    if (!m_rnnoiseState) {
        qDebug() << "Failed to create RNNoise state!";
    }

    //first fetch inputs..
    refreshAudioInputs();


    //connect for later changes
    connect(&m_mediaDevices, &QMediaDevices::audioInputsChanged, this, [this]()
    {
        qDebug() << "Audio hardware change detected!";

        // 1. Refresh your local list
        refreshAudioInputs();

        // 2. Decide what to do if the active mic was removed
        if (m_currentAudioInput >= m_audioInputs.size())
        {
            // If the current mic is gone, reset to default 0
            setCurrentAudioInput(0);
        }
    });

}

AudioCapture::~AudioCapture()
{
    if (m_rnnoiseState) {
        rnnoise_destroy(m_rnnoiseState);
    }

    stop();

}

void AudioCapture::start()
{
    // Check if the current index is actually valid
    if (m_audioInputs.isEmpty() || m_currentAudioInput >= m_audioInputs.size()) {
        qWarning() << "Invalid audio input index!";
        return;
    }

    // Clean up existing capture before starting new one
    if (m_audioSource)
    {
        stop();
    }


    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    m_audioSource = new QAudioSource(m_audioInputs[m_currentAudioInput], format, this);
    m_device = m_audioSource->start();

    m_started=true;

    connect(m_device, &QIODevice::readyRead, this, [this]()
            {
                QByteArray newData = m_device->readAll();

                if (newData.isEmpty())
                    return;

                if(pushToTalkStatus() && pushToTalkPressed()==false)
                    return;


                //for visual speaking value
                const qint16* samples = reinterpret_cast<const qint16*>(newData.constData());
                int sampleCount = newData.size() / sizeof(qint16);
                float sumSquare = 0.0f;
                for (int i = 0; i < sampleCount; ++i)
                {
                    float normalized = samples[i] / 32768.0f;
                    sumSquare += normalized * normalized;
                }
                float rms = std::sqrt(sumSquare / sampleCount);

                if(rms<=0.0)
                {
                    if(pushToTalkStatus()) //we want when user pressed button PTT even if no voice send empty to server.
                        emit pcmReady(newData);
                    else
                        return; //dont proceed because we cant hear anything. and PTT is off.
                }

                setCurrentVolume(rms); //qml is listening on this



                // --- FEATURE 1: VOLUME GATE (Standalone) ---
                if (volumeGateStatus())
                {
                    // Open threshold
                    const float openThreshold =
                        m_volumeGateThreshold;

                    // Close threshold (30% lower)
                    const float closeThreshold =
                        m_volumeGateThreshold * 0.7f;

                    if (!m_gateOpen)
                    {
                        if (rms > openThreshold)
                        {
                            m_gateOpen = true;
                        }
                    }
                    else
                    {
                        if (rms < closeThreshold)
                        {
                            m_gateOpen = false;
                        }
                    }

                    if (!m_gateOpen)
                    {
                        // QByteArray silence(newData.size(),0);
                        // emit pcmReady(silence);
                        if(!pushToTalkStatus()) //we want when user pressed button PTT even if no voice send empty to server.
                            return; //volume is under gate so dont go any further..
                    }
                    // else let other filters check data..
                }

                if(rnnoiseStatus())
                {
                    // Store incoming microphone bytes
                    m_rnnoiseBuffer.append(newData);

                    constexpr int FRAME_SIZE = 480;
                    constexpr int BYTES_PER_FRAME = FRAME_SIZE * sizeof(qint16);

                    while (m_rnnoiseBuffer.size() >= BYTES_PER_FRAME)
                    {
                        // Get one RNNoise frame (10ms @ 48kHz)
                        QByteArray frameData = m_rnnoiseBuffer.left(BYTES_PER_FRAME);

                        m_rnnoiseBuffer.remove(0, BYTES_PER_FRAME);

                        const qint16* inSamples = reinterpret_cast<const qint16*>(frameData.constData());

                        float rnnoiseFrame[FRAME_SIZE];

                        // RNNoise expects float PCM values in int16 range
                        for (int i = 0; i < FRAME_SIZE; ++i)
                            rnnoiseFrame[i] = static_cast<float>(inSamples[i]);

                        // Denoise
                        float vadProbability =
                            rnnoise_process_frame(
                                m_rnnoiseState,
                                rnnoiseFrame,
                                rnnoiseFrame);

                        // Debug
                        // qDebug() << "RNNoise VAD:" << vadProbability;

                        QByteArray processedData;
                        processedData.resize(BYTES_PER_FRAME);

                        qint16* outSamples =
                            reinterpret_cast<qint16*>(processedData.data());

                        for (int i = 0; i < FRAME_SIZE; ++i)
                        {
                            float sample = rnnoiseFrame[i];

                            if (sample > 32767.0f)
                                sample = 32767.0f;

                            if (sample < -32768.0f)
                                sample = -32768.0f;

                            outSamples[i] = static_cast<qint16>(sample);
                        }

                        // Optional hard mute when RNNoise thinks nobody is talking
                        if (vadProbability < rnnoiseValue())
                        {
                            //dont emit anything we cant hear anything..
                            processedData.fill(0);

                            if(pushToTalkStatus()) //we want when user pressed button PTT even if no voice send empty to server.
                                emit pcmReady(processedData);
                        }
                        else
                            emit pcmReady(processedData);
                    }

                }
                else
                {
                    emit pcmReady(newData);
                }
    });
}


void AudioCapture::stop()
{
    if (m_audioSource) {
        // Stop audio processing
        m_audioSource->stop();

        // Disconnect all signals from the device to prevent the lambda from firing after stop
        if (m_device) {
            m_device->disconnect();
        }

        m_started=false;

        // Clean up the object
        m_audioSource->deleteLater();
        m_audioSource = nullptr;
        m_device = nullptr;
    }
}

QStringList AudioCapture::audioInputNames() const
{
    // 1. Create a local list (this is NOT a class member, so it's allowed in const)
    QStringList names;

    // 2. Fill it with the descriptions
    for (const auto &device : m_audioInputs)
    {
        names << device.description();
    }

    // 3. Return the local list
    return names;

}

// QList<QAudioDevice> AudioCapture::audioInputs() const
// {
//     return m_audioInputs;
// }

int AudioCapture::currentAudioInput() const
{
    return m_currentAudioInput;
}

void AudioCapture::setCurrentAudioInput(int newCurrentAudioInput)
{
    if (m_currentAudioInput == newCurrentAudioInput)
        return;

    m_currentAudioInput = newCurrentAudioInput;
    emit currentAudioInputChanged();

    if (m_audioSource) {
        qDebug() << "Switching input device to:" << m_audioInputs[m_currentAudioInput].description();
        start(); // This calls the start() logic which handles stopping the old one
    }
}

void AudioCapture::setAudioInputs(QList<QAudioDevice> newList)
{
    if(m_audioInputs == newList)
        return;

    m_audioInputs=newList;

    emit audioInputsChanged();
}

bool AudioCapture::rnnoiseStatus() const
{
    return m_rnnoiseStatus;
}

void AudioCapture::setRnnoiseStatus(bool newRnnoiseStatus)
{
    if (m_rnnoiseStatus == newRnnoiseStatus)
        return;
    m_rnnoiseStatus = newRnnoiseStatus;
    emit rnnoiseStatusChanged();
}

bool AudioCapture::volumeGateStatus() const
{
    return m_volumeGateStatus;
}

void AudioCapture::setVolumeGateStatus(bool newVolumeGateStatus)
{
    if (m_volumeGateStatus == newVolumeGateStatus)
        return;
    m_volumeGateStatus = newVolumeGateStatus;
    emit volumeGateStatusChanged();
}

float AudioCapture::volumeGateThreshold() const
{
    return m_volumeGateThreshold;
}

void AudioCapture::setVolumeGateThreshold(float newVolumeGateThreshold)
{
    if (qFuzzyCompare(m_volumeGateThreshold, newVolumeGateThreshold))
        return;
    m_volumeGateThreshold = newVolumeGateThreshold;
    emit volumeGateThresholdChanged();
}

float AudioCapture::currentVolume() const
{
    return m_currentVolume;
}

void AudioCapture::setCurrentVolume(float newCurrentVolume)
{
    if (qFuzzyCompare(m_currentVolume, newCurrentVolume))
        return;
    m_currentVolume = newCurrentVolume;
    emit currentVolumeChanged();
}

float AudioCapture::rnnoiseValue() const
{
    return m_rnnoiseValue;
}

void AudioCapture::setRnnoiseValue(float newRnnoiseValue)
{
    if (qFuzzyCompare(m_rnnoiseValue, newRnnoiseValue))
        return;
    m_rnnoiseValue = newRnnoiseValue;
    emit rnnoiseValueChanged();
}

bool AudioCapture::pushToTalkStatus() const
{
    return m_pushToTalkStatus;
}

void AudioCapture::setPushToTalkStatus(bool newPushToTalkStatus)
{
    if (m_pushToTalkStatus == newPushToTalkStatus)
        return;
    m_pushToTalkStatus = newPushToTalkStatus;
    emit pushToTalkStatusChanged();
}

int AudioCapture::pushToTalkKey() const
{
    return m_pushToTalkKey;
}

int AudioCapture::pushToTalkModifiers() const
{
    return m_pushToTalkModifiers;
}

void AudioCapture::setPushToTalkModifiers(int newModifiers)
{
    if (m_pushToTalkModifiers == newModifiers)
        return;

    m_pushToTalkModifiers = newModifiers;

    emit pushToTalkModifiersChanged();
    emit pushToTalkKeyStringChanged();
}

void AudioCapture::setPushToTalkKey(int newPushToTalkKey)
{
    if (m_pushToTalkKey == newPushToTalkKey)
        return;

    m_pushToTalkKey = newPushToTalkKey;

    emit pushToTalkKeyChanged();
    emit pushToTalkKeyStringChanged();
}

bool AudioCapture::pushToTalkPressed() const
{
    return m_pushToTalkPressed;
}

void AudioCapture::setPushToTalkPressed(bool newPushToTalkPressed)
{
    if (m_pushToTalkPressed == newPushToTalkPressed)
        return;
    m_pushToTalkPressed = newPushToTalkPressed;
    emit pushToTalkPressedChanged();
}

QString AudioCapture::pushToTalkKeyString() const
{
    return QKeySequence(
               m_pushToTalkModifiers |
               m_pushToTalkKey
               ).toString(QKeySequence::NativeText);
}

bool AudioCapture::started() const
{
    return m_started;
}

void AudioCapture::refreshAudioInputs()
{
    qDebug() << "=== INPUT DEVICES ===";
    QList<QAudioDevice> inputs = QMediaDevices::audioInputs();
    for (int i = 0; i < inputs.size(); i++)
    {
        qDebug() << i
                 << inputs[i].description();
    }

    setAudioInputs(inputs);
}

bool AudioCapture::initialPushToTalkHotkey()
{
    if(m_pushToTalkHotkey) //remove old one if exists
        delete m_pushToTalkHotkey;

    //initial push to talk hotkey
    m_pushToTalkHotkey = new QHotkey(
        QKeySequence(
            m_pushToTalkModifiers |
            m_pushToTalkKey),
        true, // register immediately
        this);

    connect(
        m_pushToTalkHotkey,
        &QHotkey::activated,
        this,
        [this]()
        {
            // qDebug() << "PTT pressed";
            setPushToTalkPressed(true);
        });

    connect(
        m_pushToTalkHotkey,
        &QHotkey::released,
        this,
        [this]()
        {
            // qDebug() << "PTT released";
            setPushToTalkPressed(false);
        });

    // connect(
    //     m_pushToTalkHotkey,
    //     &QHotkey::activated,
    //     this,
    //     [this]()
    //     {
    //         qDebug() << "Active";
    //     });


    if(m_pushToTalkHotkey->isRegistered())
    {
        qDebug() << "push to talk Hotkey Registered fine.";
        return true;
    }
    qDebug() << "push to talk Hotkey failed to register.";
    return false;
}
