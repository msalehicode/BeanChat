#include "user.h"
#include <QTimer>

User::User(ChannelModel *channelModel, ChatModel *chatModel,
           ParticipantModel* currentChannelParticipant, ConnectedUsersModel *connectedUsersModel,
           CameraCapture* cam, AudioCapture *mic, AudioSpeaker* speaker,
           QObject *parent)
    : QObject{parent}, m_channelModel(channelModel), m_chatModel(chatModel),
    m_currentChannelParticipant(currentChannelParticipant), m_connectedUsersModel(connectedUsersModel),
    m_cam(cam), m_mic(mic), m_speaker(speaker)
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
        m_targetIp,
        m_targetPort);

    setMyServerName(m_targetIp+":"+QString::number(m_targetPort));

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

void User::disconnect()
{
    //disocnnect sockets.
    socket.disconnectFromHost();
    m_udpSocket.disconnectFromHost();

    //clear models
    m_channelModel->clear();
    m_currentChannelParticipant->clear();
    m_connectedUsersModel->clear();
    m_chatModel->clear();

    //reset variables
    setMyServerName("");
    setMyChannelName("");
    m_me=nullptr;

    //because of method we do use setter to send requests to server
    //then if server allowed/responsed we would do emits so, have to here reset them manually cant use setter.
    m_muteHeadphone=false;
    emit muteHeadphoneChanged();

    m_muteMicrophone=false;
    emit muteMicrophoneChanged();

    m_isCameraOpen=false;
    emit isCameraOpenChanged();


    //release resourses
    if(m_cam)
        m_cam->stop();

    if(m_mic)
        m_mic->stop();

    if(m_speaker)
        m_speaker->stop();

}

void User::createChannel(QString channelName, QString password)
{
    CreateChannelPacket cc;
    cc.name = "Gaming"+QString::number(QRandomGenerator::global()->bounded(100));
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
        QHostAddress(m_targetIp),
        m_targetUdpPort);

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
                m_me = m_currentChannelParticipant->findUser(myId());
                if(!m_me)
                    return;

                if(resp.status)
                {
                    qDebug() << "staring camera..";
                    m_cam->start();


                    //feed this user's vidoeSink with preview.
                    QObject::connect(
                        m_cam,
                        &CameraCapture::imageReady,
                        m_me->videoSink(),
                        &VideoSink::setImage,
                        Qt::QueuedConnection);
                }
                else
                {
                    qDebug() << "stopping camera..";
                    m_cam->stop();
                    QObject::disconnect(
                        m_cam,
                        &CameraCapture::imageReady,
                        m_me->videoSink(),
                        &VideoSink::setImage);
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
        m_currentChannelParticipant->setCameraOpen(resp.userId,resp.status);
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
        m_currentChannelParticipant->setMuted(resp.userId,resp.status);
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
        m_currentChannelParticipant->setDeafened(resp.userId,resp.status);
    }break;

    case PacketType::UserJoinedChannel:
    {
        auto resp =
            PacketHelpers::unpack<UserJoinedChannelPacket>(
                packet.payload);

        //update position of user on channelModel (do it before process, because in code won't see user's change on that channel..
        m_channelModel->moveUser(resp.userId,resp.channelId);


        //if user has no channel add him to channelModel
        if(resp.oldChannelId==-1) //if it's -1 means user just connected to server and didnt join any channel yet. so don't need add him channelModel
        {
            //read user's data from connectedUsersModel
            ConnectedUser* user = m_connectedUsersModel->findUser(resp.userId);

            //add user to channemodel
            m_channelModel->addUser(resp.channelId,user->id,user->username,
                                    user->muted,user->deafened,user->hasVideo);
        }


        if(resp.userId == static_cast<quint64>(myId()))
        {
            qDebug() << "voice: channel switched.";


            //open mic and speaker
            if(m_mic && !m_mic->started()) //check is mic start or not if it's start dont restart it!
                m_mic->start();
            if(m_speaker && !m_speaker->started())  //check is speaker started or not, if it started dont restart.
                m_speaker->start();

            //rest talkin status of all previous channel users, because if dont it would show/stuck user is talkin on previous channel..
            m_channelModel->resetChannelTalkingStatus(resp.oldChannelId);


            //set channel name for Chat and other parts to know current channel name
            m_myChannelId = resp.channelId;
            m_channelModel->setCurrentChannelId(m_myChannelId); //for update isTalking status users
            setMyChannelName(m_channelModel->getChannelName(m_myChannelId)); //to show on top of Chat also on userConnectedServer.


            if(!m_me) //if this user is not inside participantModel, reset model comepletely.
                m_currentChannelParticipant->clear();
            else //otherwise hold user to save camera preview ,moreover less user add user, clear model but hold this user.
                m_currentChannelParticipant->clearExcept(myId()); //to dont ruin camera preview feedback, if clear completely this user camera preview would be black!


            //add each users of this channel of (which user joint) into participantModel
            ChannelItem* channel = m_channelModel->findChannel(resp.channelId);
            if(channel)
            {
                //add found users into participant model
                for(const UserItem& user: channel->users)
                {
                    if(user.id == myId()) //because we didnt completely clear participantmodel, we hold this user.
                    {
                        if(m_me) //is thisUser inside participantModel?
                            continue;
                    }

                    m_currentChannelParticipant->addUser(user.id,user.username,user.isTalking,user.muted,
                                                         user.deafened,user.hasVideo);
                }

                if(!m_me) //user was not inside model therefore it has been added recently so lets set pointer
                    m_me = m_currentChannelParticipant->findUser(myId()); //update user's pointer (uses for camera preview to set this user videoSink to show local preview for camera)
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
                    m_currentChannelParticipant->addUser(jointUser->id,jointUser->username,jointUser->isTalking,jointUser->muted,
                                                         jointUser->deafened,jointUser->hasVideo);
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

            //rest leaved user talkin status
            m_channelModel->setUserTalking(resp.userId,false);

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
                QHostAddress(m_targetIp),
                m_targetUdpPort);
            qDebug() << " just sent a register udp message to server.";
        }

        qDebug() << "LOGIN MESSAGE= " << resp.message;
        break;
    }

    case PacketType::UserConnected:
    {
        qInfo() << "user connected:";
        auto u =
            PacketHelpers::unpack<UserConnectedPacket>(
                packet.payload);

        qDebug()
            << "User joined:"
            << u.username;

        m_connectedUsersModel->addUser(u.id,
                                       u.username,
                                       "",false,//server doesnt send these right now.
                                       u.muted,
                                       u.deafened,
                                       u.camera,
                                       UserActivityStatus::Online);//also server doesnt send this too.
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

        ChannelItem* channel = m_channelModel->findChannelOfUser(user.id);
        if(!channel)
        {
            qDebug() << "invalid hcannel id not found cant find usr was in our channel or not";
        }

        //check whether user was on our channel, if was play sound effect
        if(channel->id == m_myChannelId)
        {
            qDebug() << "play user left.";
            emit userLeft();
        }

        //remove user from connected users list
        m_connectedUsersModel->removeUser(user.id);

        //we dont know if user was inside our channel or not but anyway try to remove him from participantmodel.
        m_currentChannelParticipant->removeUser(user.id);

        //also dont know if user was inside a channel or not anyway try to remove him from model
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

        m_connectedUsersModel->clear();

        for(auto& c : state.channels)
        {
            m_channelModel->addChannel(
                c.id,
                c.name);
        }

        for(auto& u : state.users)
        {
            m_connectedUsersModel->addUser(u.id,
                                           u.username,
                                           "",false,//server doesnt send these right now.
                                           u.muted,
                                           u.deafened,
                                           u.camera,
                                           UserActivityStatus::Online);//also server doesnt send this too.


            //add those users are in channels to our channel model
            if(u.channelId!=-1)
                m_channelModel->addUser(u.channelId,u.id,u.username,u.muted,u.deafened,u.camera);
        }

        qInfo() << "voice: connected to server.";
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
        QHostAddress(m_targetIp),
        m_targetUdpPort);
}

QString User::myServerName() const
{
    return m_myServerName;
}

void User::setMyServerName(const QString &newMyServerName)
{
    if (m_myServerName == newMyServerName)
        return;
    m_myServerName = newMyServerName;
    emit myServerNameChanged();
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
