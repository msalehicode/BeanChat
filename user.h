#ifndef USER_H
#define USER_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QCoreApplication>
#include <QDataStream>
#include <QRandomGenerator> //to get a random number for default username
#include <QSysInfo>
#include <QTimer>
#include <QHostInfo>
#include <QQueue>
#include <QHostAddress>


// include from BeanChatCommon (to server and client speak one language)
#include <protocol/Packets.h>
#include <protocol/Packet.h>
#include <protocol/PacketHelpers.h>
#include <protocol/ProtocolVersion.h>
#include <protocol/ProtocolLimits.h>
#include <protocol/commonTypes.h>
using namespace BeanChatCommon;



// C++ models to expose data to QML lists
#include "models/channelmodel.h"
#include "models/chatmodel.h"
#include "models/participantmodel.h"
#include "models/myserversmodel.h"
#include "models/connectedusersmodel.h"


//capture resources
#include "video/cameracapture.h"
#include "audio/audiocapture.h"
#include "audio/audiospeaker.h"


#include "managers/settingsmanager.h"
#include "managers/soundmanager.h"
#include "database/database.h"
#include "audio/opuscodec.h"
#include "video/ffmpegdecoder.h"


#include "managers/avatarmanager.h"
#include "managers/clientusermanager.h"
#include "managers/identitymanager.h"
#include "managers/relationshipmanager.h"

#include "models/identity.h"
#include <QStandardPaths>
#define SAVE_AVATAR_PATH QStandardPaths::writableLocation(QStandardPaths::CacheLocation)+"/servers/"


#include <QProcess>
#include "update/updatechecker.h"


// enum class UserConnectionStatus
// {
//     Unknown,
//     Connecting,
//     Connected,
//     ConnectionLost,
//     Rejected,
//     Error,
//     Disconnected
// };

enum class ImportantNotificationColor
{
    Unknown=0,
    Red,
    Green,
    Blue,
    Yellow
};

//this should matches with ui/constants/NotificationTypes.qml
enum class NotificationType
{
    Info=0,
    Success,
    Warning,
    Error
};

//make those messages we dont show after each other. \
    also to remember which id is for which message

//e.g user connection lost then would disconnect trigger so we use \
    same id for them to block second to avoid print connectionlost, disconnected.
enum class NotificationId : int
{
    None=-1,

    //we dont want show these at the same time, they may push at same time.
    ConnectionLost=0,
    ConnectionRejected=0,
    ConnectionError=0,
    Disconnected=0,

    Message=1,

    YouAreMoved=2
};

enum class NotificationDuration : int
{
    Quick = 500,
    Short = 1000,
    Normal = 2500,
    Long = 3500,
    VeryLong = 5000,
    VeryVeryLong = 10000
};

class User : public QObject
{
    Q_OBJECT

public:
    explicit User(ChannelModel *channelModel,ChatModel* chatModel,
                  ParticipantModel* currentChannelParticipant,ConnectedUsersModel* connectedUsersModel, MyServersModel* myServersModel,
                  SoundManager* sounderManager, SettingsManager* settingsManager,
                  ClientUserManager* clientuserManager, IdentityManager* identityManager,
                  RelationshipManager* relationshipManager, Database* database,
                  CameraCapture* cam, AudioCapture* mic, AudioSpeaker* speaker,
                  QObject *parent = nullptr);



    //server actions
    Q_INVOKABLE void joinChannel(quint64 channelId, const QString& password="");
    Q_INVOKABLE void moveUser(quint64 userId, quint64 channelId, const QString& password);
    Q_INVOKABLE void createChannel(QString channelName, QString password, bool saveMessages);
    Q_INVOKABLE void sendMessage(QString message);
    Q_INVOKABLE void updateChannel(quint64 channelId, const QString& name,
                                    const QString& pass, bool saveMessages);
    Q_INVOKABLE QString getChannelName(quint64 channelId); //to show channlename inside user's profile
    Q_INVOKABLE void deleteChannel(quint64 channelId);

    Q_INVOKABLE ClientUser *clientUser(quint64 id);


    Q_INVOKABLE void updateApp();
    //connect,disconnect
    Q_INVOKABLE void connectToServer(bool saveThisConnection, const QString& serverIp, const QString& str_serverPort);
    Q_INVOKABLE void switchOrConnectToServer(const QString& serverIp, const QString& str_serverPort, int serverId);
    Q_INVOKABLE void disconnect();

    //myServers actions
    Q_INVOKABLE void updateSavedServer(quint64 serverId, quint64 dbIndex, const QString &name, const QString &ip, const QString &port);
    Q_INVOKABLE void deleteSavedServer(quint64 serverId, quint64 serverDbIndex=-1);

    void askForServerState();
    void askForNotFoundAvatars();

    void newAvatarArrived(quint64 userId,
                          const QString& avatarHash,
                          const QString& oldAvatarHash,
                          const QByteArray& avatarData);


    int myId() const;
    void setMyId(int newMyId);

    QString myChannelName() const;
    void setMyChannelName(const QString& name);

    QString myUsername() const;
    void setMyUsername(const QString &newMyUsername);

    void sendVoicePcm(const QByteArray& pcm);
    bool muteHeadphone() const;
    void setMuteHeadphone(bool status);

    bool muteMicrophone() const;
    void setMuteMicrophone(bool status);

    bool isCameraOpen() const;
    void setIsCameraOpen(bool status);

    QString myServerName() const;
    void setMyServerName(const QString &newMyServerName);

    bool isConnectedToServer() const;
    void setIsConnectedToServer(bool newConnectedToServer);

    QString myIdentity();
    void setMyIdentity(const QString &newIdentity);

    bool isChatOpen() const;
    void setIsChatOpen(bool newIsChatOpen);

    int chatUnreadMessages() const;
    void setChatUnreadMessages(int newChatUnreadMessages);


    int myPing() const;
    void setMyPing(int newMyPing);

    float myVoicePacketLoss() const;
    void setMyVoicePacketLoss(float newMyVoicePacketLoss);

    float myVideoPacketLoss() const;
    void setMyVideoPacketLoss(float newMyVideoPacketLoss);


    void initOrLoadSettings();
    QString myAppVersion() const;
    void setMyAppVersion(const QString &newAppVersion);

    QString buildType() const;
    // UserConnectionStatus connectionStatus() const;
    // void setConnectionStatus(UserConnectionStatus newConnectionStatus);

    bool myChannelSavesChat() const;
    void setMyChannelSavesChat(bool newMyChannelSavesChat);


    QString appTitle() const;


    Q_INVOKABLE void updateMyProfile(const QString &username, const QString &avatarPath="");
    QString checkAvatar(quint64 userId, const QString& avatarHash, bool askForAvatar=true); //if file avatar (hash.extention) found in server's directory returns QML like path otherwise would reutrn null



    QString myAvatarPath() const;
    void setMyAvatarPath(const QString &newMyAvatarPath);

    int connectedServerId() const;
    void setConnectedServerId(int newConnectedServerId);


    //to check is channel locked or not \
        usage: when wanna move a user into a locked channel would check then if channel is locked then show enter password popup
    Q_INVOKABLE int isChannelLocked(quint64 channelId);



    //server info for QML
    QString serverName() const;
    QString serverWebsite() const;
    QString serverAvatarHash() const;
    QString serverMaxUsers() const;
    QString serverVersion() const;
    QString serverUptime() const;


    BeanChatCommon::Presence::Status myStatus() const;
    void setMyStatus(const BeanChatCommon::Presence::Status &newMyStatus);

    quint64 myChannelId() const;
    void setMyChannelId(quint64 newMyChannelId);


    Q_INVOKABLE void updateMyActivityStatus(BeanChatCommon::Presence::Status status);
signals:

    void myIdChanged();

    void myUsernameChanged();

    void voiceReceived(quint64 userId,
                       const QByteArray& pcm,
                       int volume);
    void videoReceived(QByteArray data);

    void muteHeadphoneChanged();

    void muteMicrophoneChanged();

    void myChannelNameChanged();


    //play request to sound
    void newMessage();

    void youConnected();
    void youDisconnected();
    void youConnectionLost();

    void youChannelSwitched();
    void youWereMoved();

    void userJoined();
    void userLeft();
    void userTimedOut();



    //
    void isCameraOpenChanged();

    void myServerNameChanged();

    void isConnectedToServerChanged();

    void myIdentityChanged();

    void isChatOpenChanged();

    void chatUnreadMessagesChanged();

    void myPingChanged();

    void myVoicePacketLossChanged();

    void myVideoPacketLossChanged();

    // void connectionStatusChanged();

    void myChannelSavesChatChanged();

    void myAvatarPathChanged();

    void connectedServerIdChanged();

    void myAppVersionChanged();

    /* notificationRequested's ID NOTE:
     *
       id=NotificicationId::None means not defined and show it anyway,

       e.g: id=NotificicationId::Message, would ignore duplicate notifications with the same id (1)
                if there is a notification with that id and not shown yet.
    */
    void notificationRequested(
        NotificationType type,
        QString text,
        NotificationId id=NotificationId::None,
        NotificationDuration duration=NotificationDuration::Normal);

    void showImportantNotifierBar(const QString& text,
                                                                    ImportantNotificationColor color);


    void receivedServerInfoChanged();

    void myStatusChanged();

    void myChannelIdChanged();

public slots:
    void onTcpReadyRead();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);

    void onUdpReadyRead();
    void sendVideoFrame(const QByteArray& videoData);

    void currentIdentityChangedTo(const QString& name);
private:
    QString platformName();
    void resetVariables(); //when wanna disconnect and get ready for next connection
    void processPacket(const Packet& packet);
    void loginToUdpSocket();

    Database* m_database;
    SoundManager* m_soundManager;
    SettingsManager* m_settingsManager;
    ClientUserManager* m_clientUserManager;
    OpusCodec m_opus;
    IdentityManager* m_identityManager;
    RelationshipManager* m_relationshipManager;

    //avatar
    AvatarManager m_avatarManager;
    QList<quint64> m_notFoundAvatars; //a list to when user connected to server check for user's cached avatar pics if not found then add that id here to ask server for their avatars.


    //user can modify
    QString m_myUsername;
    QString m_myAvatarPath = ""; //used to set when server connects to server, server responses a avatarHash and we know it ours so set it here to load in different parts of app, such as modifyProfile, userStuff's avatar

    //user cant modify, would receive from target server
    int m_myId =-1;
    BeanChatCommon::Presence::Status m_myStatus=BeanChatCommon::Presence::Status::Offline;
    quint64 m_myChannelId=0; //channelId 0 is default value for those users didn't connect to any channel just connected to server.
    QString m_myChannelName = ""; //current channel
    bool m_myChannelSavesChat=false;
    QString m_myServerName= ""; //current server connected to (name that saved by user inside myServers, can be modified, only shown to this user)
    // UserConnectionStatus m_connectionStatus=UserConnectionStatus::Unknown;
    int m_connectedServerId_onDb=-1; //(serverDbIndex) to use for path of avatars. e.g path/to/Cached/avatars/0  <- this 0 is server id (directory to hold that server user's avatar files)
    QString m_appVersion;

    //store connected server info received from Server, then feed to QML
    ServerInfo m_receivedServerInfo;
    Q_PROPERTY(QString serverName READ serverName NOTIFY receivedServerInfoChanged)
    Q_PROPERTY(QString serverWebsite READ serverWebsite NOTIFY receivedServerInfoChanged)
    Q_PROPERTY(QString serverAvatarHash READ serverAvatarHash NOTIFY receivedServerInfoChanged)
    Q_PROPERTY(QString serverMaxUsers  READ serverMaxUsers NOTIFY receivedServerInfoChanged)
    Q_PROPERTY(QString serverVersion READ serverVersion NOTIFY receivedServerInfoChanged)
    Q_PROPERTY(QString serverUptime READ serverUptime NOTIFY receivedServerInfoChanged)


    //connect and switch servers.
    bool m_isConnectedToServer=false;
    bool m_switchingServer = false;

    //TCP connection
    QTcpSocket socket;
    int m_myPing=-1;
    QByteArray m_tcpBuffer;

    //UDP connection
    QUdpSocket m_udpSocket;
    float m_myVoicePacketLoss=0.0f;
    float m_myVideoPacketLoss=0.0f;
    QTimer m_udpConnectionTimeout;
    QHostAddress m_serverLookedupAddress; //when user enter domain.com, TCP would lookup automatically but UDP doesnt lookup, so at begin of connection we resolve/lookup that domain's ip store here and use it for udp send packets.

    //chat notification
    bool m_isChatOpen=false;//a flag to know is chatTab is active or not
    int m_chatUnreadMessages=0; //hold count of new unread messags when user is not in chatTab


    //to hold ip and ports for different parts of app such as sendVoice, sendVideo
    QString m_serverIp = USER_DEFAULT_SERVER_IP;
    quint64 m_serverPort= USER_DEFAULT_SERVER_PORT; //udp port is this value+1

    //voice
    quint32 m_sequence = 0;
    AudioCapture* m_mic=nullptr;
    bool m_muteMicrophone=false;
    QByteArray m_sendPcmBuffer;

    //video
    quint32 m_videoSequence = 0;
    CameraCapture* m_cam=nullptr;
    bool m_isCameraOpen=false;
    FFmpegDecoder m_videoDecoder;
    QHash<QString, BeanChatCommon::PendingFrame> m_pendingFrames;
    QQueue<quint64> m_decodeQueue;

    //speaker
    AudioSpeaker* m_speaker=nullptr;
    bool m_muteHeadphone=false;

    //pointers to access and contorl esources/models    
    ChannelModel* m_channelModel=nullptr;
    ChatModel* m_chatModel=nullptr;
    ParticipantModel* m_currentChannelParticipant=nullptr;
    MyServersModel* m_myServersModel=nullptr;
    ConnectedUsersModel* m_connectedUsersModel=nullptr;



    //update
    UpdateChecker m_updateChecker;


    Q_PROPERTY(int myId READ myId WRITE setMyId NOTIFY myIdChanged FINAL)
    Q_PROPERTY(QString myUsername READ myUsername WRITE setMyUsername NOTIFY myUsernameChanged FINAL)
    Q_PROPERTY(QString myChannelName READ myChannelName NOTIFY myChannelNameChanged FINAL)
    Q_PROPERTY(bool muteHeadphone READ muteHeadphone WRITE setMuteHeadphone NOTIFY muteHeadphoneChanged FINAL)
    Q_PROPERTY(bool muteMicrophone READ muteMicrophone WRITE setMuteMicrophone NOTIFY muteMicrophoneChanged FINAL)
    Q_PROPERTY(bool isCameraOpen READ isCameraOpen WRITE setIsCameraOpen NOTIFY isCameraOpenChanged FINAL)
    Q_PROPERTY(QString myServerName READ myServerName WRITE setMyServerName NOTIFY myServerNameChanged FINAL)
    Q_PROPERTY(bool isConnectedToServer READ isConnectedToServer WRITE setIsConnectedToServer NOTIFY isConnectedToServerChanged FINAL)
    Q_PROPERTY(QString myIdentity READ myIdentity WRITE setMyIdentity NOTIFY myIdentityChanged FINAL)
    Q_PROPERTY(bool isChatOpen READ isChatOpen WRITE setIsChatOpen NOTIFY isChatOpenChanged FINAL)
    Q_PROPERTY(int chatUnreadMessages READ chatUnreadMessages WRITE setChatUnreadMessages NOTIFY chatUnreadMessagesChanged FINAL)
    Q_PROPERTY(int myPing READ myPing WRITE setMyPing NOTIFY myPingChanged FINAL)
    Q_PROPERTY(float myVoicePacketLoss READ myVoicePacketLoss WRITE setMyVoicePacketLoss NOTIFY myVoicePacketLossChanged FINAL)
    Q_PROPERTY(float myVideoPacketLoss READ myVideoPacketLoss WRITE setMyVideoPacketLoss NOTIFY myVideoPacketLossChanged FINAL)
    // Q_PROPERTY(UserConnectionStatus connectionStatus READ connectionStatus WRITE setConnectionStatus NOTIFY connectionStatusChanged FINAL)
    Q_PROPERTY(bool myChannelSavesChat READ myChannelSavesChat WRITE setMyChannelSavesChat NOTIFY myChannelSavesChatChanged FINAL)
    Q_PROPERTY(int connectedServerId READ connectedServerId WRITE setConnectedServerId NOTIFY connectedServerIdChanged FINAL) //using m_connectedServerId_onDb
    Q_PROPERTY(QString myAvatarPath READ myAvatarPath WRITE setMyAvatarPath NOTIFY myAvatarPathChanged FINAL)

    Q_PROPERTY(QString myAppVersion READ myAppVersion NOTIFY myAppVersionChanged FINAL)
    Q_PROPERTY(QString buildType READ buildType CONSTANT)
    Q_PROPERTY(QString appTitle READ appTitle CONSTANT)
    Q_PROPERTY(BeanChatCommon::Presence::Status myStatus READ myStatus WRITE setMyStatus NOTIFY myStatusChanged FINAL)
    Q_PROPERTY(quint64 myChannelId READ myChannelId WRITE setMyChannelId NOTIFY myChannelIdChanged FINAL)
};

#endif // USER_H
