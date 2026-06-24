#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>


//list of devcies
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
#include "connectedusersmodel.h"
#include "myserversmodel.h"

#include "user.h"

#include "soundmanager.h"

#include <QBuffer>
#include <QTimer>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);



    //reading these from cmake
    QCoreApplication::setOrganizationName(QString::fromUtf8(APP_ORGANIZATION));
    QCoreApplication::setApplicationName(QString::fromUtf8(APP_NAME));
    QCoreApplication::setApplicationVersion(QString::fromUtf8(APP_VERSION));


    //---------- resources ----------
    AudioCapture audio;
    CameraCapture cam;
    SoundManager soundManager; //play effects
    AudioSpeaker speaker;



    //---------- models ----------
    ChannelModel channelModel;
    ChatModel chatModel;
    ParticipantModel participantsModel;//participant = current channel users which has video sink and shown on center of screen
    ConnectedUsersModel connectedUsersModel;
    MyServersModel myServersModel;

    //----------
    User usr(&channelModel, &chatModel, &participantsModel, &connectedUsersModel, &myServersModel,
             &cam, &audio, &speaker);

    //---------- camera connection ----------
    QObject::connect(
        &cam,
        &CameraCapture::jpegReady,
        &usr,
        &User::sendVideoFrame);

    //just make fake camera feed for test due to camera can use only once
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




    //---------- microphone connection ----------
    QObject::connect(
        &audio,
        &AudioCapture::pcmReady,
        &usr,
        &User::sendVoicePcm);




    //---------- sound effects connection //----------
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
        &User::messageSent,
        &soundManager,
        &SoundManager::playMessage);

    QObject::connect(
        &usr,
        &User::newMessage,
        &soundManager,
        &SoundManager::playMessageBack);





    // ---------- speaker connection ----------

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
    // qDebug() << "\n=== MONITORS ===";
    // const auto screens = QGuiApplication::screens();
    // qDebug() << "Monitor count:" << screens.size();

    // for (int i = 0; i < screens.size(); ++i)
    // {
    //     QScreen *screen = screens[i];

    //     qDebug() << "Monitor" << i;
    //     qDebug() << "  Name:" << screen->name();
    //     qDebug() << "  Geometry:" << screen->geometry();
    //     qDebug() << "  Size:" << screen->size();
    //     qDebug() << "  DPI:" << screen->logicalDotsPerInch();
    //     qDebug() << "  Refresh Rate:" << screen->refreshRate();
    // }
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



    //register to QML
    qmlRegisterType<MyVideoItem>("CustomVideo", 1, 0, "VideoItem");
    qmlRegisterUncreatableType<VideoSink>("CustomVideo", 1, 0, "VideoSink", "VideoSink cannot be created from QML");


    //qml
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("microphone", &audio);
    engine.rootContext()->setContextProperty("speaker", &speaker);
    engine.rootContext()->setContextProperty("camera", &cam);
    engine.rootContext()->setContextProperty("participantModel", &participantsModel);
    engine.rootContext()->setContextProperty("connectedUsersModel", &connectedUsersModel);
    engine.rootContext()->setContextProperty("myServersModel", &myServersModel);
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
