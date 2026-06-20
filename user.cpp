#include "user.h"
#include <QTimer>

User::User(ChannelModel *channelModel, ChatModel *chatModel,
            ParticipantModel* currentChannelParticipant,CameraCapture* cam,
            QObject *parent)
    : QObject{parent}, m_channelModel(channelModel), m_chatModel(chatModel),
    m_currentChannelParticipant(currentChannelParticipant), m_cam(cam)
{
    qDebug() << "user starting..";


    //setup socket
    m_udpSocket.bind();

    connect(&socket,
            &QTcpSocket::readyRead,
            this,
            &User::onTcpReadyRead);

    connect(&m_udpSocket,
            &QUdpSocket::readyRead,
            this,
            &User::onUdpReadyRead);

    /*
     * make a connection between channelmodel  to participantModel (center of screen [those rectangles]) sync with channelModel user's talking status
     * channelModel -> (which contineusly check users of current channel to find out who stopped talking due to last talking time)
    */
    connect(m_channelModel, &ChannelModel::userTalkingStatus,
            m_currentChannelParticipant, &ParticipantModel::setTalking);



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

    if(username=="")
    {
        //do a default and random name..
        setMyUsername("BeanUser"+QString::number(QRandomGenerator::global()->bounded(100)));
    }
    else
    {
        setMyUsername(username);
    }
    login.username = myUsername();
    login.identity = tokenlike;

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
    if(muteMicrophone() | muteHeadphone())
    {
        // qDebug() << "mic or headphone is muted.";
        return;
    }

    if(m_myId < 0)
    {
        // qDebug() << "invalid my id.";
        return;
    }

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
    {
        m_channelModel->setUserTalking(myId(),true);

        //update participantmodel too
        m_currentChannelParticipant->setTalking(myId(),true);
    }
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

void User::onTcpReadyRead()
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

    case PacketType::UserCameraClosed:
    case PacketType::UserCameraOpened:
    {
        auto resp =
            PacketHelpers::unpack<UserStatusChangedPacket>(
                packet.payload);

        if(resp.userId==myId()) //if it's himself update locaol user's variable therefore change icon at userStuff (user mic,speaker,...)
        {
            m_isCameraOpen=resp.status;

            // start or stop camera
            if(m_cam)
            {
                if(resp.status)
                {
                    qDebug() << "staring camera..";
                    m_cam->start();
                }
                else
                {
                    qDebug() << "stopping camera..";
                    m_cam->stop();
                }
            }
            else
                qDebug() << "camera is not ready. maybe say close camera to server.";

            emit isCameraOpenChanged();
        }
        else
        {
            qDebug() << "update cmaera for user:" << resp.userId << " to " << resp.status;
        }

        m_channelModel->setUserHasVideo(resp.userId,resp.status);
    }break;


    case PacketType::UserMuted:
    case PacketType::UserUnmuted:
    {
        auto resp =
            PacketHelpers::unpack<UserStatusChangedPacket>(
                packet.payload);

        if(resp.userId==myId()) //if it's himself update locaol user's variable therefore change icon at userStuff (user mic,speaker,...)
        {
            m_muteMicrophone=resp.status;

            //close mic.

            emit muteMicrophoneChanged();
        }
        else
        {
            qDebug() << "update mute for user:" << resp.userId << " to " << resp.status;
        }

        m_channelModel->setUserMuted(resp.userId,resp.status);
    }break;


    case PacketType::UserDeafened:
    case PacketType::UserUndeafened:
    {
        auto resp =
            PacketHelpers::unpack<UserStatusChangedPacket>(
                packet.payload);

        if(resp.userId==myId()) //if it's himself update locaol user's variable therefore change icon at userStuff (user mic,speaker,...)
        {
            m_muteHeadphone=resp.status;

            emit muteHeadphoneChanged();
        }
        else
        {
            qDebug() << "update speaker/headphone for user:" << resp.userId << " to " << resp.status;
        }

        m_channelModel->setUserDeafened(resp.userId,resp.status);
    }break;

    case PacketType::UserJoinedChannel:
    {
        auto resp =
            PacketHelpers::unpack<UserJoinedChannelPacket>(
                packet.payload);

        //update position of user on channelModel (do it before process, because in code won't see user's change on that channel..
        m_channelModel->moveUser(resp.userId,resp.channelId);


        if(resp.userId == static_cast<quint64>(myId()))
        {
            qDebug() << "voice: channel switched.";


            //set channel name for Chat and other parts to know current channel name
            m_myChannelId = resp.channelId;
            setMyChannelName(m_channelModel->getChannelName(m_myChannelId));

            m_channelModel->setCurrentChannelId(m_myChannelId);

            //add each users of this channel of (which user joint) into participantModel
            m_currentChannelParticipant->clear(); //clear participant model data.
            ChannelItem* channel = m_channelModel->findChannel(resp.channelId);
            if(channel)
            {
                //add found users into participant model
                for(const UserItem& user: channel->users)
                    m_currentChannelParticipant->addUser(user.id,user.username,user.isTalking,user.deafened,user.hasVideo);
            }
            else
                qDebug() << "could not find channel id:" << resp.channelId;
        }
        else if(resp.channelId == m_myChannelId)
        {
            qDebug() << "voice: user joined to your channel.";

            //find channel and that user info and add into participantModel
            ChannelItem* channel = m_channelModel->findChannel(resp.channelId);
            if(channel)
            {
                UserItem* jointUser = m_channelModel->findUserInChannel(channel,resp.userId);
                if(jointUser)
                    m_currentChannelParticipant->addUser(jointUser->id,jointUser->username,jointUser->isTalking,jointUser->deafened,jointUser->hasVideo);
                else
                    qDebug() << "user joined could not find user inside channel id:" << resp.channelId << " userid=" << resp.userId;
            }
            else
                qDebug() << "user joined could not find channel id:" << resp.channelId;

            //for soundmanager to play effect.
            emit userJoined();
        }

        else if(resp.oldChannelId==m_myChannelId)
        {
            qDebug() << "voice: user left your channel.";

            //remove that user from participant model
            m_currentChannelParticipant->removeUser(resp.userId);

            //for soundmanager to play effect
            emit userLeft();
        }
        else
            qInfo () << "user (" << resp.userId << ") has left " << resp.oldChannelId << " and joined to " << resp.channelId ;

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

        m_channelModel->addUser(1,user.id,user.username,user.muted,user.deafened,user.camera);

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


        //we dont know if user was inside our channel or not but anyway try to remove him from participantmodel.
        m_currentChannelParticipant->removeUser(user.id);
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
                u.deafened,
                u.camera);
            qDebug() << "user " << u.username << " camra=" << u.camera;
        }

        qInfo() << "voice: connected to server.";
        m_channelModel->setCurrentChannelId(m_myChannelId);

    }
    break;

    default:
        break;
    }
}

void User::onUdpReadyRead()
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

        if(type == 101) //voice
        {
            VoicePacket packet;
            in >> packet;

            if(!muteHeadphone())
            {
                UserItem* senderUser = m_channelModel->getUser(m_myChannelId, packet.senderId);
                if(!senderUser)
                    continue;

                if(!senderUser->isTalking)
                {
                    m_channelModel->setUserTalking(senderUser->id,true);

                    //update participantmodel too
                    m_currentChannelParticipant->setTalking(packet.senderId,true);
                }

                senderUser->lastVoicePacket.restart();
                emit voiceReceived(packet.audioData);
            }

            // qDebug()
            //     << "Voice received from"
            //     << packet.senderId
            //     << "seq"
            //     << packet.sequence
            //     << "size"
            //     << packet.audioData.size();
        }
        else if(type ==102) //video
        {
            VideoPacket packet;

            in >> packet;

            qDebug()
                << "Video received from "
                << packet.senderId
                << "seq="
                << packet.sequence
                << "size="
                << packet.videoData.size();

            Participant* senderParticipant = m_currentChannelParticipant->findUser(packet.senderId);
            if(!senderParticipant)
            {
                qDebug() << "could not find sender as participant..";
                continue;
            }

            //decode.
            QImage image;

            if(!image.loadFromData(
                    packet.videoData,
                    "JPG"))
            {
                qDebug() << "Failed to decode jpeg";
                continue;
            }

            VideoSink* senderSink = senderParticipant->videoSink();
            if(senderSink)
            {
                senderParticipant->setIsCameraOpen(true);
                senderSink->setImage(image);
                qDebug() << "image set fine to sender's sink..";
            }
            else
            {
                qDebug() << "fialed to load sender sink.";
                continue;
            }

            // emit videoReceived(packet.videoData);
        }
        else
            continue;
    }
}

void User::sendVideoFrame(const QByteArray &jpegData)
{
    if(!isCameraOpen())
        return;

    if(myId() < 0)
        return;

    VideoPacket videoPacket;

    videoPacket.senderId = static_cast<quint64>(myId());

    videoPacket.sequence = ++m_videoSequence;

    videoPacket.videoData = jpegData;

    QByteArray data;

    QDataStream out(
        &data,
        QIODevice::WriteOnly);

    out << quint16(102);
    out << videoPacket;

    qDebug() << "Sending Video JPEG bytes:" << jpegData.size();

    m_udpSocket.writeDatagram(
        data,
        QHostAddress("127.0.0.1"),
        9988);

    //update self status and show preview.
    m_currentChannelParticipant->setCameraOpen(myId(),true);
    Participant* me = m_currentChannelParticipant->findUser(myId());

    QObject::connect(
        m_cam,
        &CameraCapture::imageReady,
        me->videoSink(),
        &VideoSink::setImage,
        Qt::QueuedConnection);
}

bool User::isCameraOpen() const
{
    return m_isCameraOpen;
}

void User::setIsCameraOpen(bool status)
{
    if(m_isCameraOpen==status)
        return;

    //send request to server.
    Packet p;
    if(status)
        p.type = PacketType::UserCameraOpened;
    else
        p.type = PacketType::UserCameraClosed;

    socket.write(p.serialize());
}

bool User::muteMicrophone() const
{
    return m_muteMicrophone;
}

void User::setMuteMicrophone(bool status)
{
    if(m_muteMicrophone==status)
        return;

    //send request to server.
    Packet p;
    if(status)
        p.type = PacketType::UserMuted;
    else
        p.type = PacketType::UserUnmuted;

    socket.write(p.serialize());
}

bool User::muteHeadphone() const
{
    return m_muteHeadphone;
}

void User::setMuteHeadphone(bool status)
{
    if(m_muteHeadphone==status)
        return;

    //send request to server.
    Packet p;
    if(status)
        p.type = PacketType::UserDeafened;
    else
        p.type = PacketType::UserUndeafened;

    socket.write(p.serialize());
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
