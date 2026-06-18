#include "user.h"

User::User(ChannelModel *channelModel, QObject *parent)
    : QObject{parent}, m_channelModel(channelModel)
{
    qDebug() << "user starting..";

    connect(&socket,
            &QTcpSocket::readyRead,
            this,
            &User::onReadyRead);


    m_udpSocket.bind();

    //for test send fake voice data..
    // connect(
    //     &m_voiceTimer,
    //     &QTimer::timeout,
    //     this,
    //     &User::sendFakeVoice);

    // m_voiceTimer.start(20);

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

                emit voiceReceived(
                    packet.audioData);

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

// void User::sendFakeVoice()
// {
//     VoicePacket voice;

//     voice.senderId = myId();

//     voice.sequence = ++m_sequence;

//     voice.audioData = QByteArray(320,'A');

//     QByteArray data;

//     QDataStream out(
//         &data,
//         QIODevice::WriteOnly);

//     out << quint16(101);
//     out << voice;

//     m_udpSocket.writeDatagram(
//         data,
//         QHostAddress("127.0.0.1"),
//         9988);

//     qDebug()
//         << "Voice packet sent"
//         << voice.sequence;
// }

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
}

void User::sendMessage(QString message)
{
    SendMessagePacket sm;
    sm.text = message;

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
        }
        else if(resp.channelId == m_myChannelId)
            qDebug() << "voice: user joined to your channel.";
        else if(resp.oldChannelId ==m_myChannelId)
            qDebug() << "voice: user left your channel.";
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
        auto msg =
            PacketHelpers::unpack<ChatMessagePacket>(
                packet.payload);

        qDebug()
            << "Channel"
            << msg.channelId
            << "Message:"
            << msg.text;
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

        // qDebug()
        //     << "Channels:"
        //     << state.channels.size();

        // qDebug()
        //     << "Users:"
        //     << state.users.size();

        // for(auto channel :
        //      state.channels)
        // {
        //     qDebug()
        //     << "Channel"
        //     << channel.id
        //     << channel.name;
        // }

        // for(auto user :
        //      state.users)
        // {
        //     qDebug()
        //     << "User"
        //     << user.username
        //     << "Channel"
        //     << user.channelId;
        // }
    }
    break;

    default:
        break;
    }
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
