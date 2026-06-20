#include "cameracapture.h"
#include <QMediaDevices>
#include <QDebug>
#include <QBuffer>
#include <QByteArray>
#include <QElapsedTimer>

// #define D_PRINT_CAMERA_INFO

CameraCapture::CameraCapture(QObject *parent)
    : QObject(parent)
{
}

void CameraCapture::start()
{
    auto device = QMediaDevices::defaultVideoInput();


#ifdef D_PRINT_CAMERA_INFO
    //test performace
    auto formats = device.videoFormats();
    for(const auto &fmt : formats)
    {
        qDebug()
        << "Resolution:"
        << fmt.resolution()
        << "FPS:"
        << fmt.maxFrameRate();
    }
    qDebug()
        << "Video input:"
        << QMediaDevices::defaultVideoInput().description();
#endif



    // camera = new QCamera(device, this);
    camera = new QCamera(device, this);

#ifdef D_PRINT_CAMERA_INFO
    for(const auto &fmt : device.videoFormats())
    {
        if(fmt.resolution() == QSize(640,480))
        {
            camera->setCameraFormat(fmt);

            qDebug()
                << "Selected"
                << fmt.resolution()
                << fmt.maxFrameRate();

            break;
        }
    }
#endif

    sink = new QVideoSink(this);

    connect(
        sink,
        &QVideoSink::videoFrameChanged,
        this,
        [this](const QVideoFrame &frame)
        {
            if (!frame.isValid())
                return;


#ifdef D_PRINT_CAMERA_INFO
            //test2 performace
            static QElapsedTimer timer;
            static int count = 0;

            if (!timer.isValid())
                timer.start();
            count++;
            if (timer.elapsed() >= 1000)
            {
                qDebug() << "Raw camera FPS:" << count;

                count = 0;
                timer.restart();
            }
#endif


            QImage img = frame.toImage();


            if (!img.isNull())
            {
                QByteArray jpgData;

                QBuffer buffer(&jpgData);
                buffer.open(QIODevice::WriteOnly);

                img.save(
                    &buffer,
                    "JPG",
                    60);

                emit jpegReady(jpgData);

#ifdef D_PRINT_CAMERA_INFO
                qDebug()
                    << "Resolution JPEG:"
                    << img.width()
                    << "x"
                    << img.height()
                    << "JPEG size:"
                    << jpgData.size()/1024.0
                    << "KB";
#endif

                m_frame = img;

                emit frameChanged();
                emit imageReady(img);

#ifdef D_PRINT_CAMERA_INFO
                static int count = 0;
                count++;

                static QElapsedTimer timer;
                if(!timer.isValid())
                    timer.start();

                if(timer.elapsed() > 1000)
                {
                    qDebug() << "camera FPS:" << count;
                    count = 0;
                    timer.restart();
                }
#endif
            }

            emit frameReady(frame);
        });

    session.setCamera(camera);
    session.setVideoSink(sink);

    camera->start();

#ifdef D_PRINT_CAMERA_INFO
    qDebug()
        << "Camera format:"
        << camera->cameraFormat().resolution();
#endif
}

void CameraCapture::stop()
{
    camera->stop();
    delete camera;
    delete sink;
}
