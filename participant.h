#pragma once

#include <QObject>
#include "videosink.h"

class Participant : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString username
                   READ username
                       CONSTANT)

    Q_PROPERTY(VideoSink* videoSink
                   READ videoSink
                       CONSTANT)

    Q_PROPERTY(bool isTalking
                   READ isTalking
                       CONSTANT)


    Q_PROPERTY(bool isCameraOpen
                   READ isCameraOpen
                       CONSTANT)

public:
    explicit Participant(
        const QString &username, bool isTalking, bool isCameraOpen,
        QObject *parent = nullptr)
        : QObject(parent),
        m_username(username), m_isCameraOpen(isCameraOpen), m_isTalking(isTalking)
    {
        m_videoSink = new VideoSink(this);
    }

    QString username() const
    {
        return m_username;
    }

    VideoSink *videoSink()
    {
        return m_videoSink;
    }

    bool isCameraOpen() const
    {
        return m_isCameraOpen;
    }

    bool isTalking() const
    {
        return m_isTalking;
    }

private:
    QString m_username;
    bool m_isTalking;
    bool m_isCameraOpen;
    VideoSink* m_videoSink = nullptr;
};
