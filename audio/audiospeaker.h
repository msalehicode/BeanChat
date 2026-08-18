#pragma once

#include <QObject>
#include <QAudioSink>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QIODevice>
#include <QDebug>

class AudioSpeaker : public QObject
{
    Q_OBJECT

public:
    explicit AudioSpeaker(QObject *parent = nullptr);
    ~AudioSpeaker();

    bool start();
    void stop();


    int volume() const;
    void setVolume(int newVolume);

    QStringList audioOutputNames() const;
    int currentAudioOutput() const;
    int defaultAudioOutputIndex() const;
    void setCurrentAudioOutput(int newCurrentAudioOutput);
    void setAudioOutputs(QList<QAudioDevice> newList);
    bool started() const;

    int devicesCount() const; //to know how many devices found.
    Q_INVOKABLE QString audioOutputId(int index) const; //to get device id(system id) in qml
    int audioOutputIndexFromId(const QString &id) const;
signals:
    void audioOutputsChanged();

    void currentAudioOutputChanged();

    void currentAudioOutputChanged(QAudioDevice* output);

    void volumeChanged();
public slots:
    void playPcm(const QByteArray &pcm);

private:
    bool m_started=false;


    void refreshAudioOutputs();
    QAudioSink *m_sink = nullptr;
    QIODevice *m_device = nullptr;

    QAudioFormat m_format;
    int m_volume=SPEAKER_DEFAULT_OVERALL_VOLUME;

    QList<QAudioDevice> m_audioOutputs;
    QMediaDevices m_mediaDevices;
    int m_currentAudioOutput=0;
    QStringList m_audioOutputNames;

    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(QStringList audioOutputNames READ audioOutputNames NOTIFY audioOutputsChanged)
    Q_PROPERTY(int currentAudioOutput READ currentAudioOutput WRITE setCurrentAudioOutput NOTIFY currentAudioOutputChanged FINAL)
};
