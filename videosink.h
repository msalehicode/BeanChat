#pragma once

#include <QObject>
#include <QVideoFrame>
#include <QMutex>
class VideoSink : public QObject
{
    Q_OBJECT

public:
    explicit VideoSink(QObject *parent = nullptr)
        : QObject(parent)
    {
        qDebug()
        << "VideoSink thread"
        << thread();
    }
    QVideoFrame frame() const
    {
        QMutexLocker locker(&m_mutex);

        qDebug() << "returning frame"
                 << m_frame.isValid();

        return m_frame;
    }

    QImage image() const
    {
        QMutexLocker locker(&m_mutex);
        return m_image;
    }

public slots:
    void setFrame(const QVideoFrame &frame)
    {
        QMutexLocker locker(&m_mutex);

        qDebug() << "setFrame"
                 << frame.isValid();

        m_frame = frame;

        emit frameChanged();
    }
    void setImage(const QImage &image)
    {
        qDebug() << "setImage: "
        << image.size()
        << image.format()
        << image.bytesPerLine();

        {
            QMutexLocker locker(&m_mutex);
            m_image = image.copy();
        }

        emit frameChanged();
    }

signals:
    void frameChanged();

private:
    mutable QMutex m_mutex;
    QVideoFrame m_frame;
    QImage m_image;
};
