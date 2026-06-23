#ifndef USER_H
#define USER_H

#include <QObject>
#include <QTcpSocket>
#include "../BeanChatServer/src/network/packet.h"
#include "../BeanChatServer/src/network/packets.h"
#include "../BeanChatServer/src/network/packethelpers.h"
#include "../BeanChatServer/src/network/voicepackets.h"

#include <QUdpSocket>
#include "channelmodel.h"
#include "chatmodel.h"
#include "participantmodel.h"
#include "myserversmodel.h"

#include <QDataStream>

#include <QRandomGenerator> //to get a random number for default username
#include <QNetworkDatagram>

#include "cameracapture.h"
#include "audiocapture.h"
#include "audiospeaker.h"

#include "connectedusersmodel.h"
class User : public QObject
{
    Q_OBJECT

public:
    explicit User(ChannelModel *channelModel,ChatModel* chatModel,
                  ParticipantModel* currentChannelParticipant,ConnectedUsersModel* connectedUsersModel, MyServersModel* myServersModel,
                  CameraCapture* cam, AudioCapture* mic, AudioSpeaker* speaker,
                  QObject *parent = nullptr);

    Q_INVOKABLE void joinChannel(int channelId, const QString& password="");
    Q_INVOKABLE void connectToServer(bool saveThisConnection, const QString& serverIp, const QString& str_serverPort);
    Q_INVOKABLE void connectToServer(const QString& serverIp, const QString& str_serverPort, int serverId);
    Q_INVOKABLE void disconnect(bool switchingServer=false);
    Q_INVOKABLE void createChannel(QString channelName, QString password);
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

    void isCameraOpenChanged();

    void myServerNameChanged();

    void isConnectedToServerChanged();

    void myIdentityChanged();

    void isChatOpenChanged();

    void chatUnreadMessagesChanged();

public slots:
    void onTcpReadyRead();
    void onUdpReadyRead();
    void sendVideoFrame(const QByteArray& jpegData);

private:
    bool m_muteMicrophone=false;
    bool m_muteHeadphone=false;
    bool m_isCameraOpen=false;


    QTcpSocket socket;
    QUdpSocket m_udpSocket;


    //user can modify
    QString m_myUsername = "";
    QString m_myIdentity = "identity";

    //user cant modify, would receive from target server
    int m_myId =-1;
    quint64 m_myChannelId=-2; //channelId -1 is default value for those users didn't connect to any channel just connected to server.
    QString m_myChannelName = "noChannel"; //current channel
    QString m_myServerName= ""; //current server connected to

    bool m_isConnectedToServer=false;


    //chat notification
    bool m_isChatOpen=false;//a flag to know is chatTab is active or not
    int m_chatUnreadMessages=0; //hold count of new unread messags when user is not in chatTab


    //to hold ip and ports for different parts of app such as sendVoice, sendVideo
    QString m_serverIp = "10.89.46.137";
    quint64 m_serverPort=9987; //udp port is this value+1

    //voice
    quint32 m_sequence = 0;

    //video
    quint32 m_videoSequence = 0;

    Participant* m_me=nullptr; //hold this user info and update it when channel switched, to connect with cameraCapture and show images as local preview

    ChannelModel* m_channelModel=nullptr;
    ChatModel* m_chatModel=nullptr;
    ParticipantModel* m_currentChannelParticipant=nullptr;
    MyServersModel* m_myServersModel=nullptr;

    CameraCapture* m_cam=nullptr;
    AudioCapture* m_mic=nullptr;
    AudioSpeaker* m_speaker=nullptr;

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
};

#endif // USER_H
