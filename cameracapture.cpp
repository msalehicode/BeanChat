#include "cameracapture.h"
#include <QMediaDevices>
#include <QDebug>

CameraCapture::CameraCapture(QObject *parent)
    : QObject(parent)
{
}

void CameraCapture::start()
{
    auto device = QMediaDevices::defaultVideoInput();

    camera = new QCamera(device, this);

    sink = new QVideoSink(this);

    connect(sink, &QVideoSink::videoFrameChanged,
            this, [this](const QVideoFrame &frame)
            {
                if (!frame.isValid())
                    return;

                QVideoFrame copy(frame);

                if (!copy.map(QVideoFrame::ReadOnly))
                    return;

                QImage img = copy.toImage();

                copy.unmap();

                if (!img.isNull())
                {
                    m_frame = img;
                    emit frameChanged();
                }

                emit frameReady(frame);
            });

    session.setCamera(camera);
    session.setVideoSink(sink);

    camera->start();
}

void CameraCapture::stop()
{
    camera->stop();
    // delete camera;
    // delete sink;
}
