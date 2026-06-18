#pragma once

#include <QObject>
#include <QAudioSink>
#include <QMediaDevices>
#include <QAudioFormat>
#include <QIODevice>

class AudioSpeaker : public QObject
{
    Q_OBJECT

public:
    explicit AudioSpeaker(QObject *parent = nullptr);
    ~AudioSpeaker();

    bool start();
    void stop();


    QStringList audioOutputNames() const;
    int currentAudioOutput() const;
    void setCurrentAudioOutput(int newCurrentAudioOutput);
    void setAudioOutputs(QList<QAudioDevice> newList);
signals:
    void audioOutputsChanged();

    void currentAudioOutputChanged();

public slots:
    void playPcm(const QByteArray &pcm);

private:
    void refreshAudioOutputs();
    QAudioSink *m_sink = nullptr;
    QIODevice *m_device = nullptr;

    QAudioFormat m_format;

    QList<QAudioDevice> m_audioOutputs;
    QMediaDevices m_mediaDevices;
    int m_currentAudioOutput=0;
    QStringList m_audioOutputNames;

    Q_PROPERTY(QStringList audioOutputNames READ audioOutputNames NOTIFY audioOutputsChanged)
    Q_PROPERTY(int currentAudioOutput READ currentAudioOutput WRITE setCurrentAudioOutput NOTIFY currentAudioOutputChanged FINAL)
};
