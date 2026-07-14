#include "cameracapture.h"

#include "logging/loggingcategories.h"

CameraCapture::CameraCapture(QObject *parent)
    : QObject(parent)
{
    refreshCameraInputs();




    //connect for later changes
    QObject::connect(&m_cameraDevices, &QMediaDevices::videoInputsChanged, this, [this]()
            {

                qCInfo(_camera) << "Camera hardware change detected!";

                // 1. Refresh your local list
                refreshCameraInputs();

                // 2. Decide what to do if the active mic was removed
                if (m_currentCameraInput >= m_cameraInputs.size())
                {
                    // If the current mic is gone, reset to default 0
                    qCInfo(_camera) << "current mic is gone, reset to default (0)";
                    setCurrentCameraInput(0);
                }
            });


    connect(
        &m_encoder,
        &FFmpegEncoder::packetReady,
        this,
        [this](const QByteArray &packet, bool keyFrame)
        {

#if D_PRINT_CAMERA_INFO
            qDebug()
            << "Encoded packet:"
            << packet.size()
            << "Key:"
            << keyFrame;
#endif
            emit videoPacketReady(packet);

        });
}


CameraCapture::~CameraCapture()
{
    stop();
}

void CameraCapture::start()
{
    qCInfo(_camera) << "start camera.";
    // Check if the current index is actually valid
    if (m_cameraInputs.isEmpty() || m_currentCameraInput>= m_cameraInputs.size()) {
        qCCritical(_camera) << "Invalid camera input index!";
        // device = QMediaDevices::defaultVideoInput();
        return;
    }

    // Clean up existing capture before starting new one
    if (camera)
    {
        qCInfo(_camera) << "camera exists, lets clean up old one";
        stop();
    }



    camera = new QCamera(m_cameraInputs[m_currentCameraInput], this);


    //check performace
    auto formats = m_cameraInputs[m_currentCameraInput].videoFormats();
    for(const auto &fmt : formats)
    {
        qCInfo(_camera) << "current camera input performance/formats are: "
        << " Resolution:" << fmt.resolution()
        << " FPS:" << fmt.maxFrameRate();
    }
    qCInfo(_camera) << "Video input:" << QMediaDevices::defaultVideoInput().description();


    sink = new QVideoSink(this);

    connect(
        sink,
        &QVideoSink::videoFrameChanged,
        this,
        [this](const QVideoFrame &frame)
        {
            if (!frame.isValid())
                return;


            #if D_PRINT_CAMERA_INFO
                        //test performace
                        static QElapsedTimer timer;
                        static int count = 0;

                        if (!timer.isValid())
                            timer.start();
                        count++;
                        if (timer.elapsed() >= 1000)
                        {
                            qCDebug(_camera) << "Raw camera FPS:" << count;

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

                // qCDebug(_camera) << "Cameraa frame:" << img.size();

                m_encoder.encode(img);

                #if D_PRINT_CAMERA_INFO
                                qCDebug(_camera)
                                    << "Resolution JPEG:"
                                    << img.width()
                                    << "x"
                                    << img.height();
                                    // << "JPEG size:" << jpgData.size()/1024.0<< "KB";
                #endif

                m_frame = img;

                emit frameChanged();
                emit imageReady(img);

            #if D_PRINT_CAMERA_INFO
                            static int count = 0;
                            count++;

                            static QElapsedTimer timer;
                            if(!timer.isValid())
                                timer.start();

                            if(timer.elapsed() > 1000)
                            {
                                qCDebug(_camera)<< "camera FPS:" << count;
                                count = 0;
                                timer.restart();
                            }
            #endif
            }

            emit frameReady(frame);
        });

    session.setCamera(camera);
    session.setVideoSink(sink);


    if (!m_encoder.open(
            CAMERA_DEFAuLT_WIDTH,
            CAMERA_DEFAULT_HEIGHT,
            CAMERA_DEFAULT_FPS,
            CAMERA_DEFAULT_BITRATE,
            CAMERA_DEFAULT_KEYFRAME))
    {
        qCCritical(_camera) << "Failed to open FFmpeg encoder.";
        stop();
        return;
    }

    camera->start();

#if D_PRINT_CAMERA_INFO
    qCDebug(_camera)
        << "Camera format:"
        << camera->cameraFormat().resolution();
#endif
}


void CameraCapture::refreshCameraInputs()
{
    qCInfo(_camera) << "=== CAMERA DEVICES ===";
    QList<QCameraDevice> inputs = QMediaDevices::videoInputs();
    for (int i = 0; i < inputs.size(); i++)
    {
        qCInfo(_camera) << i << inputs[i].description();
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
        qCInfo(_camera) << "stopping camera";
        camera->stop();
        delete camera;
        camera = nullptr;
    }

    m_encoder.close();

    if (sink)
    {
        qCInfo(_camera) << "cleaning camera's sink";
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
    qCInfo(_camera) << "set current camera input index to " << newCurrentCameraInput;
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
