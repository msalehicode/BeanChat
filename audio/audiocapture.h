#pragma once

#include <QObject>
#include <QAudioSource>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QDebug>

#include <rnnoise.h>

#include <QHotkey>

#include <QKeySequence>

class AudioCapture : public QObject
{
    Q_OBJECT

public:
    explicit AudioCapture(QObject *parent = nullptr);
    ~AudioCapture();
    Q_INVOKABLE void start();

    Q_INVOKABLE void stop();

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

    float rnnoiseValue() const;
    void setRnnoiseValue(float newRnnoiseValue);

    bool pushToTalkStatus() const;
    void setPushToTalkStatus(bool newPushToTalkStatus);

    int pushToTalkKey() const;
    void setPushToTalkKey(int newPushToTalkKey);
    int pushToTalkModifiers() const;
    void setPushToTalkModifiers(int newModifiers);

    bool pushToTalkPressed() const;
    void setPushToTalkPressed(bool newPushToTalkPressed);

    Q_INVOKABLE bool initialPushToTalkHotkey(); //to later after change modifier and key call this from QML

    QString pushToTalkKeyString() const;
    Q_INVOKABLE bool started() const;


    int devicesCount() const; //to know how many devices found.
    Q_INVOKABLE QString audioInputId(int index) const; //to get device id(system id) in qml
    int audioInputIndexFromId(const QString &id) const;
signals:
    void pcmReady(QByteArray pcm);
    void levelChanged(double level);

    void audioInputsChanged();

    void currentAudioInputChanged();

    void rnnoiseStatusChanged();

    void volumeGateStatusChanged();

    void volumeGateThresholdChanged();

    void currentVolumeChanged();

    void rnnoiseValueChanged();

    void pushToTalkStatusChanged();

    void pushToTalkKeyChanged();
    void pushToTalkModifiersChanged();

    void pushToTalkPressedChanged();

    void pushToTalkKeyStringChanged();

private:
    bool m_started=false; //hold status of mic

    //denoise stuff
    bool m_rnnoiseStatus;
    float m_rnnoiseValue= 0.15f;
    DenoiseState *m_rnnoiseState = nullptr; // The RNNoise engine state
    QByteArray m_rnnoiseBuffer; // This will hold "leftover" samples

    //push to talk
    bool m_pushToTalkStatus=false;
    bool m_pushToTalkPressed=false; //when hotkey pressed/released this flag will turn off/on
    QHotkey* m_pushToTalkHotkey=nullptr;
    int m_pushToTalkKey = MIC_DEFAULT_PTT_HOTKEY; //to hold key
    int m_pushToTalkModifiers = Qt::NoModifier; //to hold combination of key like ALT+V or ..

    //volume gate and visual current sound of microhpone
    float m_currentVolume = 0.0f;
    bool m_gateOpen = false; //for code, not status for user to set.
    bool m_volumeGateStatus; //status user turn off/on
    float m_volumeGateThreshold=0.01f; //gate limit/spot position

    //to set/update latest list of inputs
    void refreshAudioInputs();


    //microhpone stuff
    QAudioSource *m_audioSource = nullptr;
    QIODevice *m_device = nullptr;
    QList<QAudioDevice> m_audioInputs; //hold list of inputs
    QMediaDevices m_mediaDevices; //to notify input changes.
    int m_currentAudioInput=0; //hold selected input / current

    Q_PROPERTY(QString pushToTalkKeyString READ pushToTalkKeyString NOTIFY pushToTalkKeyStringChanged)
    Q_PROPERTY(QStringList audioInputNames READ audioInputNames NOTIFY audioInputsChanged) //to expose inputs list to QML without using any model.
    Q_PROPERTY(int currentAudioInput READ currentAudioInput WRITE setCurrentAudioInput NOTIFY currentAudioInputChanged FINAL)
    Q_PROPERTY(bool rnnoiseStatus READ rnnoiseStatus WRITE setRnnoiseStatus NOTIFY rnnoiseStatusChanged FINAL)
    Q_PROPERTY(bool volumeGateStatus READ volumeGateStatus WRITE setVolumeGateStatus NOTIFY volumeGateStatusChanged FINAL)
    Q_PROPERTY(float volumeGateThreshold READ volumeGateThreshold WRITE setVolumeGateThreshold NOTIFY volumeGateThresholdChanged FINAL)
    Q_PROPERTY(float currentVolume READ currentVolume WRITE setCurrentVolume NOTIFY currentVolumeChanged FINAL)
    Q_PROPERTY(float rnnoiseValue READ rnnoiseValue WRITE setRnnoiseValue NOTIFY rnnoiseValueChanged FINAL)
    Q_PROPERTY(bool pushToTalkStatus READ pushToTalkStatus WRITE setPushToTalkStatus NOTIFY pushToTalkStatusChanged FINAL)
    Q_PROPERTY(int pushToTalkKey READ pushToTalkKey WRITE setPushToTalkKey NOTIFY pushToTalkKeyChanged FINAL)
    Q_PROPERTY(int pushToTalkModifiers READ pushToTalkModifiers WRITE setPushToTalkModifiers NOTIFY pushToTalkModifiersChanged FINAL)
    Q_PROPERTY(bool pushToTalkPressed READ pushToTalkPressed WRITE setPushToTalkPressed NOTIFY pushToTalkPressedChanged FINAL)
};
