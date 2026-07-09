#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>


//
#include <QScreen>
#include <QDebug>
#include <QSGRendererInterface>
#include <QBuffer>
#include <QTimer>
#include <QIcon>

//resources
#include "audio/audiospeaker.h"
#include "audio/audiocapture.h"

#include "video/cameracapture.h"
#include "video/monitorcapture.h"


//models
#include "models/channelmodel.h"
#include "models/chatmodel.h"
#include "models/connectedusersmodel.h"
#include "models/myserversmodel.h"
#include "models/participantmodel.h"
#include "models/userrelationship.h"

//QML components
#include "video/myvideoitem.h"


//
#include "user.h"
#include "managers/soundmanager.h"
#include "managers/settingsmanager.h"
#include "managers/clientusermanager.h"
#include "managers/identitymanager.h"
#include "managers/relationshipmanager.h"
#include "audio/audiomixer.h"



//test video/camera (draw things)
#include "video/ffmpegencoder.h"
#include <QPainter>
#include <QDateTime>
#include <QTimer>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/icons/BeanChat.png"));


    //options
    bool testVideo = QGuiApplication::arguments().contains("-testVid");
    qDebug() << "\n\n"
                "================================================================";
    qDebug() << "======================== LAUNCH OPTIONS ========================";
    qDebug() << "-testVid (draw fake instead of camera data) = " << testVideo;
    qDebug() << "================================================================"
                "\n\n";



    //reading these from cmake
    QCoreApplication::setOrganizationName(QString::fromUtf8(APP_ORGANIZATION));
    QCoreApplication::setApplicationName(QString::fromUtf8(APP_NAME));
    QCoreApplication::setApplicationVersion(QString::fromUtf8(APP_VERSION));


    //---------- resources ----------
    AudioCapture audio;
    CameraCapture cam;
    AudioSpeaker speaker;

    //etc
    Database database;
    SoundManager soundManager; //play effects
    SettingsManager settingsManager;
    ClientUserManager clientuserManager;
    IdentityManager identityManager;
    RelationshipManager relationshipManager(&database);
    AudioMixer mixer;




    //---------- models ----------
    ChannelModel channelModel;
    ChatModel chatModel;
    ParticipantModel participantsModel;//participant = current channel users which has video sink and shown on center of screen
    ConnectedUsersModel connectedUsersModel;
    MyServersModel myServersModel;

    //----------
    User usr(&channelModel, &chatModel, &participantsModel, &connectedUsersModel, &myServersModel,
             &soundManager, &settingsManager, &clientuserManager, &identityManager,
             &relationshipManager, &database,
             &cam, &audio, &speaker);

    //---------- camera connection ----------

    //just make fake camera feed for test
    FFmpegEncoder fakeEncoder;
    QTimer timer;
    if(testVideo)
    {
        fakeEncoder.open(
            CAMERA_DEFAuLT_WIDTH,
            CAMERA_DEFAULT_HEIGHT,
            CAMERA_DEFAULT_FPS,
            CAMERA_DEFAULT_BITRATE,
            CAMERA_DEFAULT_KEYFRAME);

        QObject::connect(
            &fakeEncoder,
            &FFmpegEncoder::packetReady,
            &usr,
            &User::sendVideoFrame);


        QObject::connect(
            &timer,
            &QTimer::timeout,
            [&]()
            {
                static int frame = 0;
                QImage img(CAMERA_DEFAuLT_WIDTH, CAMERA_DEFAULT_HEIGHT, QImage::Format_RGB888);
                img.fill(Qt::black);
                QPainter p(&img);
                p.setPen(Qt::white);
                p.setFont(QFont("Arial", 28));
                p.drawText(20, 40, QString("Frame %1").arg(frame++));
                p.drawText( 20, 80, QTime::currentTime().toString());
                // Moving rectangle
                p.fillRect((frame * 8) % 500, 150, 100, 100, Qt::red);
                p.end();
                fakeEncoder.encode(img);
            });
        timer.start(1000 / CAMERA_DEFAULT_FPS); //FPS
    }
    else
    {
        QObject::connect(
            &cam,
            &CameraCapture::videoPacketReady,
            &usr,
            &User::sendVideoFrame);
    }





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

    QObject::connect(
        &usr,
        &User::voiceReceived,
        &mixer,
        &AudioMixer::addVoice);

    QObject::connect(
        &mixer,
        &AudioMixer::mixedFrameReady,
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
    qmlRegisterUncreatableType<ClientUser>("BeanChatClient", 1, 0, "ClientUser", "Created by C++ only");
    qmlRegisterUncreatableType<Relationship>("BeanChatClient", 1, 0, "Relationship", "Enum Only");

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
    engine.rootContext()->setContextProperty("settings",&settingsManager);
    engine.rootContext()->setContextProperty("identityManager",&identityManager);
    engine.rootContext()->setContextProperty("relationshipManager",&relationshipManager);

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
