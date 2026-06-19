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



    SoundManager soundManager;


    ChannelModel channelModel;
    ChatModel chatModel;
    User usr(&channelModel,&chatModel);


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

    //play received voice from udp
    QObject::connect(
        &usr,
        &User::voiceReceived,
        &speaker,
        &AudioSpeaker::playPcm);

    //make soundmanager output change when speakre output changed.
    QObject::connect(
        &speaker,
        QOverload<QAudioDevice*>::of(&AudioSpeaker::currentAudioOutputChanged),
        &soundManager,
        &SoundManager::changeAudioOutput);


    // ---------- CAMERA ----------
    CameraCapture cam;
    // cam.start();

    // QObject::connect(&cam, &CameraCapture::frameReady,
    //                  [](const QVideoFrame &frame)
    //                  {
                         // qDebug() << "Frame received:" << frame.size();
                     // });

    // ---------- SCREEN ----------
    //monitor
    // MonitorCapture *capture = new MonitorCapture();
    // QStringList monitors = capture->monitors();
    // for (int i = 0; i < monitors.size(); ++i)
    // {
    //     qDebug() << i << monitors[i];
    // }
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
    //             // e.g. JPEG, H264, WebRTC

    //             copy.unmap();
    //         });
    // capture->start();








    ParticipantModel participants;

    //add test users.. with camera
    // const int userCount = 7;

    // for (int i = 0; i < userCount; ++i)
    // {
    //     participants.addUser(
    //         QString("User %1").arg(i + 1),
    //         (i%2==0?false:true),   // microphone
    //         (i%2==0?true:false));  // camera
    // }

    // for (int i = 0; i < userCount; ++i)
    // {
    //     Participant *p =
    //         participants.findUser(
    //             QString("User %1").arg(i + 1));

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



    //check renreder
    auto *window =
        qobject_cast<QQuickWindow*>(
            engine.rootObjects().first());
    qDebug() << "render" << window->rendererInterface()->graphicsApi();


    return app.exec();
}
