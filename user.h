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

#include "models/attachmentimageprovider.h"

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
#include "managers/badgemanager.h"

#include "models/identity.h"
#include <QStandardPaths>
#define SAVE_AVATAR_PATH QStandardPaths::writableLocation(QStandardPaths::CacheLocation)+"/servers/"


#include <QProcess>
#include "update/updatechecker.h"



//upload file in chat
#include <QFileInfo>
#include <QMimeDatabase>
#include <QCryptographicHash>

#include <QLocale>


//donwload attachment
#include "models/downloadsession.h"


//opus codec thread
#include <QThread>
#include "voiceworker.h"

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
                  AttachmentImageProvider* attachmentImageProvider,
                  CameraCapture* cam, AudioCapture* mic, AudioSpeaker* speaker,
                  QObject *parent = nullptr);
    ~User();


    //server actions
    Q_INVOKABLE void joinChannel(quint64 channelId, const QString& password="", bool isTextChannel=false);
    Q_INVOKABLE void moveUser(quint64 userId, quint64 channelId, const QString& password);
    Q_INVOKABLE void createChannel(QString channelName, QString password, bool saveMessages, bool isVoiceChannel);
    Q_INVOKABLE void sendMessage(const QString& message,quint64 attachId, const QUrl &url, quint64 channelId=0);
    Q_INVOKABLE void sendMessage(const QString& message, quint64 channelId=0);
    Q_INVOKABLE void sendFile(const QString& filePath, quint64 channelId=0);
    Q_INVOKABLE void downloadAttachment(quint64 attachId);
    Q_INVOKABLE bool hasAttachmentImage(quint64 attachmentId) const; //to know if image is downloaded or not (when list scorlls image component becomes downloaded=false by default)
    Q_INVOKABLE QUrl attachmentUrl(quint64 id); //for animated images need this to get actual path, imageProvider doesnt work

    Q_INVOKABLE void checkForUpdate();

    Q_INVOKABLE void updateChannel(quint64 channelId, const QString& name,
                                    const QString& pass, bool saveMessages);
    Q_INVOKABLE QString getChannelName(quint64 channelId); //to show channlename inside user's profile
    Q_INVOKABLE void deleteChannel(quint64 channelId);

    Q_INVOKABLE ClientUser *clientUser(quint64 id);


    Q_INVOKABLE void updateApp();
    //connect,disconnect
    Q_INVOKABLE void connectToServer(bool saveThisConnection, const QString& serverIp, const QString& str_serverPort);
    Q_INVOKABLE void switchOrConnectToServer(const QString& serverIp, const QString& str_serverPort, int serverId);


    //myServers actions
    Q_INVOKABLE void updateSavedServer(quint64 serverId, quint64 dbIndex, const QString &name, const QString &ip, const QString &port);
    Q_INVOKABLE void deleteSavedServer(quint64 serverId, quint64 serverDbIndex=-1);

    Q_INVOKABLE bool hasBadge(quint64 userId, BadgeManager::Badge badge) const;

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

    enum class UserConnectionStatus
    {
        Unknown,
        Connecting,
        Connected,
        ConnectionLost,
        Rejected,
        Error,
        Disconnecting, //when we are busy with reset variables..
        Disconnected
    };
    Q_ENUM(UserConnectionStatus)

    UserConnectionStatus connectionStatus() const;
    void setConnectionStatus(UserConnectionStatus newConnectionStatus);

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
    quint64 connectedUsersCount() const;
    void setConnectedUsersCount(quint64 newConnectedUsersCount);
    void setConnectedUsersCount(quint64 newConnectedUsersCount, bool increase);


    quint64 currentTextChannelId() const;
    void setCurrentTextChannelId(quint64 newCurrentTextChannelId);


    ChatModel* currentTextChatModel() const;

    template<typename T>
    void sendPacket(PacketType type, const T &payload);
    void sendPacket(PacketType type); // no payload

    qint64 sendUdp(const QByteArray &data);

    quint64 totalPacketsReceived();
    quint64 totalPacketsSent();
    QString totalBytesReceived();
    QString totalBytesSent();
    QString currentTextChannelName() const;
    void setCurrentTextChannelName(const QString &newCurrentTextChannelName);

    bool currentTextChannelSaveMessages() const;
    void setCurrentTextChannelSaveMessages(bool newCurrentTextChannelSaveMessages);

    bool checkUpdate() const;
    void setCheckUpdate(bool newCheckUpdate);

    QString updateRepositoryAddress() const;
    void setUpdateRepositoryAddress(const QString &newUpdateRepositoryAddress);

    QString currentTheme() const;
    void setCurrentTheme(const QString &newCurrentTheme);

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
    void userDisconnected();
    void userTimedOut();

    void cameraOpened();
    void cameraClosed();
    void micOpened();
    void micClosed();
    void speakerOpened();
    void speakerClosed();


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

    void connectionStatusChanged();

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

    void connectedUsersCountChanged();


    //send file signals:
    void sendFileResult(bool status, QString error,  quint64 attachId);

    void attachmentDownloadProgress(quint64 attachmentId, float progress);
    void attachmentDownloaded(quint64 attachmentId);
    void attachmentDownloadFailed(quint64 attachmentId, const QString &reason);

    void currentTextChannelIdChanged();
    void currentTextChatModelChanged();


    //network stats like packets sent and received..
    void networkStatsChanged();

    void currentTextChannelNameChanged();

    void currentTextChannelSaveMessagesChanged();


    //talk with other thread (voice thread)
    void decodeVoice(quint64 senderId, QByteArray opusData);


    void checkUpdateChanged();

    void updateRepositoryAddressChanged();

    void currentThemeChanged();

private slots:
    void onPcmDecoded(quint64 senderId, QByteArray pcm);

public slots:
    void onTcpReadyRead();
    Q_INVOKABLE void disconnect();
    void onSocketError(QAbstractSocket::SocketError error);

    void onUdpReadyRead();
    void sendVideoFrame(const QByteArray& videoData);

    void currentIdentityChangedTo(const QString& name);

    void sendVoiceOpus(const QByteArray& opus);

private:
    QString platformName();
    void resetVariables(); //when wanna disconnect and get ready for next connection
    void processPacket(const Packet& packet);
    void loginToUdpSocket();

    //appereance
    QString m_currentTheme;

    //voice
    QThread m_voiceThread;
    VoiceWorker* m_voiceWorker = nullptr;


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
    quint64 m_connectedUsersCount=0;
    QString m_myServerName= ""; //current server connected to (name that saved by user inside myServers, can be modified, only shown to this user)
    UserConnectionStatus m_connectionStatus=UserConnectionStatus::Unknown;
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


    //upload file in chat
    QFile m_uploadFile;
    quint64 m_currentUploadId = 0;
    QString m_uploadFilename;
    AttachmentImageProvider* m_attachmentImageProvider=nullptr;


    //download file in chat
    QHash<quint64, DownloadSession*> m_downloadSessions;

    //connect and switch servers.
    bool m_isConnectedToServer=false;
    bool m_switchingServer = false;

    //TCP connection
    QTcpSocket socket;
    QByteArray m_tcpBuffer;

    //hold TCP  statistics
    quint64 m_totalTcpBytesSent = 0;
    quint64 m_totalTcpPacketsSent = 0;

    quint64 m_totalUdpBytesSent = 0;
    quint64 m_totalUdpPacketsSent = 0;

    quint64 m_totalTcpBytesReceived = 0;
    quint64 m_totalTcpPacketsReceived = 0;

    quint64 m_totalUdpBytesReceived = 0;
    quint64 m_totalUdpPacketsReceived = 0;

    Q_PROPERTY(quint64 totalPacketsReceived READ totalPacketsReceived NOTIFY myPingChanged FINAL) //networkStatsChanged
    Q_PROPERTY(quint64 totalPacketsSent READ totalPacketsSent NOTIFY myPingChanged FINAL) //networkStatsChanged

    Q_PROPERTY(QString totalBytesReceived READ totalBytesReceived NOTIFY myPingChanged FINAL) //networkStatsChanged
    Q_PROPERTY(QString totalBytesSent READ totalBytesSent NOTIFY myPingChanged FINAL) //networkStatsChanged


    //UDP connection
    QUdpSocket m_udpSocket;
    int m_myPing=-1;
    float m_myVoicePacketLoss=0.0f;
    float m_myVideoPacketLoss=0.0f;
    QTimer m_udpConnectionTimeout;
    QElapsedTimer m_lastUdpActivity;
    QElapsedTimer m_lastTcpActivity; //to know is tcp alive or not so sometimes after tcp socket being idle would os kill it and we dont know are we online?! e.g max time is 5minutes
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
    ParticipantModel* m_currentChannelParticipant=nullptr;
    MyServersModel* m_myServersModel=nullptr;
    ConnectedUsersModel* m_connectedUsersModel=nullptr;

    //chats
    ChatModel* m_voiceChatModel=nullptr; //chat for current channel (voice channel)
    quint64 m_currentTextChannelId=0;//0 is none
    QString m_currentTextChannelName="";
    bool m_currentTextChannelSaveMessages=false;

    QHash<quint64, ChatModel*> m_textChatModels;
    //to expose currentTextModel
    Q_PROPERTY(ChatModel* currentTextChatModel  READ currentTextChatModel NOTIFY currentTextChatModelChanged)

    //update
    BadgeManager m_badgeManager;
    UpdateChecker* m_updateChecker=nullptr;
    bool m_checkUpdate=false;
    QString m_updateRepositoryAddress="";

    //reconnect when connection lost
    short m_reconnectTryCount=0;
    QTimer m_reconnectTimer; //when connection lost try every x seconds to re-connect

    //reconnect last server
    QString m_lastServerIp="";
    QString m_lastServerPort="";

    //re-join last channel
    quint64 m_lastChannelId=0;
    QString m_lastChannelPassword="";


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
    Q_PROPERTY(User::UserConnectionStatus connectionStatus READ connectionStatus WRITE setConnectionStatus NOTIFY connectionStatusChanged FINAL)
    Q_PROPERTY(bool myChannelSavesChat READ myChannelSavesChat WRITE setMyChannelSavesChat NOTIFY myChannelSavesChatChanged FINAL)
    Q_PROPERTY(int connectedServerId READ connectedServerId WRITE setConnectedServerId NOTIFY connectedServerIdChanged FINAL) //using m_connectedServerId_onDb
    Q_PROPERTY(QString myAvatarPath READ myAvatarPath WRITE setMyAvatarPath NOTIFY myAvatarPathChanged FINAL)

    Q_PROPERTY(QString myAppVersion READ myAppVersion NOTIFY myAppVersionChanged FINAL)
    Q_PROPERTY(QString buildType READ buildType CONSTANT)
    Q_PROPERTY(QString appTitle READ appTitle CONSTANT)
    Q_PROPERTY(BeanChatCommon::Presence::Status myStatus READ myStatus WRITE setMyStatus NOTIFY myStatusChanged FINAL)
    Q_PROPERTY(quint64 myChannelId READ myChannelId WRITE setMyChannelId NOTIFY myChannelIdChanged FINAL)
    Q_PROPERTY(quint64 connectedUsersCount READ connectedUsersCount WRITE setConnectedUsersCount NOTIFY connectedUsersCountChanged FINAL)
    Q_PROPERTY(quint64 currentTextChannelId READ currentTextChannelId WRITE setCurrentTextChannelId NOTIFY currentTextChannelIdChanged FINAL)
    Q_PROPERTY(QString currentTextChannelName READ currentTextChannelName WRITE setCurrentTextChannelName NOTIFY currentTextChannelNameChanged FINAL)
    Q_PROPERTY(bool currentTextChannelSaveMessages READ currentTextChannelSaveMessages WRITE setCurrentTextChannelSaveMessages NOTIFY currentTextChannelSaveMessagesChanged FINAL)
    Q_PROPERTY(bool checkUpdate READ checkUpdate WRITE setCheckUpdate NOTIFY checkUpdateChanged FINAL)
    Q_PROPERTY(QString updateRepositoryAddress READ updateRepositoryAddress WRITE setUpdateRepositoryAddress NOTIFY updateRepositoryAddressChanged FINAL)
    Q_PROPERTY(QString currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged FINAL)
};

#endif // USER_H
