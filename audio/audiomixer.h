#pragma once

#include <QObject>
#include <QHash>
#include <QMutex>
#include <QByteArray>
#include <QTimer>
#include <QQueue>
#include <QDebug>


class AudioMixer : public QObject
{
    Q_OBJECT

public:
    explicit AudioMixer(QObject *parent = nullptr);

    void addVoice(
        quint64 userId,
        const QByteArray &pcm,
        int volume);

signals:
    void mixedFrameReady(QByteArray pcm);

private slots:
    void mix();

private:

    struct VoiceFrame
    {
        QByteArray pcm;
        int volume = 100;
    };

    QHash<quint64, QQueue<VoiceFrame>> m_pendingFrames;

    QMutex m_mutex;

    QTimer m_mixTimer;
};
