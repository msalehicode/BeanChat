#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>


//list of devcies
#include <QMediaDevices>
#include <QAudioDevice>
#include <QCameraDevice>
#include <QScreen>
#include <QDebug>

#include "audiocapture.h"
#include "opusencoder.h"

#include "cameracapture.h"

#include "myvideoitem.h"

#include "monitorcapture.h"

#include "participantmodel.h"

#include <QSGRendererInterface>
// #include "videoencoder.h"
// #include "ffmpegencoder.h"
#include "audiospeaker.h"

#include "channelmodel.h"

#include "user.h"
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    //list devices
    qDebug() << "=== INPUT DEVICES ===";
    auto inputs = QMediaDevices::audioInputs();
    for (int i = 0; i < inputs.size(); i++)
    {
        qDebug() << i
                 << inputs[i].description();
    }

    qDebug() << "\n=== OUTPUT DEVICES ===";
    auto outputs = QMediaDevices::audioOutputs();
    for (int i = 0; i < outputs.size(); i++)
    {
        qDebug() << i
                 << outputs[i].description();
    }

    qDebug() << "\n=== CAMERAS ===";
    auto cameras = QMediaDevices::videoInputs();
    for (int i = 0; i < cameras.size(); i++)
    {
        qDebug() << i
                 << cameras[i].description();
    }

    qDebug() << "\n=== MONITORS ===";
    const auto screens = QGuiApplication::screens();
    qDebug() << "Monitor count:" << screens.size();

    for (int i = 0; i < screens.size(); ++i)
    {
        QScreen *screen = screens[i];

        qDebug() << "Monitor" << i;
        qDebug() << "  Name:" << screen->name();
        qDebug() << "  Geometry:" << screen->geometry();
        qDebug() << "  Size:" << screen->size();
        qDebug() << "  DPI:" << screen->logicalDotsPerInch();
        qDebug() << "  Refresh Rate:" << screen->refreshRate();
    }



    //voice
    static QByteArray buffer;

    AudioCapture audio;
    audio.start();

    QObject::connect(&audio, &AudioCapture::levelChanged,
                     [](double level)
                     {
                         // qDebug() << "Voice Level:" << level;
                     });

    QObject::connect(&audio, &AudioCapture::pcmReady,
                     [](QByteArray pcm)
                     {
                         // qDebug() << "PCM frame size:" << pcm.size();
                     });


    // OpusEncoderWrapper opus;
    // QObject::connect(&audio, &AudioCapture::pcmReady,
    //                  [&](QByteArray pcm)
    //                  {
    //                      buffer.append(pcm);

    //                      const int frameSizeBytes = 960 * sizeof(short); // 1920 bytes

    //                      while (buffer.size() >= frameSizeBytes)
    //                      {
    //                          const short *samples =
    //                              reinterpret_cast<const short*>(buffer.constData());

    //                          QByteArray opusData =
    //                              opus.encode(samples, 960);

    //                          qDebug() << "Opus size:" << opusData.size();

    //                          buffer.remove(0, frameSizeBytes);
    //                      }
    //                  });



    // camera
    CameraCapture cam;
    cam.start();

    // FFmpegEncoder encoder;
    // encoder.init(640, 480);

    // connect(&cam, &CameraCapture::frameReady,
    //         &encoder, &FFmpegEncoder::encodeFrame);

    QObject::connect(&cam, &CameraCapture::frameReady,
                     [](const QVideoFrame &frame)
                     {
                         // qDebug() << "Frame received:" << frame.size();
                     });

    // connect(&cam, &CameraCapture::frameReady,
    //         &encoder, &VideoEncoder::encodeFrame);




    //speaker
    AudioSpeaker speaker;
    speaker.start();
    QObject::connect(&audio, &AudioCapture::pcmReady,
            &speaker, &AudioSpeaker::playPcm);



    //monitor
    MonitorCapture *capture = new MonitorCapture();
    QStringList monitors = capture->monitors();
    for (int i = 0; i < monitors.size(); ++i)
    {
        qDebug() << i << monitors[i];
    }
    capture->selectMonitor(0);

    // QObject::connect(capture,
    //         &MonitorCapture::frameReady,
    //         [](const QVideoFrame &frame)
    //         {
    //             QVideoFrame copy(frame);

    //             if (!copy.map(QVideoFrame::ReadOnly))
    //                 return;

    //             QImage image = copy.toImage();

    //             // Encode and send over network
    //             // e.g. JPEG, H264, WebRTC

    //             copy.unmap();
    //         });

    capture->start();





    ChannelModel channelModel;
    User usr(&channelModel);




    ParticipantModel participants;

    //test
    // ParticipantModel participants;
    const int userCount = 7;

    for (int i = 0; i < userCount; ++i)
    {
        participants.addUser(
            QString("User %1").arg(i + 1),
            (i%2==0?false:true),   // microphone
            (i%2==0?true:false));  // camera
    }

    for (int i = 0; i < userCount; ++i)
    {
        Participant *p =
            participants.findUser(
                QString("User %1").arg(i + 1));

        if (!p)
            continue;

        QObject::connect(
            &cam,
            &CameraCapture::imageReady,
            p->videoSink(),
            &VideoSink::setImage,
            Qt::QueuedConnection);
    }

    // qDebug() << "Mike =" << mike;
    // qDebug() << "Sarah =" << sarah;

    // qDebug() << "Mike sink =" << mike->videoSink();
    // qDebug() << "Sarah sink =" << sarah->videoSink();



    //register to QML
    qmlRegisterType<MyVideoItem>("CustomVideo", 1, 0, "VideoItem");
    qmlRegisterUncreatableType<VideoSink>("CustomVideo", 1, 0, "VideoSink", "VideoSink cannot be created from QML");


    //qml
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("camera", &cam);
    engine.rootContext()->setContextProperty("microphone", &audio);
    engine.rootContext()->setContextProperty("speaker", &speaker);
    engine.rootContext()->setContextProperty("participantModel", &participants);
    engine.rootContext()->setContextProperty("user", &usr);
    engine.rootContext()->setContextProperty("channelModel",&channelModel);


    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("BeanChat", "Main");



    //check renreder
    auto *window =
        qobject_cast<QQuickWindow*>(
            engine.rootObjects().first());
    qDebug() << "render" << window->rendererInterface()->graphicsApi();

    //monitor preview
    // auto monitorItem =
    //     root->findChild<MyVideoItem*>("monitorItem");
    // if (monitorItem)
    // {
    //     QObject::connect(capture,
    //                      &MonitorCapture::frameReady,
    //                      monitorItem,
    //                      &MyVideoItem::setFrame,
    //                      Qt::DirectConnection);
    // }


    return app.exec();
}
