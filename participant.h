#pragma once

#include <QObject>
#include "videosink.h"

class Participant : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint64 id
                   READ id
                       CONSTANT)

    Q_PROPERTY(QString username
                   READ username
                       CONSTANT)

    Q_PROPERTY(VideoSink* videoSink
                   READ videoSink
                       CONSTANT)

    Q_PROPERTY(bool isTalking
                   READ isTalking
                       CONSTANT)

    Q_PROPERTY(bool isDeafened
                   READ isDeafened
                       CONSTANT)


    Q_PROPERTY(bool isCameraOpen
                   READ isCameraOpen
                       CONSTANT)

public:
    explicit Participant(quint64 id,
        const QString &username, bool isTalking, bool isDeafened, bool isCameraOpen,
        QObject *parent = nullptr)
        : QObject(parent),
        m_id(id), m_username(username), m_isCameraOpen(isCameraOpen), m_isTalking(isTalking), m_isDeafened(isDeafened)
    {
        m_videoSink = new VideoSink(this);
    }


    void setUserName(const QString& name)
    {
        if(m_username==name)
            return;

        m_username = name;
    }
    QString username() const
    {
        return m_username;
    }

    quint64 id() const
    {
        return m_id;
    }

    VideoSink *videoSink()
    {
        return m_videoSink;
    }


    void setIsCameraOpen(bool status)
    {
        if(m_isCameraOpen==status)
            return;

        m_isCameraOpen=status;
    }
    bool isCameraOpen() const
    {
        return m_isCameraOpen;
    }

    void setIsTalking(bool status)
    {
        if(m_isTalking==status)
            return;

        m_isTalking=status;
    }
    bool isTalking() const
    {
        return m_isTalking;
    }

    void setIsDeafened(bool status)
    {
        if(m_isDeafened==status)
            return;

        m_isDeafened=status;
    }
    bool isDeafened() const
    {
        return m_isDeafened;
    }

private:
    quint64 m_id;
    QString m_username;
    bool m_isTalking;
    bool m_isDeafened;
    bool m_isCameraOpen;
    VideoSink* m_videoSink = nullptr;
};
