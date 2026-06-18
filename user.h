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
#include <QTimer>
#include <QDataStream>

#include <QNetworkDatagram>

class User : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString messages READ getMessages WRITE setMessages NOTIFY messagesChanged FINAL)

public:
    explicit User(ChannelModel *channelModel, QObject *parent = nullptr);

    Q_INVOKABLE void joinChannel(int channelId);
    Q_INVOKABLE void login(QString username, QString tokenlike);
    Q_INVOKABLE void createChannel(QString channelName, QString password);
    Q_INVOKABLE void sendMessage(QString message);
    Q_INVOKABLE void askForServerState();

    QString getMessages() const;
    void setMessages(const QString &newMessages);

    int myId() const;
    void setMyId(int newMyId);

    QString myUsername() const;
    void setMyUsername(const QString &newMyUsername);

    void sendVoicePcm(const QByteArray& pcm);
signals:

    void messagesChanged();

    void myIdChanged();

    void myUsernameChanged();

    void voiceReceived(QByteArray pcm);

public slots:
    void onReadyRead();

private slots:
    // void sendFakeVoice();

private:
    QTcpSocket socket;
    QUdpSocket m_udpSocket;
    QString m_myUsername = "";
    int m_myId =-1;
    quint64 m_myChannelId=1; //default is lobby id 1

    //voice
    QTimer m_voiceTimer;
    quint32 m_sequence = 0;

    QString m_messages;

    ChannelModel* m_channelModel=nullptr;
    Q_PROPERTY(int myId READ myId WRITE setMyId NOTIFY myIdChanged FINAL)
    Q_PROPERTY(QString myUsername READ myUsername WRITE setMyUsername NOTIFY myUsernameChanged FINAL)
};

#endif // USER_H
