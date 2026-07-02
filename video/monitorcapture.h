#pragma once

#include <QObject>
#include <QScreenCapture>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QVideoFrame>
#include <QScreen>
#include <QGuiApplication>

class MonitorCapture : public QObject
{
    Q_OBJECT

public:
    explicit MonitorCapture(QObject *parent = nullptr);

    QStringList monitors() const;

    bool selectMonitor(int index);

    void start();
    void stop();

signals:
    void frameReady(const QVideoFrame &frame);
    void errorOccurred(const QString &error);

private:
    QScreenCapture *m_capture;
    QMediaCaptureSession m_session;
    QVideoSink *m_sink;
};
