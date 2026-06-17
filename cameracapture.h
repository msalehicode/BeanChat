#pragma once

#include <QObject>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QVideoFrame>
#include <QImage>

class CameraCapture : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QImage frame READ frame NOTIFY frameChanged)

public:
    explicit CameraCapture(QObject *parent = nullptr);
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    QImage frame() const { return m_frame; }

signals:
    void frameChanged();
    void frameReady(const QVideoFrame &frame);
    void imageReady(const QImage &image);

private:
    QCamera *camera = nullptr;
    QMediaCaptureSession session;
    QVideoSink *sink = nullptr;

    QImage m_frame;
};


