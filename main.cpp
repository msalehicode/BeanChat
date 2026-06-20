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

#include "cameracapture.h"

#include "myvideoitem.h"

#include "monitorcapture.h"

#include "participantmodel.h"

#include <QSGRendererInterface>
// #include "opusencoder.h"
// #include "videoencoder.h"
// #include "ffmpegencoder.h"
#include "audiospeaker.h"

#include "channelmodel.h"
#include "chatmodel.h"

#include "user.h"

#include "soundmanager.h"

#include <QBuffer>
#include <QTimer>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    //list devices


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

    CameraCapture cam;

    SoundManager soundManager;


    ChannelModel channelModel;
    ChatModel chatModel;
    ParticipantModel participants;//hold center rectangles for current channel users.
    User usr(&channelModel,&chatModel,&participants, &cam);

    //CAMERA aconnection:
    QObject::connect(
        &cam,
        &CameraCapture::jpegReady,
        &usr,
        &User::sendVideoFrame);

    //just make fake camera feed for test due to camera can use once
    // QTimer timer;
    // QObject::connect(
    //     &timer,
    //     &QTimer::timeout,
    //     [&usr]()
    //     {
    //         qDebug() << "generating fake camera ..";

    //         QImage img(640, 400, QImage::Format_RGB32);
    //         img.fill(Qt::red);

    //         QByteArray jpgData;

    //         QBuffer buffer(&jpgData);
    //         buffer.open(QIODevice::WriteOnly);

    //         img.save(&buffer, "JPG", 60);

    //         usr.sendVideoFrame(jpgData);
    //     });
    // timer.start(1000);




    //sound effects connections
    QObject::connect(
        &usr,
        &User::userJoined,
        &soundManager,
        &SoundManager::playUserJoin);

    QObject::connect(
        &usr,
        &User::userLeft,
        &soundManager,
        &SoundManager::playUserLeave);

    QObject::connect(
        &usr,
        &User::newMessage,
        &soundManager,
        &SoundManager::playMessage);





    // ---------- MICROHPONE ----------
    AudioCapture audio;
    audio.start();

    //print volume level.
    // QObject::connect(&audio, &AudioCapture::levelChanged, [](double level) { qDebug() << "Voice Level:" << level; });

    //send mic voice over udp
    QObject::connect(
        &audio,
        &AudioCapture::pcmReady,
        &usr,
        &User::sendVoicePcm);



    // ---------- SPEAKER ----------
    AudioSpeaker speaker;
    speaker.start();
    //playback microhpone..
    // QObject::connect(&audio, &AudioCapture::pcmReady,
    //         &speaker, &AudioSpeaker::playPcm);

    //play received voice from udp socket
    QObject::connect(
        &usr,
        &User::voiceReceived,
        &speaker,
        &AudioSpeaker::playPcm);

    //make soundmanager output change when speaker output changed.
    QObject::connect(
        &speaker,
        QOverload<QAudioDevice*>::of(&AudioSpeaker::currentAudioOutputChanged),
        &soundManager,
        &SoundManager::changeAudioOutput);




    // ---------- SCREEN ----------
    // MonitorCapture *capture = new MonitorCapture();
    // QStringList monitors = capture->monitors();
    // for (int i = 0; i < monitors.size(); ++i)
    //     qDebug() << i << monitors[i];

    // capture->selectMonitor(0);
    // QObject::connect(capture,
    //         &MonitorCapture::frameReady,
    //         [](const QVideoFrame &frame)
    //         {
    //             QVideoFrame copy(frame);
    //             if (!copy.map(QVideoFrame::ReadOnly))
    //                 return;

    //             QImage image = copy.toImage();
    //             // Encode and send over network
    //             copy.unmap();
    //         });
    // capture->start();









    //add test users.. with camera
    // const int userCount = 2;
    // for (int i = 0; i < userCount; ++i)
    // {
    //     participants.addUser(
    //         QString("User %1").arg(i + 1),
    //         (i%2==0?false:true),   // microphone
    //         (i%2==0?true:false));  // camera
    // }
    // for (int i = 0; i < userCount; ++i)
    // {
    //     Participant *p = participants.findUser(QString("User %1").arg(i + 1));

    //     if (!p)
    //         continue;

    //     QObject::connect(
    //         &cam,
    //         &CameraCapture::imageReady,
    //         p->videoSink(),
    //         &VideoSink::setImage,
    //         Qt::QueuedConnection);
    // }


    //register to QML
    qmlRegisterType<MyVideoItem>("CustomVideo", 1, 0, "VideoItem");
    qmlRegisterUncreatableType<VideoSink>("CustomVideo", 1, 0, "VideoSink", "VideoSink cannot be created from QML");


    //qml
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("microphone", &audio);
    engine.rootContext()->setContextProperty("speaker", &speaker);
    engine.rootContext()->setContextProperty("participantModel", &participants);
    engine.rootContext()->setContextProperty("user", &usr);
    engine.rootContext()->setContextProperty("channelModel",&channelModel);
    engine.rootContext()->setContextProperty("chatModel",&chatModel);
    engine.rootContext()->setContextProperty("soundManager",&soundManager);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("BeanChat", "Main");



    //check renderer
    auto *window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    qDebug() << "window graphicsApi renderer Interface : " << window->rendererInterface()->graphicsApi();


    return app.exec();
}
