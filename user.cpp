#include "user.h"

User::User(QObject *parent)
    : QObject{parent}
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

    join.channelId = 2;
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

    login.username = username;
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

void User::onReadyRead()
{
    QByteArray data =
        socket.readAll();

    Packet packet =
        Packet::deserialize(data);

    setMessages(data);
    switch(packet.type)
    {
    case PacketType::LoginResponse:
    {
        auto resp =
            PacketHelpers::unpack<LoginResponsePacket>(
                packet.payload);

        qDebug()
            << "Login:"
            << resp.accepted
            << resp.message;

        break;
    }

    case PacketType::UserConnected:
    {
        auto user =
            PacketHelpers::unpack<UserConnectedPacket>(
                packet.payload);

        qDebug()
            << "User joined:"
            << user.username;

        break;
    }

    case PacketType::UserDisconnected:
    {
        auto user =
            PacketHelpers::unpack<UserConnectedPacket>(
                packet.payload);

        qDebug()
            << "User disconnected:"
            << user.username;

        break;
    }

    case PacketType::ChatMessage:
    {
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

    default:
        break;
    }
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
