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
#include "models/connectedusersproxymodel.h"

#include "models/attachmentimageprovider.h"

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


#include "sharing/qrcodeimageprovider.h"
#include "sharing/servercode.h"
#include "sharing/clipboardhelper.h"

#include <protocol/commonTypes.h>


//logger
#include <QStandardPaths>
#include "logging/logger.h"
#include "logging/loggingcategories.h"
#include "logging/crashreporter.h"
#include "managers/logmanager.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QString logsPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logs";

    // ---------------------- log manager and report crashes  ----------------------
    qDebug() << "log manager -> cleanup old ones"
                     "and check for any crashes";

    LogManager logManager(logsPath);
    logManager.cleanupOldLogs();

    //find crashes
    const auto crashes = logManager.crashLogs();
    for (const LogFile &log : crashes)
    {
        qDebug() << "Crash log:" << log.fileName;
    }

    //setup crash reporter
    CrashReporter reporter;
    reporter.setUploadUrl(CRASH_API_URL);
    reporter.setApiKey(CRASH_API_KEY);

    QObject::connect(&reporter,
            &CrashReporter::uploadFinished,
            [&](const QString &file,
                bool success,
                const QString &errorString)
            {
                if (success)
                    logManager.markUploaded(file);
                else
                    qDebug() << "upload crash failed. error=" << errorString;
            }
        );

    //upload them.
    qDebug() << "Reporting crashes count: " << logManager.crashLogs().count();
    if( logManager.crashLogs().count()>0)
    {
        for (const LogFile &log : logManager.crashLogs())
        {
            qDebug() << "uploading log file: " << log.filePath;
            reporter.upload(log.filePath);
        }
    }




    //------------------------- logger -------------------------
    Logger logger;

    // QLoggingCategory::setFilterRules(R"(
    // ffmpeg.debug=false
    // identity.debug=true
    // database.debug=true
    // udp.debug=false
    // settings.debug=true
    // app.debug=true
    // )"); //*.debug=false

    if (!logger.initialize(logsPath))
        qFatal() << "Failed to initialize logger.";


    QObject::connect(
        &app,
        &QCoreApplication::aboutToQuit,
        &logger,
        &Logger::shutdown);

    //set icon
    app.setWindowIcon(QIcon(":/icons/BeanChat.png"));


    //------------------------- options -------------------------
    bool testVideo = QGuiApplication::arguments().contains("-testVid");
    qCInfo(_main) << "============== LAUNCH OPTIONS ==============";
    qCInfo(_main) << "-testVid (draw fake instead of camera data) = " << testVideo;


    //reading these from cmake
    QCoreApplication::setOrganizationName(QString::fromUtf8(APP_ORGANIZATION));
    QCoreApplication::setOrganizationDomain(QString::fromUtf8(APP_ORG_DOMAIN));
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


    //show directly downloaded attachment images in component chat
    AttachmentImageProvider* attachmentImageProvider = new AttachmentImageProvider;


    //sharing
    ServerCode serverCode; //encode/decode (ipv4:port -> XXX-XXX-XXX)
    ClipboardHelper clipboardHelper;



    //---------- models ----------
    ChannelModel channelModel;
    ChatModel chatModel;
    ParticipantModel participantsModel;//participant = current channel users which has video sink and shown on center of screen
    ConnectedUsersModel connectedUsersModel;
    ConnectedUsersProxyModel connectedUsersProxy;
    MyServersModel myServersModel;



    //connect proxyModel to connectedUsers to sort users as we wish
    connectedUsersProxy.setSourceModel(&connectedUsersModel);
    connectedUsersProxy.sort(0);

    //----------
    User usr(&channelModel, &chatModel, &participantsModel, &connectedUsersModel, &myServersModel,
             &soundManager, &settingsManager, &clientuserManager, &identityManager,
             &relationshipManager, &database,
             attachmentImageProvider,
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




    //---------- sound pack connection ----------
    QObject::connect(&usr, &User::newMessage, &soundManager, &SoundManager::playNewMessage);

    QObject::connect(&usr, &User::youConnected, &soundManager, &SoundManager::playConnected);
    QObject::connect(&usr, &User::youDisconnected, &soundManager, &SoundManager::playDisconnected);
    QObject::connect(&usr, &User::youConnectionLost, &soundManager, &SoundManager::playConnectionLost);

    QObject::connect(&usr, &User::youChannelSwitched, &soundManager, &SoundManager::playChannelSwitched);
    QObject::connect(&usr, &User::youWereMoved, &soundManager, &SoundManager::playYouWereMoved);

    QObject::connect(&usr, &User::userJoined, &soundManager, &SoundManager::playUserJoin);
    QObject::connect(&usr, &User::userLeft, &soundManager, &SoundManager::playUserLeft);
    QObject::connect(&usr, &User::userTimedOut, &soundManager, &SoundManager::playUserTimedOut);
    QObject::connect(&usr, &User::userDisconnected, &soundManager, &SoundManager::playUserDisconnected);

    QObject::connect(&usr, &User::micClosed, &soundManager, &SoundManager::playMicMuted);
    QObject::connect(&usr, &User::micOpened, &soundManager, &SoundManager::playMicActiavted);

    QObject::connect(&usr, &User::speakerClosed, &soundManager, &SoundManager::playSoundMuted);
    QObject::connect(&usr, &User::speakerOpened, &soundManager, &SoundManager::playSoundResumed);

    QObject::connect(&usr, &User::cameraClosed, &soundManager, &SoundManager::playCameraOff);
    QObject::connect(&usr, &User::cameraOpened, &soundManager, &SoundManager::playCameraOn);



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
    qmlRegisterUncreatableMetaObject(Presence::staticMetaObject, "BeanChatClient", 1, 0, "Presence", "Enums only");
    qmlRegisterUncreatableMetaObject(Msg::staticMetaObject, "BeanChatClient", 1, 0, "MessageType", "Enums only");
    qmlRegisterUncreatableType<User::UserConnectionStatus> ("BeanChatClient", 1, 0, "ConnectionStatus", "Enums only");

    //qml
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("logger", &logger);
    engine.rootContext()->setContextProperty("microphone", &audio);
    engine.rootContext()->setContextProperty("speaker", &speaker);
    engine.rootContext()->setContextProperty("camera", &cam);
    engine.rootContext()->setContextProperty("participantModel", &participantsModel);
    engine.rootContext()->setContextProperty("connectedUsersModel", &connectedUsersProxy);
    engine.rootContext()->setContextProperty("myServersModel", &myServersModel);
    engine.rootContext()->setContextProperty("user", &usr);
    engine.rootContext()->setContextProperty("channelModel",&channelModel);
    engine.rootContext()->setContextProperty("chatModel",&chatModel);
    engine.rootContext()->setContextProperty("soundManager",&soundManager);
    engine.rootContext()->setContextProperty("settings",&settingsManager);
    engine.rootContext()->setContextProperty("identityManager",&identityManager);
    engine.rootContext()->setContextProperty("relationshipManager",&relationshipManager);
    engine.rootContext()->setContextProperty("serverCode", &serverCode);
    engine.rootContext()->setContextProperty("clipboardHelper", &clipboardHelper);

    engine.addImageProvider("qrcode", new QRCodeImageProvider);
    engine.addImageProvider("attachments",attachmentImageProvider);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("BeanChat", "Main");



    //check renderer
    auto *window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    qCInfo(_main) << "window graphicsApi renderer Interface : " << window->rendererInterface()->graphicsApi();


    return app.exec();
}
