#include "user.h"

User::User(ChannelModel *channelModel, ChatModel *chatModel,
           ParticipantModel* currentChannelParticipant, ConnectedUsersModel *connectedUsersModel, MyServersModel* myServersModel,
           SoundManager* sounderManager, SettingsManager* settingsManager,
           CameraCapture* cam, AudioCapture *mic, AudioSpeaker* speaker,
           QObject *parent)
    : QObject{parent}, m_channelModel(channelModel), m_chatModel(chatModel),
    m_currentChannelParticipant(currentChannelParticipant), m_connectedUsersModel(connectedUsersModel),
    m_myServersModel(myServersModel),
    m_soundManager(sounderManager), m_settingsManager(settingsManager),
    m_cam(cam), m_mic(mic), m_speaker(speaker)
{
    qDebug() << "user starting..";



    if (!m_opus.initialize(48000, 1, 32000))
    {
        qFatal("Failed to initialize Opus");
    }


    //video decoder
    m_videoDecoder.open();
    connect(
        &m_videoDecoder,
        &FFmpegDecoder::imageReady,
        this,
        [this](const QImage &image)
        {
            Participant *sender =
                m_currentChannelParticipant->findUser(m_lastVideoSender);

            if(!sender)
                return;

            VideoSink *sink = sender->videoSink();

            if(!sink)
                return;

            sender->setIsCameraOpen(true);

            sink->setImage(image);
        });

    initOrLoadSettings();



    //setup database stuff
    m_database.createTable(R"(
        CREATE TABLE IF NOT EXISTS MyServers
        (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            ip TEXT,
            port TEXT
        )
        )");



    //load all saved servers from database.
    qDebug() << "loading all saved servers.";
    QVariantList servers = m_database.getAll("MyServers");
    for (const QVariant &v : servers)
    {
        QVariantMap row = v.toMap();

        qDebug()
            << row["id"]
            << row["name"]
            << row["ip"]
            << row["port"];

        //add to myServers model
        m_myServersModel->addServer(row["name"].toString(),
                                    row["ip"].toString(),
                                    row["port"].toString(),
                                    false, //set IsActive FALSE
                                    row["id"].toUInt()); //set server index
    }




    //setup TCP socket

    connect(&socket,
            &QTcpSocket::readyRead,
            this,
            &User::onTcpReadyRead);

    connect(&socket, &QTcpSocket::disconnected,
            this, &User::onDisconnected);

    connect(&socket, &QTcpSocket::errorOccurred,
            this, &User::onSocketError);


    //setup UDP socket
    m_udpSocket.bind();

    connect(&m_udpSocket,
            &QUdpSocket::readyRead,
            this,
            &User::onUdpReadyRead);

    //when request register sent to to udp server, would expect ping request evey xSeconds from server, whenever didn't receive any assuming server is down or connection is lost.
    m_udpConnectionTimeout.setSingleShot(true);

    connect(&m_udpConnectionTimeout, &QTimer::timeout,
            this, [&]()
            {
                qDebug() << "server didn't send ping request for xSeconds so UDP connection has lost.";

                disconnect(); //make sure tcp disconnects and ui show disconnected elemnts
            });



    /*
     * make a connection between channelmodel  to participantModel (center of screen [those rectangles]) sync with channelModel user's talking status
     * channelModel -> (which contineusly check users of current channel to find out who stopped talking due to last talking time)
    */
    connect(m_channelModel, &ChannelModel::userTalkingStatus,
            m_currentChannelParticipant, &ParticipantModel::setTalking);


    //get system info to send to server on login stage.
    m_info.appVersion = QCoreApplication::applicationVersion();
    #ifdef QT_NO_DEBUG
        m_info.buildType = "Release";
    #else
        m_info.buildType = "Debug";
    #endif
    m_info.osName = platformName();
    m_info.osVersion = QSysInfo::prettyProductName();
    m_info.machineName = QSysInfo::machineHostName();
    m_info.machineId = QString(QSysInfo::machineUniqueId().toHex());
}


void User::joinChannel(int channelId, const QString& password)
{
    JoinChannelPacket join;

    join.channelId = channelId;
    join.password =  password;

    Packet p;

    p.type = PacketType::JoinChannel;
    p.payload = PacketHelpers::pack(join);

    socket.write(p.serialize());
}

void User::connectToServer(bool saveThisConnection, const QString& serverIp, const QString& str_serverPort)
{
    //if user is connected to somewhere, disconnect before new connection
    if(isConnectedToServer())
        disconnect();


    //convert ports to quint64
    bool ok = false;
    quint64 serverPort = str_serverPort.toULongLong(&ok);
    if(!ok)
    {
        qDebug() << "Invalid port number!";
        return;
    }


    //validate entered ip and ports
    //code here


    //save this server if was not in myServers
    int serverId = m_myServersModel->doesServerExists(serverIp, str_serverPort);

    if(serverId==-1) //server doesnt exist on list
    {
        QString serverName= "The Server";
        int serverDbIndex = -1;
        if(saveThisConnection)
        {
            //save server into local storage
            bool result = m_database.insert("MyServers",
                      {
                          {"name", serverName},
                          {"ip", serverIp},
                          {"port", str_serverPort}
                      });
            if(result)
            {
                qDebug() << "server saved to myServers";
                QVariantMap serverInfo = m_database.getServer(serverIp,str_serverPort);

                if (serverInfo.isEmpty())
                {
                    qDebug() << "while reading data from recently added server got: Server not found";
                    return;
                }

                serverDbIndex = serverInfo["id"].toInt();
                serverName = serverInfo["name"].toString();
                setMyServerName(serverName);
            }
            else
                qDebug() << "failed to save server to myServers.";
        }

        //add to myServers model and set isActive to TRUE
        m_myServersModel->addServer(serverName, serverIp, str_serverPort,true,serverDbIndex);
    }
    else //server exists, so just set server active
        m_myServersModel->setIsActive(serverId);



    //store in variables for different parts of app
    m_serverIp=serverIp;
    m_serverPort=serverPort;

    //connect to TCP
    socket.connectToHost(
        m_serverIp,
        m_serverPort);


    //update servername for QML
    if(serverId!=-1) //server exists just try to read server name from myServers table. otherwise when adding server would setServerName.
    {
        QVariantMap serverInfo = m_database.getServer(serverIp,str_serverPort);
        if (!serverInfo.isEmpty())
            setMyServerName(serverInfo["name"].toString());
        else
            setMyServerName("The Server");
    }

    LoginRequestPacket login;

    if(m_myUsername=="")
    {
        //do a default and random name..
        setMyUsername("BeanUser"+QString::number(QRandomGenerator::global()->bounded(100)));
    }

    setIsConnectedToServer(true);
    login.username = myUsername();
    login.identity = myIdentity();
    //system info.
    login.appVersion = m_info.appVersion;
    login.buildType = m_info.buildType;
    login.machineId = m_info.machineId;
    login.machineName = m_info.machineName;
    login.osName = m_info.osName;
    login.osVersion = m_info.osVersion;

    Packet p;
    p.type = PacketType::LoginRequest;
    p.payload = PacketHelpers::pack(login);

    qDebug() << "sending login request.. will wait for response.. connecting server is "
             << m_serverIp << ":" << m_serverPort  << " name=" << myUsername() << "identity=" << myIdentity() ;

    socket.write(p.serialize());


    //reset flag for next use.
    m_switchingServer=false;
}

void User::updateSavedServer(quint64 serverId, quint64 dbIndex, const QString& name, const QString& ip, const QString& port)
{
    bool res = m_database.update("MyServers",
                    dbIndex,
                    {
                        {"name", name},
                        {"ip", ip},
                        {"port", port.toInt()}
                    });

    if(res)
    {
        qDebug() << "success to update saved server.";

        //update model data.
        m_myServersModel->updateServer(serverId,name,ip,port);
        
        //update servername on local variable too
        setMyServerName(name);
    }
    else
        qDebug() << "failed to update saved server.";
}

void User::deleteSavedServer(quint64 serverId, quint64 serverDbIndex)
{
    if(serverDbIndex!=-1) //server is not saved in database. just delete it from model.
    {
        bool res = m_database.remove("MyServers",serverDbIndex);
        if(res)
            qDebug() << "server deleted from MyServers.";
        else
            qDebug() << "faield to delete server from MyServers.";
    }

    //anyway delete from model
    m_myServersModel->removeServer(serverId);
}

void User::switchOrConnectToServer(const QString &serverIp, const QString &str_serverPort, int serverId)
{
    qDebug() << "connectToServer server id, called ";
    //tell myServers model im connected to this server.
    m_myServersModel->setIsActive(serverId);

    m_switchingServer=true;

    //do normal connectToServer things
    connectToServer(false, serverIp,str_serverPort);
}

void User::disconnect()
{
    if(!isConnectedToServer()) //to prevent double run this function, first user do disconnect manually/switched to antoher server, then QTCPSocket::Disconnect would run this again..
        return;


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
    setIsConnectedToServer(false);
    m_me=nullptr;
    m_serverIp.clear();
    m_serverPort=0;

    if(!m_switchingServer) //if we are not switching reset/turn-off all server's indicator status
        m_myServersModel->resetPreviousIsActiveServer();


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
    if(!isConnectedToServer())
        return;

    if(muteMicrophone() || muteHeadphone())
    {
        // qDebug() << "mic or headphone is muted.";
        return;
    }

    if(m_myId < 0)
    {
        // qDebug() << "invalid my id.";
        return;
    }



    // Accumulate microphone PCM
    m_sendPcmBuffer.append(pcm);

    bool sentPacket = false;


    constexpr int FRAME_BYTES = 960 * sizeof(qint16);

    // Encode every complete 20ms frame
    while (m_sendPcmBuffer.size() >= FRAME_BYTES)
    {
        QByteArray frame = m_sendPcmBuffer.left(FRAME_BYTES);
        m_sendPcmBuffer.remove(0, FRAME_BYTES);

        VoicePacket voice;

        voice.senderId =
            static_cast<quint64>(myId());

        voice.sequence =
            ++m_sequence;

        voice.audioData = m_opus.encode(frame);

        if (voice.audioData.isEmpty())
            continue;

        qDebug() << "sending Opus:" << voice.audioData.size()
                 << "frame:" << frame.size()
                 << "pcm raw " << pcm.size();

        QByteArray data;

        QDataStream out(
            &data,
            QIODevice::WriteOnly);

        out << quint16(101);
        out << voice;

        m_udpSocket.writeDatagram(
            data,
            QHostAddress(m_serverIp),
            m_serverPort + 1);

        sentPacket = true;
    }


    if (!sentPacket)
        return;

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
    switch(packet.type)
    {

    case PacketType::ChannelCreated:
    {
        qDebug() <<"a channel created";
        auto resp =
            PacketHelpers::unpack<ChannelCreatedPacket>(
                packet.payload);

        m_channelModel->addChannel(resp.id,resp.name,resp.isLocked);
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
            setMyId(resp.id); //server just told us our name, to know when e.g: user connected to that channel is that same channel as us? what is my id? so here is it.
            qDebug() << "my id is=" << myId();


            //----------------udp voice. video login
            UdpRegisterPacket reg;

            reg.userId = static_cast<quint64>(myId());

            QByteArray data;

            QDataStream out(
                &data,
                QIODevice::WriteOnly);

            //for now we dont login for UDP later need token/identity and .. for security
            //code here

            out << quint16(100); //#100 is known as register on server.
            out << reg;



            //start to expect every xSeconds ping request from server otherwise, assuming UDP connection has failed
            m_udpConnectionTimeout.start(10000); // 10 seconds


            m_udpSocket.writeDatagram(
                data,
                QHostAddress(m_serverIp),
                m_serverPort+1);
            qDebug() << " just sent a register udp message to server.";


            //ask for server channels, users, ...
            askForServerState();
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
                                       u.appVersion, u.buildType, u.osName, u.osVersion,
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
        if(channel)
        {
            //check whether user was on our channel, if was play sound effect
            if(channel->id == m_myChannelId)
            {
                qDebug() << "play user left.";
                emit userLeft();
            }
        }
        else
            qDebug() << "invalid channel id, not found cant find usr was in our channel or not";


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

        //show notification dot near chat indicator when chat isn't open
        if(!m_isChatOpen)
            setChatUnreadMessages(chatUnreadMessages()+1); //increase unread messages count

        if(msg.senderId==myId())
            emit messageSent(); //play message sent effect.
        else
            emit newMessage(); //play message recevied effect.

        m_chatModel->addMessage(msg);
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
                c.name,
                c.isLocked);
        }

        for(auto& u : state.users)
        {
            qDebug() << "add user to connected: " <<  u.appVersion << "-" << u.buildType << "-" << u.osName << "-" << u.osVersion;
            m_connectedUsersModel->addUser(u.id,
                                           u.username,
                                           "",false,//server doesnt send these right now.
                                           u.muted,
                                           u.deafened,
                                           u.camera,
                                           u.appVersion, u.buildType, u.osName, u.osVersion,
                                           UserActivityStatus::Online);//also server doesnt send this too.


            //add those users are in channels to our channel model
            if(u.channelId!=-1)
                m_channelModel->addUser(u.channelId,u.id,u.username,u.muted,u.deafened,u.camera);
        }

        qInfo() << "voice: connected to server.";
    }
    break;

    default:
        qDebug() << "an unknown packetType received. type=" << static_cast<int>(packet.type);
        break;
    }
}

void User::onDisconnected()
{
    qDebug() << "Server disconnected";
    disconnect();
}

void User::onSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << "TCP error:" << error
             << socket.errorString();
    disconnect();
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

        switch(type)
        {
        case 101:  //voice
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


                //decode
                QByteArray pcm = m_opus.decode(packet.audioData);
                qDebug() << "received opus=" << pcm.size() << " raw pcm=" << packet.audioData.size();
                if (!pcm.isEmpty())
                    emit voiceReceived(pcm);
            }

            // qDebug()
            //     << "Voice received from"
            //     << packet.senderId
            //     << "seq"
            //     << packet.sequence
            //     << "size"
            //     << packet.audioData.size();
            break;
        }

        case 102: //video
        {
            VideoPacket packet;

            in >> packet;

            // qDebug()
            //     << "Video received from "
            //     << packet.senderId
            //     << "seq="
            //     << packet.sequence
            //     << "size="
            //     << packet.videoData.size();

            Participant* senderParticipant = m_currentChannelParticipant->findUser(packet.senderId);
            if(!senderParticipant)
            {
                qDebug() << "could not find sender as participant..";
                continue;
            }

            //decode.
            m_lastVideoSender = packet.senderId;
            m_videoDecoder.decode(packet.videoData);

            // emit videoReceived(packet.videoData);
            break;
        }

        case 103: //ping requst from server.
            PingPacket p;
            in >> p;

            //set variables to show in QML component.
            setMyPing(p.lastPing);
            setMyVoicePacketLoss(p.voicePacketLoss);
            setMyVideoPacketLoss(p.videoPacketLoss);


            //a udp request-ping received, now reset connection timeout to later know is udp connection still alive or not
            m_udpConnectionTimeout.start(10000);


            //send back same sequence..
            QByteArray data2;

            QDataStream out(
                &data2,
                QIODevice::WriteOnly);

            out << quint16(104);//response ping
            out << p;

            m_udpSocket.writeDatagram(
                data2,
                QHostAddress(m_serverIp),
                m_serverPort+1);
            break;
        }        
    }
}

void User::sendVideoFrame(const QByteArray &videoData)
{
    qDebug() << "sendVideoFrame:" << videoData.size();

    if(!isConnectedToServer())
        return;

    if(!isCameraOpen())
        return;

    if(myId() < 0)
        return;

    VideoPacket videoPacket;

    videoPacket.senderId = static_cast<quint64>(myId());

    videoPacket.sequence = ++m_videoSequence;

    videoPacket.videoData = videoData;

    QByteArray data;

    QDataStream out(
        &data,
        QIODevice::WriteOnly);

    out << quint16(102);
    out << videoPacket;

    qDebug() << "Sending Video JPEG bytes:" << videoData.size();
    qDebug() << "Video UDP datagram bytes:" << data.size();

    qDebug()
        << "VIDEO DEST:"
        << QHostAddress(m_serverIp)
        << m_serverPort + 1;

    qint64 sent = m_udpSocket.writeDatagram(
        data,
        QHostAddress(m_serverIp),
        m_serverPort+1);

    qDebug() << "writeDatagram returned:" << sent;

    if (sent == -1)
    {
        qDebug() << "Error:"
                 << m_udpSocket.error()
                 << m_udpSocket.errorString();
    }
}


QString User::platformName()
{
    #ifdef Q_OS_ANDROID
        return "Android";
    #elif defined(Q_OS_WIN)
        return "Windows";
    #elif defined(Q_OS_IOS)
            return "IOS";
    #elif defined(Q_OS_MACOS)
        return "MacOS";
    #elif defined(Q_OS_LINUX)
        return "Linux";
#elif
    #else
        return "Unknown";
    #endif
}

float User::myVideoPacketLoss() const
{
    return m_myVideoPacketLoss;
}

void User::setMyVideoPacketLoss(float newMyVideoPacketLoss)
{
    if (qFuzzyCompare(m_myVideoPacketLoss, newMyVideoPacketLoss))
        return;
    m_myVideoPacketLoss = newMyVideoPacketLoss;
    emit myVideoPacketLossChanged();
}

void User::initOrLoadSettings()
{
    //load all saved settings such as saved devices index for audio input/output, volumes and ... and set to variables


    // =================================== read and set audioinput settings
    if(m_settingsManager->contains(MIC_SETTING_RNN_STATUS))
        m_mic->setRnnoiseStatus(m_settingsManager->value(MIC_SETTING_RNN_STATUS, MIC_DEFAULT_RNNOISE_STATUS).toBool());
    else //set default setting.
    {
        m_settingsManager->setValue(MIC_SETTING_RNN_STATUS, MIC_DEFAULT_RNNOISE_STATUS);
        m_mic->setRnnoiseStatus(MIC_DEFAULT_RNNOISE_STATUS);
    }


    if(m_settingsManager->contains(MIC_SETTING_RNN_VALUE))
        m_mic->setRnnoiseValue(m_settingsManager->value(MIC_SETTING_RNN_VALUE, MIC_DEFAULT_RNNOISE_VALUE).toFloat());
    else //set default setting.
    {
        m_settingsManager->setValue(MIC_SETTING_RNN_VALUE, MIC_DEFAULT_RNNOISE_VALUE);
        m_mic->setRnnoiseValue(MIC_DEFAULT_RNNOISE_VALUE);
    }


    if(m_settingsManager->contains(MIC_SETTING_VOLUMEGATE))
        m_mic->setVolumeGateStatus(m_settingsManager->value(MIC_SETTING_VOLUMEGATE, MIC_DEFAULT_VOLUMEGATE_STATUS).toBool());
    else //set default setting.
    {
        m_settingsManager->setValue(MIC_SETTING_VOLUMEGATE, MIC_DEFAULT_VOLUMEGATE_STATUS);
        m_mic->setVolumeGateStatus(MIC_DEFAULT_VOLUMEGATE_STATUS);
    }


    if(m_settingsManager->contains(MIC_SETTING_VOLUMEGATE_THRESHOLD))
        m_mic->setVolumeGateThreshold(m_settingsManager->value(MIC_SETTING_VOLUMEGATE_THRESHOLD, MIC_DEFAULT_VOLUMEGATE_THRESHOLD).toFloat());
    else //set default setting.
    {
        m_settingsManager->setValue(MIC_SETTING_VOLUMEGATE_THRESHOLD, MIC_DEFAULT_VOLUMEGATE_THRESHOLD);
        m_mic->setVolumeGateThreshold(MIC_DEFAULT_VOLUMEGATE_THRESHOLD);
    }


    if(m_settingsManager->contains(MIC_SETTING_PTT_STATUS))
        m_mic->setPushToTalkStatus(m_settingsManager->value(MIC_SETTING_PTT_STATUS, MIC_DEFAULT_PTT_STATUS).toBool());
    else //set default setting.
    {
        m_settingsManager->setValue(MIC_SETTING_PTT_STATUS, MIC_DEFAULT_PTT_STATUS);
        m_mic->setPushToTalkStatus(MIC_DEFAULT_PTT_STATUS);
    }


    if(m_settingsManager->contains(MIC_SETTING_PTT_HOTKEY))
        m_mic->setPushToTalkKey(m_settingsManager->value(MIC_SETTING_PTT_HOTKEY, MIC_DEFAULT_PTT_HOTKEY).toInt());
    else //set default setting.
    {
        m_settingsManager->setValue(MIC_SETTING_PTT_HOTKEY, MIC_DEFAULT_PTT_HOTKEY);
        m_mic->setPushToTalkKey(MIC_DEFAULT_PTT_HOTKEY);
    }



    //check for input device exists or not
    if(m_mic->devicesCount() > 0)
    {
        //read from saved settings and convert to valid id,
        int micIndex = m_mic->audioInputIndexFromId(m_settingsManager->value(MIC_SETTING_DEVICE, "").toString());
        if(micIndex != -1)
            m_mic->setCurrentAudioInput(micIndex); //m_soundManager->outputDevice would automatically obey this change
        else
        {
            qDebug() << "saved audioInput device not found, we did reset it to default.";

            //set to default (first device)
            QString deviceId = m_mic->audioInputId(MIC_DEFUALT_DEVICE_INDEX);
            m_mic->setCurrentAudioInput(m_mic->audioInputIndexFromId(deviceId));
            m_settingsManager->setValue(MIC_SETTING_DEVICE, deviceId);
        }
    }
    else
    {
        m_settingsManager->setValue(MIC_SETTING_DEVICE, "");
        qDebug() << "ERROR there is no microphone at all.. plug something in";
    }


    // =================================== read and set audiooutput settings

    if(m_settingsManager->contains(SPEAKER_SETTING_EFFECTS_VOLUME))
        m_soundManager->setVolume(m_settingsManager->value(SPEAKER_SETTING_EFFECTS_VOLUME, SPEAKER_DEFAULT_EFFECTS_VOLUME).toFloat());
    else //set default setting.
    {
        m_settingsManager->setValue(SPEAKER_SETTING_EFFECTS_VOLUME, SPEAKER_DEFAULT_EFFECTS_VOLUME);
        m_soundManager->setVolume(SPEAKER_DEFAULT_EFFECTS_VOLUME);
    }


    //check for output device exists or not
    if(m_speaker->devicesCount() > 0)
    {
        //read from saved settings and convert to valid id,
        int speakerIndex = m_speaker->audioOutputIndexFromId(m_settingsManager->value(SPEAKER_SETTING_DEVICE, "").toString());
        if(speakerIndex != -1)
            m_speaker->setCurrentAudioOutput(speakerIndex); //m_soundManager->outputDevice would automatically obey this change
        else
        {
            qDebug() << "saved audioOutput device not found, we did reset it to default.";
            m_speaker->setCurrentAudioOutput(SPEAKER_DEFAULT_DEVICE_INDEX); //set to default, note m_soundManager->outputDevice would automatically obey this change

            //set to default (first device)
            QString deviceId = m_speaker->audioOutputId(SPEAKER_DEFAULT_DEVICE_INDEX);
            m_speaker->setCurrentAudioOutput(m_speaker->audioOutputIndexFromId(deviceId));
            m_settingsManager->setValue(SPEAKER_SETTING_DEVICE, deviceId);
        }
    }
    else
    {
        m_settingsManager->setValue(SPEAKER_SETTING_DEVICE, "");
        qDebug() << "ERROR there is no speaker at all.. plug something in";
    }


    // =================================== read and set video settings
    //check for output device exists or not
    if(m_cam->devicesCount() > 0)
    {
        //read from saved settings and convert to valid id,
        int cameraIndex = m_cam->cameraInputIndexFromId(m_settingsManager->value(CAMERA_SETTING_DEVICE, "").toString());
        if(cameraIndex != -1)
            m_cam->setCurrentCameraInput(cameraIndex);
        else
        {
            qDebug() << "saved cameraInput device not found, we did reset it to default.";

            //set to default (first device)
            QString deviceId = m_cam->cameraIntputId(CAMERA_DEFAULT_DEVICE_INDEX);
            m_cam->setCurrentCameraInput(m_cam->cameraInputIndexFromId(deviceId));
            m_settingsManager->setValue(CAMERA_SETTING_DEVICE, deviceId);
        }
    }
    else
    {
        m_settingsManager->setValue(CAMERA_SETTING_DEVICE, "");
        qDebug() << "ERROR there is no cameraInput at all.. plug something in";
    }



}

QString User::myAppVersion() const
{
    return m_info.appVersion + " - " + m_info.buildType;
}

float User::myVoicePacketLoss() const
{
    return m_myVoicePacketLoss;
}

void User::setMyVoicePacketLoss(float newMyVoicePacketLoss)
{
    if (qFuzzyCompare(m_myVoicePacketLoss, newMyVoicePacketLoss))
        return;
    m_myVoicePacketLoss = newMyVoicePacketLoss;
    emit myVoicePacketLossChanged();
}

int User::myPing() const
{
    return m_myPing;
}

void User::setMyPing(int newMyPing)
{
    if (m_myPing == newMyPing)
        return;
    m_myPing = newMyPing;
    emit myPingChanged();
}

int User::chatUnreadMessages() const
{
    return m_chatUnreadMessages;
}

void User::setChatUnreadMessages(int newChatUnreadMessages)
{
    if (m_chatUnreadMessages == newChatUnreadMessages)
        return;
    m_chatUnreadMessages = newChatUnreadMessages;
    emit chatUnreadMessagesChanged();
}

bool User::isChatOpen() const
{
    return m_isChatOpen;
}

void User::setIsChatOpen(bool newIsChatOpen)
{
    if (m_isChatOpen == newIsChatOpen)
        return;
    m_isChatOpen = newIsChatOpen;
    emit isChatOpenChanged();
}

QString User::myIdentity() const
{
    return m_myIdentity;
}

void User::setMyIdentity(const QString &newIdentity)
{
    if (m_myIdentity == newIdentity)
        return;
    m_myIdentity = newIdentity;
    emit myIdentityChanged();
}

bool User::isConnectedToServer() const
{
    return m_isConnectedToServer;
}

void User::setIsConnectedToServer(bool newConnectedToServer)
{
    if (m_isConnectedToServer == newConnectedToServer)
        return;
    m_isConnectedToServer = newConnectedToServer;
    emit isConnectedToServerChanged();
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

