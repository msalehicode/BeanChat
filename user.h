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

// include from BeanChatServer to server and client speak one language and undrestand packets
#include "network/packet.h"
#include "network/packets.h"
#include "network/packethelpers.h"
#include "network/voicepackets.h"
#include "models/user.h"


// C++ models to expose data to QML lists
#include "channelmodel.h"
#include "chatmodel.h"
#include "participantmodel.h"
#include "myserversmodel.h"
#include "connectedusersmodel.h"


//capture resources
#include "cameracapture.h"
#include "audiocapture.h"
#include "audiospeaker.h"


#include "settingsmanager.h"
#include "soundmanager.h"
#include "database.h"
#include "opuscodec.h"
#include "ffmpegdecoder.h"


enum class UserConnectionStatus
{
    Unknown,
    Connecting,
    Connected,
    ConnectionLost,
    Rejected,
    Error,
    Disconnected
};

class User : public QObject
{
    Q_OBJECT

public:
    explicit User(ChannelModel *channelModel,ChatModel* chatModel,
                  ParticipantModel* currentChannelParticipant,ConnectedUsersModel* connectedUsersModel, MyServersModel* myServersModel,
                  SoundManager* sounderManager, SettingsManager* settingsManager,
                  CameraCapture* cam, AudioCapture* mic, AudioSpeaker* speaker,
                  QObject *parent = nullptr);

    Q_INVOKABLE void joinChannel(int channelId, const QString& password="");
    Q_INVOKABLE void connectToServer(bool saveThisConnection, const QString& serverIp, const QString& str_serverPort);
    Q_INVOKABLE void updateSavedServer(quint64 serverId, quint64 dbIndex, const QString &name, const QString &ip, const QString &port);
    Q_INVOKABLE void deleteSavedServer(quint64 serverId, quint64 serverDbIndex=-1);
    Q_INVOKABLE void switchOrConnectToServer(const QString& serverIp, const QString& str_serverPort, int serverId);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void createChannel(QString channelName, QString password, bool saveMessages);
    Q_INVOKABLE void sendMessage(QString message);

    void askForServerState();

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

    QString myIdentity() const;
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
    UserConnectionStatus connectionStatus() const;
    void setConnectionStatus(UserConnectionStatus newConnectionStatus);

    bool myChannelSavesChat() const;
    void setMyChannelSavesChat(bool newMyChannelSavesChat);

signals:

    void myIdChanged();

    void myUsernameChanged();

    void voiceReceived(QByteArray pcm);
    void videoReceived(QByteArray data);

    void muteHeadphoneChanged();

    void muteMicrophoneChanged();

    void myChannelNameChanged();


    void userJoined();
    void userLeft();
    void newMessage();
    void messageSent();

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

public slots:
    void onTcpReadyRead();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);

    void onUdpReadyRead();
    void sendVideoFrame(const QByteArray& videoData);

private:
    QString platformName();
    void processPacket(const Packet& packet);
    void loginToUdpSocket();

    Database m_database;
    SoundManager* m_soundManager;
    SettingsManager* m_settingsManager;
    OpusCodec m_opus;

    //user can modify
    QString m_myUsername = USER_DEFAULT_USERNAME;
    QString m_myIdentity = USER_DEFAULT_IDENTITY;


    //user cant modify, would receive from target server
    int m_myId =-1;
    quint64 m_myChannelId=-2; //channelId -1 is default value for those users didn't connect to any channel just connected to server.
    QString m_myChannelName = ""; //current channel
    bool m_myChannelSavesChat=false;
    QString m_myServerName= ""; //current server connected to
    UserModel m_info; //to store system info such as appVersio and ..
    Participant* m_me=nullptr; //hold this user info and update it when channel switched, to connect with cameraCapture and show images as local preview
    UserConnectionStatus m_connectionStatus=UserConnectionStatus::Unknown;

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
    quint64 m_lastVideoSender = 0; //temporary, need to be REPLACED --------------

    //speaker
    AudioSpeaker* m_speaker=nullptr;
    bool m_muteHeadphone=false;

    //pointers to access and contorl esources/models    
    ChannelModel* m_channelModel=nullptr;
    ChatModel* m_chatModel=nullptr;
    ParticipantModel* m_currentChannelParticipant=nullptr;
    MyServersModel* m_myServersModel=nullptr;
    ConnectedUsersModel* m_connectedUsersModel=nullptr;


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
    Q_PROPERTY(QString myAppVersion READ myAppVersion)
    Q_PROPERTY(UserConnectionStatus connectionStatus READ connectionStatus WRITE setConnectionStatus NOTIFY connectionStatusChanged FINAL)
    Q_PROPERTY(bool myChannelSavesChat READ myChannelSavesChat WRITE setMyChannelSavesChat NOTIFY myChannelSavesChatChanged FINAL)
};

#endif // USER_H
