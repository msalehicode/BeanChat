#include "cameracapture.h"

// #define D_PRINT_CAMERA_INFO

CameraCapture::CameraCapture(QObject *parent)
    : QObject(parent)
{
    refreshCameraInputs();




    //connect for later changes
    QObject::connect(&m_cameraDevices, &QMediaDevices::videoInputsChanged, this, [this]()
            {
                qDebug() << "Camera hardware change detected!";

                // 1. Refresh your local list
                refreshCameraInputs();

                // 2. Decide what to do if the active mic was removed
                if (m_currentCameraInput >= m_cameraInputs.size())
                {
                    // If the current mic is gone, reset to default 0
                    setCurrentCameraInput(0);
                }
            });


    connect(
        &m_encoder,
        &FFmpegEncoder::packetReady,
        this,
        [this](const QByteArray &packet, bool keyFrame)
        {
            qDebug()
            << "Encoded packet:"
            << packet.size()
            << "Key:"
            << keyFrame;

            emit videoPacketReady(packet);

        });
}

void CameraCapture::start()
{
    // Check if the current index is actually valid
    if (m_cameraInputs.isEmpty() || m_currentCameraInput>= m_cameraInputs.size()) {
        qWarning() << "Invalid camera input index!";
        // device = QMediaDevices::defaultVideoInput();
        return;
    }

    // Clean up existing capture before starting new one
    if (camera)
    {
        stop();
    }



    camera = new QCamera(m_cameraInputs[m_currentCameraInput], this);


#ifdef D_PRINT_CAMERA_INFO
    //test performace
    auto formats = m_cameraInputs[m_currentCameraInput].videoFormats();
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

                //fps limit
                // static int counter = 0;
                // counter++;
                // Send only one frame out of every 30
                // if (counter % 30 != 0)
                //     return;

                qDebug() << "Cameraa frame:" << img.size();

                m_encoder.encode(img);

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


    m_encoder.open(
        640, //width
        480, //height
        15, //fps
        50000, //bitrate -> 100000 / 8 = 12500 bytes/sec ≈ 12.5 KB/sec
        15);  // keyframe every 15 frames (1 second)



    camera->start();

#ifdef D_PRINT_CAMERA_INFO
    qDebug()
        << "Camera format:"
        << camera->cameraFormat().resolution();
#endif
}


void CameraCapture::refreshCameraInputs()
{
    qDebug() << "=== CAMERA DEVICES ===";
    QList<QCameraDevice> inputs = QMediaDevices::videoInputs();
    for (int i = 0; i < inputs.size(); i++)
    {
        qDebug() << i
                 << inputs[i].description();
    }

    setCameraInputs(inputs);
}

void CameraCapture::setCameraInputs(QList<QCameraDevice> newList)
{
    if(m_cameraInputs == newList)
        return;

    m_cameraInputs=newList;

    emit cameraInputsChanged();
}


void CameraCapture::stop()
{
    if (camera)
    {
        camera->stop();
        delete camera;
        camera = nullptr;
    }

    if (sink)
    {
        delete sink;
        sink = nullptr;
    }
}

QStringList CameraCapture::cameraInputsNames() const
{
    QStringList names;

    for (const auto &device : m_cameraInputs)
    {
        names << device.description();
    }

    return names;
}

int CameraCapture::currentCameraInput() const
{
    return m_currentCameraInput;
}

void CameraCapture::setCurrentCameraInput(int newCurrentCameraInput)
{
    if (m_currentCameraInput == newCurrentCameraInput)
        return;
    m_currentCameraInput = newCurrentCameraInput;
    emit currentCameraInputChanged();
}

int CameraCapture::devicesCount() const
{
    return m_cameraInputs.count();
}

QString CameraCapture::cameraIntputId(int index) const
{
    if (index < 0 || index >= m_cameraInputs.size())
        return {};

    return QString::fromUtf8(m_cameraInputs[index].id());
}

int CameraCapture::cameraInputIndexFromId(const QString &id) const
{
    for (int i = 0; i < m_cameraInputs.size(); ++i)
    {
        if (QString::fromUtf8(m_cameraInputs[i].id()) == id)
            return i;
    }

    return -1;
}
