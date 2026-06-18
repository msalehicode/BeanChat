#include "user.h"

User::User(ChannelModel *channelModel, QObject *parent)
    : QObject{parent}, m_channelModel(channelModel)
{
    qDebug() << "user starting..";

    connect(&socket,
            &QTcpSocket::readyRead,
            this,
            &User::onReadyRead);

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
        qInfo () << "user joined a channel";
        auto resp =
            PacketHelpers::unpack<UserJoinedChannelPacket>(
                packet.payload);

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
