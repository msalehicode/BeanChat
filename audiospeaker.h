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

public slots:
    void playPcm(const QByteArray &pcm);

private:
    QAudioSink *m_sink = nullptr;
    QIODevice *m_device = nullptr;

    QAudioFormat m_format;
};
