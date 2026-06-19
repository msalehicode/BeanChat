#include "user.h"
#include <QTimer>

User::User(ChannelModel *channelModel, ChatModel *chatModel, QObject *parent)
    : QObject{parent}, m_channelModel(channelModel), m_chatModel(chatModel)
{
    qDebug() << "user starting..";

    connect(&socket,
            &QTcpSocket::readyRead,
            this,
            &User::onReadyRead);


    m_udpSocket.bind();


    connect(
        &m_udpSocket,
        &QUdpSocket::readyRead,
        this,
        [this]()
        {
            while(m_udpSocket.hasPendingDatagrams())
            {
                auto datagram =
                    m_udpSocket.receiveDatagram();

                QByteArray data =
                    datagram.data();

                QDataStream in(data);

                quint16 type;

                in >> type;

                if(type != 101)
                    continue;

                VoicePacket packet;

                in >> packet;



                if(!muteHeadphone())
                {
                    UserItem* senderUser = m_channelModel->getUser(m_myChannelId, packet.senderId);
                    if(!senderUser)
                        return;

                    if(!senderUser->isTalking)
                    {
                        m_channelModel->updateUserStatus(packet.senderId, true, senderUser->muted,senderUser->deafened);
                    }

                    senderUser->lastVoicePacket.restart();
                    emit voiceReceived(packet.audioData);
                }
                    // emit voiceReceived(packet.audioData);

                // qDebug()
                //     << "Voice received from"
                //     << packet.senderId
                //     << "seq"
                //     << packet.sequence
                //     << "size"
                //     << packet.audioData.size();
            }
        });

}


void User::joinChannel(int channelId)
{
    JoinChannelPacket join;

    join.channelId = channelId;
    join.password = "123";

    Packet p;

    p.type = PacketType::JoinChannel;
    p.payload = PacketHelpers::pack(join);

    socket.write(p.serialize());
}

void User::login(QString username, QString tokenlike)
{
    socket.connectToHost(
        "127.0.0.1",
        9987);

    LoginRequestPacket login;

    setMyUsername(username);
    login.username = myUsername();
    login.identity = "ABC123";

    Packet p;
    p.type = PacketType::LoginRequest;
    p.payload = PacketHelpers::pack(login);

    socket.write(p.serialize());
}

void User::createChannel(QString channelName, QString password)
{
    CreateChannelPacket cc;
    cc.name = "Gaming";
    cc.password="123";
    cc.permanentChat=true;
    cc.temporaryChat=false;

    Packet p;
    p.type = PacketType::CreateChannel;
    p.payload = PacketHelpers::pack(cc);

    socket.write(p.serialize());
}

void User::sendVoicePcm(
    const QByteArray& pcm)
{
    if(muteMicrophone())
        return;

    if(m_myId < 0)
        return;

    VoicePacket voice;

    voice.senderId =
        static_cast<quint64>(myId());

    voice.sequence =
        ++m_sequence;

    voice.audioData =
        pcm;

    QByteArray data;

    QDataStream out(
        &data,
        QIODevice::WriteOnly);

    out << quint16(101);
    out << voice;

    m_udpSocket.writeDatagram(
        data,
        QHostAddress("127.0.0.1"),
        9988);

    //update isTalking this user/device
    UserItem* senderUser = m_channelModel->getUser(m_myChannelId, myId());
    if(!senderUser)
        return;

    if(!senderUser->isTalking)
        m_channelModel->updateUserStatus(myId(), true, senderUser->muted,senderUser->deafened);
    senderUser->lastVoicePacket.restart();
}

void User::sendMessage(QString message)
{
    SendMessagePacket sm;
    sm.text = message;
    sm.type = SendMessagePacket::Type::Text;
    sm.mediaPath = "";

    Packet p;
    p.type = PacketType::ChatMessage;
    p.payload = PacketHelpers::pack(sm);

    socket.write(p.serialize());
}

void User::askForServerState()
{
    qDebug() << "asking for server State packet..";
    ServerStatePacket ssp;

    Packet p;
    p.type = PacketType::RequestServerState;
    p.payload = PacketHelpers::pack(ssp);

    socket.write(p.serialize());
}

void User::onReadyRead()
{
    QByteArray data =
        socket.readAll();

    Packet packet =
        Packet::deserialize(data);

    qInfo() << "received message: code:" << static_cast<int>(packet.type);
    setMessages(data);
    switch(packet.type)
    {

    case PacketType::ChannelCreated:
    {
        qDebug() <<"a channel created";
        auto resp =
            PacketHelpers::unpack<ChannelCreatedPacket>(
                packet.payload);

        m_channelModel->addChannel(resp.id,resp.name);
    }break;

    case PacketType::UserJoinedChannel:
    {
        auto resp =
            PacketHelpers::unpack<UserJoinedChannelPacket>(
                packet.payload);

        if(resp.userId == static_cast<quint64>(myId()))
        {
            qDebug() << "voice: channel switched.";
            m_myChannelId = resp.channelId;
            m_channelModel->setCurrentChannelId(m_myChannelId);

            //set channel name for Chat
            setMyChannelName(m_channelModel->getChannelName(m_myChannelId));
        }
        else if(resp.channelId == m_myChannelId)
        {
            qDebug() << "voice: user joined to your channel.";
            emit userJoined();
        }

        else if(resp.oldChannelId ==m_myChannelId)
        {
            qDebug() << "voice: user left your channel.";
            emit userLeft();
        }
        else
            qInfo () << "user (" << resp.userId << ") has left " << resp.oldChannelId << " and joined to " << resp.channelId ;

        m_channelModel->moveUser(resp.userId,resp.channelId);
    }break;

    case PacketType::LoginResponse:
    {
        qInfo() << "login response:";
        auto resp =
            PacketHelpers::unpack<LoginResponsePacket>(
                packet.payload);

        qDebug()
            << "-------------------------- Login:"
            << resp.accepted
            << resp.message;


        if(!resp.accepted)
            qInfo() << "login.. failed" ;
        else
        {
            setMyId(resp.id);
            qDebug() << "my id is=" << myId();
            askForServerState();

            //----------------udp voice. register
            UdpRegisterPacket reg;

            reg.userId = static_cast<quint64>(myId());

            QByteArray data;

            QDataStream out(
                &data,
                QIODevice::WriteOnly);

            out << quint16(100);
            out << reg;

            m_udpSocket.writeDatagram(
                data,
                QHostAddress("127.0.0.1"),
                9988);
            qDebug() << " just sent a register udp message to server.";
        }

        qDebug() << "LOGIN MESSAGE= " << resp.message;
        break;
    }

    case PacketType::UserConnected:
    {
        qInfo() << "user connected:";
        auto user =
            PacketHelpers::unpack<UserConnectedPacket>(
                packet.payload);

        qDebug()
            << "User joined:"
            << user.username;

        m_channelModel->addUser(1,user.id,user.username,false,false);

        break;
    }

    case PacketType::UserDisconnected:
    {
        qInfo() << "user disconnected:";
        auto user =
            PacketHelpers::unpack<UserConnectedPacket>(
                packet.payload);

        qDebug()
            << "User disconnected:"
            << user.username;

        m_channelModel->removeUser(user.id);
        break;
    }

    case PacketType::ChatMessage:
    {
        qInfo() << "chat message:";
        auto msg = PacketHelpers::unpack<ChatMessagePacket>(packet.payload);

        m_chatModel->addMessage(msg);
        emit newMessage();
    }
    break;

    case PacketType::ServerState:
    {
        qInfo() << "server state:";
        auto state =
            PacketHelpers::unpack<ServerStatePacket>(
                packet.payload);

        if(!m_channelModel)
            return;

        m_channelModel->clear();

        for(auto& c : state.channels)
        {
            m_channelModel->addChannel(
                c.id,
                c.name);
        }

        for(auto& u : state.users)
        {
            m_channelModel->addUser(
                u.channelId,
                u.id,
                u.username,
                u.muted,
                u.deafened);
        }

        qInfo() << "voice: connected to server.";
        m_channelModel->setCurrentChannelId(m_myChannelId);

    }
    break;

    default:
        break;
    }
}

bool User::muteMicrophone() const
{
    return m_muteMicrophone;
}

void User::setMuteMicrophone(bool newMuteMicrophone)
{
    if (m_muteMicrophone == newMuteMicrophone)
        return;
    m_muteMicrophone = newMuteMicrophone;
    emit muteMicrophoneChanged();
}

bool User::muteHeadphone() const
{
    return m_muteHeadphone;
}

void User::setMuteHeadphone(bool newMuteHeadphone)
{
    if (m_muteHeadphone == newMuteHeadphone)
        return;
    m_muteHeadphone = newMuteHeadphone;
    emit muteHeadphoneChanged();
}

QString User::myUsername() const
{
    return m_myUsername;
}

void User::setMyUsername(const QString &newMyUsername)
{
    if (m_myUsername == newMyUsername)
        return;
    m_myUsername = newMyUsername;
    emit myUsernameChanged();
}

int User::myId() const
{
    return m_myId;
}

void User::setMyId(int newMyId)
{
    if (m_myId == newMyId)
        return;
    m_myId = newMyId;
    emit myIdChanged();
}

QString User::myChannelName() const
{
    return m_myChannelName;
}

void User::setMyChannelName(const QString& name)
{
    if(m_myChannelName == name)
        return;

    m_myChannelName = name;
    emit myChannelNameChanged();
}

void User::setMessages(const QString &newMessages)
{
    if (m_messages == newMessages)
        return;
    m_messages += "\n"+  newMessages;
    emit messagesChanged();
}

QString User::getMessages() const
{
    return m_messages;
}
