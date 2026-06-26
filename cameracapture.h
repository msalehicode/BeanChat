#pragma once

#include <QObject>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QVideoFrame>
#include <QImage>
#include <QMediaDevices>
#include <QDebug>
#include <QBuffer>
#include <QByteArray>
#include <QElapsedTimer>

class CameraCapture : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QImage frame READ frame NOTIFY frameChanged)

public:
    explicit CameraCapture(QObject *parent = nullptr);
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    QImage frame() const { return m_frame; }


    QStringList cameraInputsNames() const;

    int currentCameraInput() const;
    void setCurrentCameraInput(int newCurrentCameraInput);

    int devicesCount() const; //to know how many device found.
    Q_INVOKABLE QString cameraIntputId(int index) const; //to get device id(system id) in qml
    int cameraInputIndexFromId(const QString &id) const;
signals:
    void frameChanged();
    void frameReady(const QVideoFrame &frame);
    void imageReady(const QImage &image);
    void jpegReady(const QByteArray& jpegData); //send to socket

    void cameraInputsChanged();

    void currentCameraInputChanged();

private:
    QCamera *camera = nullptr;
    QMediaCaptureSession session;
    QVideoSink *sink = nullptr;

    QImage m_frame;

    int m_currentCameraInput=0; //hold selected input / current

    QList<QCameraDevice> m_cameraInputs; //hold list of inputs to know which index user has picked
    QMediaDevices m_cameraDevices; //to notify input changes and list
    QStringList m_cameraInputsNames;
    Q_PROPERTY(QStringList cameraInputsNames READ cameraInputsNames NOTIFY cameraInputsChanged)
    void refreshCameraInputs();
    void setCameraInputs(QList<QCameraDevice> newList);
    Q_PROPERTY(int currentCameraInput READ currentCameraInput WRITE setCurrentCameraInput NOTIFY currentCameraInputChanged FINAL)
};


