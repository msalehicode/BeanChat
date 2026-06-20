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
#include <QDataStream>

#include <QRandomGenerator> //to get a random number for default username
#include <QNetworkDatagram>

#include "cameracapture.h"

class User : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString messages READ getMessages WRITE setMessages NOTIFY messagesChanged FINAL)

public:
    explicit User(ChannelModel *channelModel,ChatModel* chatModel,
                  ParticipantModel* currentChannelParticipant,CameraCapture* cam,
                  QObject *parent = nullptr);

    Q_INVOKABLE void joinChannel(int channelId);
    Q_INVOKABLE void login(QString username="", QString tokenlike="defaultBeanChatIdentity");
    Q_INVOKABLE void createChannel(QString channelName, QString password);
    Q_INVOKABLE void sendMessage(QString message);
    Q_INVOKABLE void askForServerState();

    QString getMessages() const;
    void setMessages(const QString &newMessages);

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

signals:

    void messagesChanged();

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
    QString m_myUsername = "";
    int m_myId =-1;
    quint64 m_myChannelId=1;
    QString m_myChannelName = "Chat";

    //voice
    quint32 m_sequence = 0;

    //video
    quint32 m_videoSequence = 0;

    QString m_messages;

    ChannelModel* m_channelModel=nullptr;
    ChatModel* m_chatModel=nullptr;
    ParticipantModel* m_currentChannelParticipant=nullptr;
    CameraCapture* m_cam=nullptr;
    Q_PROPERTY(int myId READ myId WRITE setMyId NOTIFY myIdChanged FINAL)
    Q_PROPERTY(QString myUsername READ myUsername WRITE setMyUsername NOTIFY myUsernameChanged FINAL)
    Q_PROPERTY(QString myChannelName READ myChannelName NOTIFY myChannelNameChanged FINAL)
    Q_PROPERTY(bool muteHeadphone READ muteHeadphone WRITE setMuteHeadphone NOTIFY muteHeadphoneChanged FINAL)
    Q_PROPERTY(bool muteMicrophone READ muteMicrophone WRITE setMuteMicrophone NOTIFY muteMicrophoneChanged FINAL)
    Q_PROPERTY(bool isCameraOpen READ isCameraOpen WRITE setIsCameraOpen NOTIFY isCameraOpenChanged FINAL)
};

#endif // USER_H
