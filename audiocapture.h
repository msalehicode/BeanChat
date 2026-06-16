#pragma once

#include <QObject>
#include <QAudioSource>
#include <QMediaDevices>

class AudioCapture : public QObject
{
    Q_OBJECT

public:
    explicit AudioCapture(QObject *parent = nullptr);
    void start();

signals:
    void pcmReady(QByteArray pcm);
    void levelChanged(double level);

private:
    QAudioSource *m_audioSource = nullptr;
    QIODevice *m_device = nullptr;
    QByteArray m_buffer;
};
