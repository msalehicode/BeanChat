#pragma once

#include <QObject>
#include <QVideoFrame>
#include <QMutex>

// #define D_PRINT_VIDEOSINK_INFO

class VideoSink : public QObject
{
    Q_OBJECT

public:
    explicit VideoSink(QObject *parent = nullptr)
        : QObject(parent)
    {
#ifdef D_PRINT_VIDEOSINK_INFO
        qDebug()
        << "VideoSink thread"
        << thread();
#endif
    }
    QVideoFrame frame() const
    {
        QMutexLocker locker(&m_mutex);

#ifdef D_PRINT_VIDEOSINK_INFO
        qDebug() << "returning frame"
                 << m_frame.isValid();
#endif

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

#ifdef D_PRINT_VIDEOSINK_INFO
        qDebug() << "setFrame"
                 << frame.isValid();
#endif

        m_frame = frame;

        emit frameChanged();
    }
    void setImage(const QImage &image)
    {
#ifdef D_PRINT_VIDEOSINK_INFO
        qDebug() << "setImage: "
        << image.size()
        << image.format()
        << image.bytesPerLine();
#endif

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
