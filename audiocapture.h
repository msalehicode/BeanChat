#pragma once

#include <QObject>
#include <QAudioSource>
#include <QMediaDevices>

#include <rnnoise.h>

class AudioCapture : public QObject
{
    Q_OBJECT

public:
    explicit AudioCapture(QObject *parent = nullptr);
    ~AudioCapture();
    void start();

    void stop();

    QStringList audioInputNames() const;


    int currentAudioInput() const;
    void setCurrentAudioInput(int newCurrentAudioInput);

    void setAudioInputs(QList<QAudioDevice> newList);
    bool rnnoiseStatus() const;
    void setRnnoiseStatus(bool newRnnoiseStatus);

    bool volumeGateStatus() const;
    void setVolumeGateStatus(bool newVolumeGateStatus);

    float volumeGateThreshold() const;
    void setVolumeGateThreshold(float newVolumeGateThreshold);

    float currentVolume() const;
    void setCurrentVolume(float newCurrentVolume);

signals:
    void pcmReady(QByteArray pcm);
    void levelChanged(double level);

    void audioInputsChanged();

    void currentAudioInputChanged();

    void rnnoiseStatusChanged();

    void volumeGateStatusChanged();

    void volumeGateThresholdChanged();

    void currentVolumeChanged();

private:
    bool m_rnnoiseStatus;
    DenoiseState *m_rnnoiseState = nullptr; // The RNNoise engine state
    QByteArray m_rnnoiseBuffer; // This will hold "leftover" samples

    bool m_gateOpen = false;
    bool m_volumeGateStatus;
    float m_currentVolume = 0.0f;
    float m_volumeGateThreshold=0.01f;

    void refreshAudioInputs();
    void volumeGateCheck(QByteArray& newData);


    QAudioSource *m_audioSource = nullptr;
    QIODevice *m_device = nullptr;
    QByteArray m_buffer;
    QList<QAudioDevice> m_audioInputs;
    QMediaDevices m_mediaDevices;
    int m_currentAudioInput=0;
    QStringList m_audioInputNames;

    Q_PROPERTY(QStringList audioInputNames READ audioInputNames NOTIFY audioInputsChanged)
    Q_PROPERTY(int currentAudioInput READ currentAudioInput WRITE setCurrentAudioInput NOTIFY currentAudioInputChanged FINAL)
    Q_PROPERTY(bool rnnoiseStatus READ rnnoiseStatus WRITE setRnnoiseStatus NOTIFY rnnoiseStatusChanged FINAL)
    Q_PROPERTY(bool volumeGateStatus READ volumeGateStatus WRITE setVolumeGateStatus NOTIFY volumeGateStatusChanged FINAL)
    Q_PROPERTY(float volumeGateThreshold READ volumeGateThreshold WRITE setVolumeGateThreshold NOTIFY volumeGateThresholdChanged FINAL)
    Q_PROPERTY(float currentVolume READ currentVolume WRITE setCurrentVolume NOTIFY currentVolumeChanged FINAL)
};
