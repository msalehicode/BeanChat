#include "user.h"

User::User(ChannelModel *channelModel, ChatModel *chatModel,
           ParticipantModel* currentChannelParticipant, ConnectedUsersModel *connectedUsersModel, MyServersModel* myServersModel,
           SoundManager* sounderManager, SettingsManager* settingsManager,
           ClientUserManager *clientuserManager, IdentityManager *identityManager,
           RelationshipManager* relationshipManager, Database* database,
           CameraCapture* cam, AudioCapture *mic, AudioSpeaker* speaker,
           QObject *parent)
    : QObject{parent}, m_channelModel(channelModel), m_chatModel(chatModel),
    m_currentChannelParticipant(currentChannelParticipant), m_connectedUsersModel(connectedUsersModel),
    m_myServersModel(myServersModel),
    m_soundManager(sounderManager), m_settingsManager(settingsManager),
    m_clientUserManager(clientuserManager), m_identityManager(identityManager),
    m_relationshipManager(relationshipManager), m_database(database),
    m_cam(cam), m_mic(mic), m_speaker(speaker)
{
    qInfo() << "using BeanChatCommon version " << BeanChatCommon::Protocol::Version;


    //read or generate identitiy if not exsit
    if(!m_identityManager->load())
    {
        qFatal() << "failed to load/generate identity.";
    }

    qDebug() << "Loaded identities count= " << m_identityManager->identities().count();
    qDebug() << "found identities:";
    for(auto& identity : m_identityManager->identities())
    {
        qDebug() << "name= " <<identity.name  << " pub:" <<identity.publicKeyBase64() << " priv:" << identity.privateKeyBase64() << " craeted at=" <<identity.createdAt;
    }
    //notify QML identity loaded/changed
    emit myIdentityChanged();




    qDebug() << "user starting..";



    if (!m_opus.initialize(OPUS_DEFAULT_SAMPLE_RATE,
                           OPUS_DEFAULT_CHANNELS,
                           OPUS_DEFAULT_BITRATE))
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
            if(m_decodeQueue.isEmpty())
                return;

            quint64 senderId = m_decodeQueue.dequeue();

            ClientUser *sender = m_currentChannelParticipant->findUser(senderId);

            if(!sender)
                return;

            VideoSink *sink = m_currentChannelParticipant->videoSink(sender->id());

            if(!sink)
                return;

            sender->setHasCamera(true);

            sink->setImage(image);
        });

    initOrLoadSettings();



    //setup database stuff
    m_database->createTable(R"(
        CREATE TABLE IF NOT EXISTS MyServers
        (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            avatarPath TEXT,
            ip TEXT,
            port TEXT
        )
        )");


    m_database->createTable(R"(
        CREATE TABLE IF NOT EXISTS UserRelations
        (
            identity TEXT PRIMARY KEY,

            nickname TEXT,
            note TEXT,

            relationship INTEGER NOT NULL DEFAULT 0,

            muted INTEGER NOT NULL DEFAULT 0,
            voiceVolume INTEGER NOT NULL DEFAULT 100,
            createdAt DATETIME DEFAULT CURRENT_TIMESTAMP
        );
        )");


    //load all saved servers from database.
    qDebug() << "loading all saved servers.";
    QVariantList servers = m_database->getAll("MyServers");
    for (const QVariant &v : servers)
    {
        QVariantMap row = v.toMap();

        qDebug()
            << row["id"]
            << row["name"]
            << row["avatarPath"]
            << row["ip"]
            << row["port"];

        //add to myServers model
        m_myServersModel->addServer(row["name"].toString(),
                                    row["avatarPath"].toString(),
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
                if(isConnectedToServer())
                {
                    qDebug() << "server didn't send ping request for a while, so UDP connection has lost.";
                    emit notificationRequested(NotificationType::Error,
                                               "Connection Lost",
                                               NotificationId::ConnectionLost,
                                               NotificationDuration::Long);
                }

                //close connection, sometimes user may stuck in middle of connecting and connection lost
                //so here we make sure close connection even we dont show connection lost messag to them
                disconnect(); //make sure tcp disconnects and ui show disconnected elemnts
            });



    /*
     * make a connection between channelmodel  to participantModel (center of screen [those rectangles]) sync with channelModel user's talking status
     * channelModel -> (which contineusly check users of current channel to find out who stopped talking due to last talking time)
    */
    connect(m_channelModel,
            &ChannelModel::userTalkingStatus,
            this,
            [this](quint64 userId, bool talking)
            {
                if (ClientUser *user = m_clientUserManager->user(userId))
                    user->setIsTalking(talking);
            });
}


void User::joinChannel(quint64 channelId, const QString& password)
{
    JoinChannelPacket join;

    join.channelId = channelId;
    join.password =  password;

    Packet p;

    p.type = PacketType::JoinChannel;
    p.payload = PacketHelpers::pack(join);

    socket.write(p.serialize());
}

int User::isChannelLocked(quint64 channelId)
{
    ChannelItem* channel = m_channelModel->findChannel(channelId);
    if(channel)
        return channel->isLocked;
    else
        qDebug() << "invalid channel id.";
    return -1;
}

QString User::serverName() const
{
    return m_receivedServerInfo.name;
}

QString User::serverWebsite() const
{
    return m_receivedServerInfo.website;
}

QString User::serverAvatarHash() const
{
    return m_receivedServerInfo.avatarHash;
}

QString User::serverVersion() const
{
    return m_receivedServerInfo.version;
}

QString User::serverUptime() const
{
    qint64 seconds = m_receivedServerInfo.startTime.secsTo(QDateTime::currentDateTimeUtc());

    qint64 days = seconds / 86400;
    seconds %= 86400;

    if (days > 0)
        return QString("%1 day%2").arg(days).arg(days == 1 ? "" : "s");

    qint64 hours = seconds / 3600;
    seconds %= 3600;

    if (hours > 0)
        return QString("%1 hour%2").arg(hours).arg(hours == 1 ? "" : "s");

    qint64 minutes = seconds / 60;
    seconds %= 60;

    if (minutes > 0)
        return QString("%1 minute%2").arg(minutes).arg(minutes == 1 ? "" : "s");

    if (seconds > 0)
        return QString("%1 second%2").arg(seconds).arg(seconds == 1 ? "" : "s");

    return QString();
}

void User::moveUser(quint64 userId, quint64 channelId, const QString& password)
{
    MoveUserPacket mv;
    mv.channelId=channelId;
    mv.userId=userId;
    mv.channelPassword=password;

    Packet p;

    p.type = PacketType::MoveUser;
    p.payload = PacketHelpers::pack(mv);

    socket.write(p.serialize());
}

void User::connectToServer(bool saveThisConnection, const QString& serverIp, const QString& str_serverPort)
{
    //if user is connected to somewhere, disconnect before new connection
    if(isConnectedToServer())
        disconnect();


    //check is server saved or is temporary?
    if(m_connectedServerId_onDb==-1) //server is temporary
    {
        qDebug() << "server connection is termporary. connectedServerId DB=" << m_connectedServerId_onDb;
    }

    //convert ports to quint64
    bool ok = false;
    quint64 serverPort = str_serverPort.toULongLong(&ok);
    if(!ok)
    {
        qDebug() << "Invalid port number!";
        emit notificationRequested(NotificationType::Error,
                                   "Invalid port number.");
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
            bool result = m_database->insert("MyServers",
                      {
                          {"name", serverName},
                          {"ip", serverIp},
                          {"port", str_serverPort}
                      });
            if(result)
            {
                qDebug() << "server saved to myServers";
                QVariantMap serverInfo = m_database->getServer(serverIp,str_serverPort);

                if (serverInfo.isEmpty())
                {
                    qDebug() << "while reading data from recently added server got: Server not found";
                    return;
                }

                serverDbIndex = serverInfo["id"].toInt();
                m_connectedServerId_onDb =serverDbIndex;
                serverName = serverInfo["name"].toString();
                setMyServerName(serverName);
            }
            else
                qDebug() << "failed to save server to myServers.";
        }

        //add to myServers model and set isActive to TRUE
        m_myServersModel->addServer(serverName,
                                    "", //avatarPath, we haven't server's avatar
                                    serverIp,
                                    str_serverPort,
                                    true, //is Active
                                    serverDbIndex);
    }
    else //server exists, so just set server active
        m_myServersModel->setIsActive(serverId);




    //update servername for QML
    if(serverId!=-1) //server exists just try to read server name from myServers table. otherwise when adding server would setServerName.
    {
        QVariantMap serverInfo = m_database->getServer(serverIp,str_serverPort);
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


    login.username = myUsername();
    if(!m_identityManager->currentIdentity())
    {
        qDebug() << "connect failed. no identity selected... create one";
        if(m_identityManager->createIdentity("Default"+QString::number(QRandomGenerator::global()->bounded(100))))
        {
            qDebug()<<"we create one new identity for you";
        }
        else
        {
            qDebug() << "you didnt have an identity and sadly we couldn't create one for you!";
            return;
        }
    }
    login.publicKey = m_identityManager->currentIdentity()->publicKey;

    //system info.
    login.appVersion = myAppVersion();
    login.appProtocolVersion = BeanChatCommon::Protocol::Version;
    login.buildType = buildType();
    login.machineId = QString(QSysInfo::machineUniqueId().toHex());
    login.machineName = QSysInfo::machineHostName();
    login.osName =  platformName();
    login.osVersion = QSysInfo::prettyProductName();

    Packet p;
    p.type = PacketType::LoginRequest;
    p.payload = PacketHelpers::pack(login);

    //store in variables for different parts of app
    m_serverIp=serverIp;
    m_serverPort=serverPort;

    //connect to TCP
    socket.connectToHost(
        m_serverIp,
        m_serverPort);

    setIsConnectedToServer(true);


    qDebug() << "sending login request.. will wait for response.. connecting server is "
             << m_serverIp << ":" << m_serverPort  << " name=" << myUsername() << "identity=" << myIdentity() ;

    socket.write(p.serialize());


    //reset flag for next use.
    m_switchingServer=false;
}

void User::updateSavedServer(quint64 serverId, quint64 dbIndex, const QString& name, const QString& ip, const QString& port)
{
    bool res = m_database->update("MyServers",
                    dbIndex,
                    {
                        {"name", name},
                        {"ip", ip},
                        {"port", port.toInt()}
                    });

    if(res)
    {
        emit notificationRequested(NotificationType::Success,
                                   "MyServer updated.");

        //update model data.
        m_myServersModel->updateServer(serverId,name,ip,port);

        //update servername on local variable too
        setMyServerName(name);
    }
    else
    {
        emit notificationRequested(NotificationType::Error,
                                   "Failed to update MyServer.");
    }
}

void User::deleteSavedServer(quint64 serverId, quint64 serverDbIndex)
{
    if(serverDbIndex!=-1) //server is not saved in database. just delete it from model.
    {
        bool res = m_database->remove("MyServers",serverDbIndex);
        if(res)
        {
            qDebug() << "server deleted from MyServers.";
            emit notificationRequested(NotificationType::Success,
                                       "Server deleted from MyServers.");

            //delete saved avatars in that server's avatar directory
            qDebug() << "trying to delete avatars of that server: target path = " << SAVE_AVATAR_PATH+QString::number(serverDbIndex);

            QDir dir(SAVE_AVATAR_PATH+QString::number(serverDbIndex));
            if (dir.exists())
            {
                if (!dir.removeRecursively())
                    qDebug() << "Failed to remove avatar.";
            }
            else
                qDebug() << "that path avatar doesn't exists.";
        }
        else
        {
            emit notificationRequested(NotificationType::Error,
                                       "Failed to delete server from MyServers.");
        }
    }

    //anyway delete from model
    m_myServersModel->removeServer(serverId);
}

void User::switchOrConnectToServer(const QString &serverIp, const QString &str_serverPort, int serverId)
{
    qDebug() << "connectToServer server " << serverId << " called ";
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

    emit notificationRequested(NotificationType::Error,
                               "Disconnected",
                               NotificationId::Disconnected,
                               NotificationDuration::Short);

    //disocnnect sockets.
    socket.disconnectFromHost();
    m_udpSocket.disconnectFromHost();

    resetVariables();

}

void User::createChannel(QString channelName, QString password, bool saveMessages)
{
    CreateChannelPacket cc;
    cc.name = channelName;
    cc.password= password;
    cc.saveChats=saveMessages;

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
#if D_PRINT_VOICE_INFO
        qDebug() << "mic or headphone is muted.";
#endif
        return;
    }

    if(m_myId < 0)
    {
#if D_PRINT_VOICE_INFO
        qDebug() << "invalid my id.";
#endif
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

#if D_PRINT_VOICE_INFO
        qDebug() << "sending Opus:" << voice.audioData.size()
                 << "frame:" << frame.size()
                 << "pcm raw " << pcm.size();
#endif

        QByteArray data;

        QDataStream out(
            &data,
            QIODevice::WriteOnly);

        out << PacketType::UdpVoiceData;
        out << voice;

        m_udpSocket.writeDatagram(
            data,
            m_serverLookedupAddress,
            m_serverPort);

        sentPacket = true;
    }


    if (!sentPacket)
        return;

    //update isTalking ourself
    ClientUser *senderUser = m_channelModel->getUser(m_myChannelId, myId());
    if(!senderUser)
        return;

    if (!senderUser->isTalking())
        senderUser->setIsTalking(true);
    m_channelModel->restartVoiceTimer(myId());
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

void User::updateChannel(quint64 channelId, const QString &name, const QString &pass, bool saveMessages)
{
    UpdateChannelPacket uc;
    uc.channelId =channelId;
    uc.name = name;
    uc.password = pass;
    uc.saveChats = saveMessages;

    Packet p;
    p.type = PacketType::UpdateChannel;
    p.payload = PacketHelpers::pack(uc);

    socket.write(p.serialize());
}

QString User::getChannelName(quint64 channelId)
{
    return m_channelModel->getChannelName(channelId);
}

void User::deleteChannel(quint64 channelId)
{
    DeleteChannelPacket d;
    d.channelId = channelId;

    Packet p;
    p.type = PacketType::DeleteChannel;
    p.payload = PacketHelpers::pack(d);

    socket.write(p.serialize());
}

ClientUser *User::clientUser(quint64 id)
{
    return m_clientUserManager->user(id);
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

void User::askForNotFoundAvatars()
{
    qDebug() << "asking for not found avatars... not found avatars count=" << m_notFoundAvatars.count();
    RequestAvatarsPacket ra;
    ra.notFoundIds = m_notFoundAvatars;

    Packet p;
    p.type = PacketType::RequestAvatars;
    p.payload = PacketHelpers::pack(ra);

    socket.write(p.serialize());
}

void User::newAvatarArrived(quint64 userId,
                            const QString& avatarHash,
                            const QString& oldAvatarHash,
                            const QByteArray& avatarData)
{
    qDebug()<< "new Avatars Arrived";

    //check whether that received avatarHash is valid?
    if(avatarHash.isEmpty())
        return;


    if(m_avatarManager.saveAvatar(SAVE_AVATAR_PATH+QString::number(m_connectedServerId_onDb)
                                   ,avatarHash,avatarData))
    {
        qDebug() << "avatar saved for that user, "
                 << "userid=" << userId
                 << "hash=" << avatarHash
                 << "oldHash=" << oldAvatarHash
                 << "avatarData.size=" << avatarData.size();

        //delete old avatar to protect privacy and decrease cache avatars size
        m_avatarManager.deleteAvatar(SAVE_AVATAR_PATH+QString::number(m_connectedServerId_onDb)
                                     ,oldAvatarHash);

        //check and get that avatar path.
        QString avatarPath = checkAvatar(userId, avatarHash,false); //FALSE to turn off askForAvatar

        if(avatarPath.isEmpty())
        {
            qDebug() << "avatarpath is empty!";
        }

        //check is it server's avatar or not
        if(userId == BeanChatCommon::ReservedIds::ServerAvatar)
        {
            //apply new avatar to myServers model
            if(m_myServersModel->setAvatarPath(avatarPath))
                qDebug() << "myServersModel avatar updated for that server avatarPath=" << avatarPath;
            else
                qDebug() << "failed to update avatar for that server on myServersModel avatarPath=" << avatarPath;

            //update datbase for that myServer id. to when didnt connected to servers load their avatar if found.
            QVariantMap values;
            values["avatarPath"] = avatarPath;
            if(m_database->update("MyServers", m_connectedServerId_onDb, values))
                qDebug() << "server avatarPath updated on myServers' table.";
            else
                qDebug() << "failed to update avatarPath on myServers' table.";


            return;
        }

        //notify models to update..
        ClientUser* user = m_clientUserManager->user(userId);
        user->setAvatarPath(avatarPath);

        //check if its me, set this to my variable to later load different parts like modifyProfile, userStuff's avatar
        if(user->self())
        {
            setMyAvatarPath(avatarPath);
            emit notificationRequested(NotificationType::Success,
                                       "Avatar has updated successfully.");
        }

    }
    else
        qDebug() << "failed to save avatar for that user...";
}

void User::onTcpReadyRead()
{
    m_tcpBuffer += socket.readAll();

    while (true)
    {

        // Need at least the header
        if (m_tcpBuffer.size() < 6)
            return;


        QDataStream in(m_tcpBuffer);

        quint16 type;
        quint32 payloadSize;

        in >> type;
        in >> payloadSize;

        // Wait until whole packet arrives
        if (m_tcpBuffer.size() < 6 + payloadSize)
        {
            qDebug() <<"TCP READ WAITING... to packet completes";
            return;
        }

        QByteArray payload =
            m_tcpBuffer.mid(6, payloadSize);

        Packet packet;
        packet.type = static_cast<PacketType>(type);
        packet.payload = payload;

        processPacket(packet);

        m_tcpBuffer.remove(0, 6 + payloadSize);
    }
}
void User::processPacket(const Packet& packet)
{
    qInfo() << "received message: code:" << static_cast<int>(packet.type);
    switch(packet.type)
    {

    case PacketType::LoginChallenge:
    {
        auto challange = PacketHelpers::unpack<LoginPacket>(packet.payload);
        qDebug() << "chalange received. lets proof";
        qDebug() << "Challenge received:" << packet.payload.toBase64();

        const Identity* identity = m_identityManager->currentIdentity();

        if(!identity)
            return;

        qDebug() << "Public:" << identity->publicKeyBase64();

        LoginPacket proof;

        // proof.payload = BeanChatCommon::Crypto::sign(identity->privateKey, packet.payload);
        QByteArray signature = Crypto::sign(identity->privateKey, challange.payload);
        proof.payload = signature;

        qDebug() << "proofing Signature:" << signature.toBase64();

        Packet p;
        p.type = PacketType::LoginProof;
        p.payload = PacketHelpers::pack(proof);

        qDebug() << "sending proof to server.";
        socket.write(p.serialize());

        break;
    }

    case PacketType::UserInfoChanged: //when users change their profile stuff, name,identtiy,avatar,...
    {
        auto info =
            PacketHelpers::unpack<UserInfoChangedPacket>(
                packet.payload);

        switch (info.updateType)
        {
            case UpdateUserInfoType::Username:
            {
                qDebug() << "received a username changed";
                ClientUser* user = m_clientUserManager->user(info.userId);
                if(!user)
                    return;

                user->setUsername(info.payloadValue);
                if(user->self())
                {
                    emit notificationRequested(NotificationType::Info,
                                               "Your username has been updated.");
                    setMyUsername(info.payloadValue);
                }

                break;
            }
            case UpdateUserInfoType::Avatar:
            {
                qDebug() << "received avatar: hash="  << info.payloadValue << " avatar size=" << info.payloadData.size();

                //save this new avatar and apply it to models
                newAvatarArrived(info.userId, info.payloadValue, info.payloadSecondValue, info.payloadData);

               break;
            }
            default:
                qDebug() << "unkown UpdateUserInfoType received.";
        }
    }break;


    case PacketType::ResponseAvatars:
    {
        qDebug() << "response avatars received lets update users' avatars";

        //clear list for next connection
        m_notFoundAvatars.clear();

        auto resp =
            PacketHelpers::unpack<ResponseAvatarsPacket>(
                packet.payload);


        for(UserAvatar& avatar : resp.avatars)
        {
            //save avatars and apply into models
            newAvatarArrived(avatar.userId, avatar.avatarHash, avatar.oldHash, avatar.imageData);
        }
        break;
    }
    case PacketType::ChannelUpdated:
    {
        qDebug() << " a channel updated";
        auto resp =
            PacketHelpers::unpack<ChannelUpdatedPacket>(
                packet.payload);

        m_channelModel->updateChannel(resp.channelId, resp.name, resp.isLocked, resp.saveChats);

        if(resp.channelId == m_myChannelId)
        {
            emit notificationRequested(NotificationType::Info,
                                       "Your channel has been updated.");
            setMyChannelName(resp.name);
            setMyChannelSavesChat(resp.saveChats);
        }
        break;
    }

    case PacketType::ChannelDeleted:
    {
        qDebug() << " a channel deleted";
        auto resp =
            PacketHelpers::unpack<DeleteChannelPacket>(
                packet.payload);

        m_channelModel->removeChannel(resp.channelId);

        if(resp.channelId == m_myChannelId)
        {
            emit notificationRequested(NotificationType::Warning,
                                       "Your channel has been deleted.");
            m_currentChannelParticipant->clear();
            setMyChannelName("");
            setMyChannelId(0); //default vlaue for homeless users :D
            setMyChannelSavesChat(false);
            setChatUnreadMessages(0);
        }
        break;
    }
    case PacketType::ChannelCreated:
    {
        qDebug() <<"a channel created";
        auto resp =
            PacketHelpers::unpack<ChannelCreatedPacket>(
                packet.payload);

        m_channelModel->addChannel(resp.id,resp.name,resp.isLocked, resp.saveChats);
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


                    //feed this user's vidoeSink with preview.
                    if (VideoSink *sink = m_currentChannelParticipant->videoSink(myId()))
                    {
                        connect(m_cam,
                                &CameraCapture::imageReady,
                                sink,
                                &VideoSink::setImage,
                                Qt::QueuedConnection);
                    }
                }
                else
                {
                    qDebug() << "stopping camera..";
                    if (VideoSink *sink = m_currentChannelParticipant->videoSink(myId()))
                    {
                        QObject::disconnect(m_cam,
                                   &CameraCapture::imageReady,
                                   sink,
                                   &VideoSink::setImage);
                    }

                    m_cam->stop();
                }
            }
            else
            {
                emit notificationRequested(NotificationType::Error,
                                           "Failed to open camera.");
            }


            emit isCameraOpenChanged();
        }
        else
        {
            qDebug() << "update cmaera for user:" << resp.userId << " to " << resp.status;
        }

        if (ClientUser *user = m_clientUserManager->user(resp.userId))
            user->setHasCamera(resp.status);
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

        if (ClientUser *user = m_clientUserManager->user(resp.userId))
            user->setMuted(resp.status);
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

        if (ClientUser *user = m_clientUserManager->user(resp.userId))
            user->setDeafened(resp.status);
    }break;


    case PacketType::UserMoved:
    case PacketType::UserJoinedChannel:
    {
        auto resp =
            PacketHelpers::unpack<UserJoinedChannelPacket>(
                packet.payload);

        //update position of user on channelModel, because in next codes we won't see user's change on that channel..
        m_channelModel->moveUser(resp.userId,resp.channelId);


        //if user has no channel add him to channelModel
        if(resp.oldChannelId==0) //if it's 0 means user just connected to server and didnt join any channel yet. so don't need add him channelModel
        {
            //read user's data from connectedUsersModel
            ClientUser* user = m_connectedUsersModel->findUser(resp.userId);

            //add user to channemodel
            m_channelModel->addUser(resp.channelId, user);
        }

        //check if user was me?
        if(resp.userId == static_cast<quint64>(myId()))
        {
            if(packet.type==PacketType::UserMoved)
            {
                emit notificationRequested(NotificationType::Info,
                                           "You are moved.",
                                           NotificationId::YouAreMoved);
            }
            else
                qDebug() << "voice: channel switched.";

            //check if i had channel dont remove me from paritcipant (because wanna keep preview feed)\
                    else remove everyone
            if(resp.oldChannelId>0)
                m_currentChannelParticipant->clearExcept(myId()); //reset channel participants except ourself, this way our camera live-preview won't broken and we skipped an unnecessary addUser into currentChannelParticipantModel
            else
                m_currentChannelParticipant->clear();

            //open mic and speaker
            if(m_mic && !m_mic->started()) //check is mic start or not if it's start dont restart it!
                m_mic->start();
            if(m_speaker && !m_speaker->started())  //check is speaker started or not, if it started dont restart.
                m_speaker->start();

            //rest talkin status of all previous channel users, because if dont it would show/stuck user is talkin on previous channel..
            m_channelModel->resetChannelTalkingStatus(resp.oldChannelId);

            //set channel name for Chat and other parts to know current channel name
            setMyChannelId(resp.channelId);
            m_channelModel->setCurrentChannelId(m_myChannelId); //for update isTalking status users
            setMyChannelName(m_channelModel->getChannelName(m_myChannelId)); //to show on top of Chat also on userConnectedServer.
            setMyChannelSavesChat(m_channelModel->getChannelSaveChats(m_myChannelId)); //to show on top of Chat

            //add each users of this channel of (which user joint) into participantModel
            ChannelItem* channel = m_channelModel->findChannel(resp.channelId);
            if(channel)
            {
                //add found users into participant model
                for (const UserItem &user : channel->users)
                {
                    //check if its me AND i was in a channel THEN skip adding me because in this case we didnt remove me at all from model
                    if(user.user->id() == myId() && resp.oldChannelId>0)
                        continue; //skip this round, we didnt remove ourself so no need add ourself into participant

                    if (ClientUser *clientUser = m_clientUserManager->user(user.user->id()))
                        m_currentChannelParticipant->addUser(clientUser);
                }
            }
            else
                qDebug() << "could not find channel id:" << resp.channelId;
        }

        //check did user join into my channel?
        else if(resp.channelId == m_myChannelId)
        {
            if(packet.type==PacketType::UserMoved)
                qDebug() << "voice: user moved into your channel.";
            else
                qDebug() << "voice: user joined to your channel.";

            //find channel and that user info and add into participantModel
            ChannelItem* channel = m_channelModel->findChannel(resp.channelId);
            if(channel)
            {
                ClientUser* jointUser = m_channelModel->findUserInChannel(channel,resp.userId);
                if(jointUser)
                    if (ClientUser *clientUser = m_clientUserManager->user(jointUser->id()))
                        m_currentChannelParticipant->addUser(clientUser);
                else
                    qDebug() << "user joined could not find user inside channel id:" << resp.channelId << " userid=" << resp.userId;
            }
            else
                qDebug() << "user joined could not find channel id:" << resp.channelId;

            //for soundmanager to play effect.
            emit userJoined();
        }

        //check did user left my channel?
        else if(resp.oldChannelId==m_myChannelId)
        {
            if(packet.type==PacketType::UserMoved)
                qDebug() << "voice: user moved from your channel.";
            else
                qDebug() << "voice: user left your channel.";


            //remove that user from participant model
            m_currentChannelParticipant->removeUser(resp.userId);

            //rest leaved user talkin status
            if (ClientUser *user = m_clientUserManager->user(resp.userId))
                user->setIsTalking(false);

            //for soundmanager to play effect
            emit userLeft();
        }

        else //user's action is not my concern, no sound effect or additional actions
            qInfo () << "user (" << resp.userId << ") has left " << resp.oldChannelId << " and joined to " << resp.channelId ;



        //update user's channelId
        if (ClientUser *user = m_clientUserManager->user(resp.userId))
            user->setChannelId(resp.channelId);
        break;
    }

    case PacketType::LoginResponse:
    {
        auto resp =
            PacketHelpers::unpack<LoginResponsePacket>(
                packet.payload);

        if(!resp.accepted)
        {
            emit notificationRequested(NotificationType::Error,
                                       "Connection Rejected. ("+resp.message+")",
                                       NotificationId::ConnectionRejected,
                                       NotificationDuration::Long);
            disconnect();
        }
        else
        {
            setMyId(resp.id); //server just told us our name, to know when e.g: user connected to that channel is that same channel as us? what is my id? so here is it.
            qDebug() << "my id is=" << myId();


            loginToUdpSocket();

            //ask for server channels, users, ...
            askForServerState();
        }

        break;
    }

    case PacketType::UserConnected:
    {
        auto u =
            PacketHelpers::unpack<UserConnectedPacket>(
                packet.payload);

        qDebug() << "User connected:" << u.username << " identity:" << u.identity;

        //add user.
        ClientUser* user = m_clientUserManager->createUser(u.id);
        if(user)
        {
            QString avatarPath = checkAvatar(u.id, u.avatarHash);
            if(avatarPath.isEmpty())
            {
                //send request to server for all not found avatars
                askForNotFoundAvatars();
            }
            qDebug() << "add user to connected list: " <<  u.appVersion << "-"
                     << u.buildType << "-" << u.osName
                     << "-" << u.osVersion << "- avatar hash= " << u.avatarHash
                     << "avatar path=" << avatarPath << "channelid=";

            //if user is me set myAvatarPath
            if(u.id == myId())
            {
                qDebug() << "user connected received, this user is me: ";
                setMyAvatarPath(avatarPath);
                user->setSelf(true);
            }


            user->setUsername(u.username);
            user->setIdentity(u.identity);
            user->setAvatarPath(avatarPath);
            // user->setIconsId(u.icon); //for now packet haven;t this
            user->setMuted(u.muted);
            user->setDeafened(u.deafened);
            user->setHasCamera(u.camera);
            // user->setStatus(u.status); //for now packet haven;t this
            user->setAppVersion(u.appVersion);
            user->setBuildType(u.buildType);
            user->setOsName(u.osName);
            user->setOsVersion(u.osVersion);

            //load info about this user. from our UserRelationship
            if(!user->self())
            {
                const UserRelationship *relationship = m_relationshipManager->find(u.identity);
                if (relationship)
                {
                    user->setRelationship(relationship->relationship);
                    user->setNickname(relationship->nickname);
                    user->setNote(relationship->note);
                    user->setVolume(relationship->voiceVolume);
                    user->setLocalMuted(relationship->muted);
                }
                else
                {
                    user->setRelationship(Relationship::Type::None);
                    user->setNickname({});
                    user->setNote({});
                    user->setVolume(SPEAKER_DEFAULT_CHANNEL_USERS_VOLUME);
                    user->setLocalMuted(false);
                }

            }

            //add him to list.
            m_connectedUsersModel->addUser(user);

            qDebug() << "user added to connectedusrs id="<<u.id;
        }
        else
            qDebug() << "failed to create user, invalid id or user exists. id=" << u.id;


        break;
    }

    case PacketType::UserConnectionLost:
        qInfo () << "user connection lost.";
    case PacketType::UserDisconnected:
    {
        qInfo() << "user disconnected:";
        auto resp =
            PacketHelpers::unpack<UserDisconnectedPacket>(
                packet.payload);

        qDebug()<< "User disconnected:" << resp.id << " wasConnectinLost?" << resp.wasConnectionLost;

        ChannelItem* channel = m_channelModel->findChannelOfUser(resp.id);
        if(channel)
        {
            //check whether user was on our channel, if was play sound effect
            if(channel->id == m_myChannelId)
            {
                qDebug() << "play user left.";
                emit userLeft();

                //try to remove him from participantmodel of our channel
                m_currentChannelParticipant->removeUser(resp.id);
            }
        }
        else
            qDebug() << "invalid channel id, not found cant find usr was in our channel or not";


        //remove user from connected users list
        m_connectedUsersModel->removeUser(resp.id);

        //also dont know if user was inside a channel or not anyway try to remove him from model
        m_channelModel->removeUser(resp.id);


        //set some status for that user. e.g set him offline to show user status as offline on chat
        ClientUser* user = m_clientUserManager->user(resp.id);
        if(user)
        {
            user->setStatus(ClientUser::Status::Offline);
        }

        //remove user.
        m_clientUserManager->removeUser(resp.id);

        break;
    }

    case PacketType::ChatMessage:
    {
        auto msg = PacketHelpers::unpack<ChatMessagePacket>(packet.payload);

        //show notification dot near chat indicator when chat isn't open
        if(!m_isChatOpen)
            setChatUnreadMessages(chatUnreadMessages()+1); //increase unread messages count

        if(msg.senderId==myId())
            emit messageSent(); //play message sent effect.
        else
        {
            //check if chat is not open (user is in connectedUsersList), \
                    show notification and play effect
            if(!isChatOpen())
            {
                emit notificationRequested(NotificationType::Info,
                                           "New Message received.",
                                           NotificationId::Message,
                                           NotificationDuration::Short);

                emit newMessage(); //play message recevied effect.
            }
        }

        ClientUser* senderUser = m_clientUserManager->user(msg.senderId);
        if(senderUser)
        {
            m_chatModel->addMessage(msg,senderUser);
        }

    }
    break;

    case PacketType::ServerState:
    {
        qInfo() << "server state:";
        auto state =
            PacketHelpers::unpack<ServerStatePacket>(
                packet.payload);

        //store server info
        m_receivedServerInfo = state.serverInfo;
        emit receivedServerInfoChanged(); //notify QML serverInfo changed go read those exposed methods

         //get avatar path or ask from server, id=RESERVED_TO_ASK_SERVERS_AVATAR server would return his avatarHash and data.
        QString serverAvatarPath = checkAvatar(BeanChatCommon::ReservedIds::ServerAvatar, state.serverInfo.avatarHash);


        if(!serverAvatarPath.isEmpty())
        {
            //update myServerModel avatar for this server.
            m_myServersModel->setAvatarPath(serverAvatarPath);
        }
        // else checkAvatar() would add id RESERVED .. to list \
                then askForNotFoundAvatars would send server that list and when response \
                arrived apply avatars for users or in this case if userId==RESERVED_TO_ASK_SERVERS_AVATAR \
                would apply it for myServers' avatarPath



        //channels
        m_channelModel->clear();        
        for(auto& c : state.channels)
        {
            m_channelModel->addChannel(
                c.id,
                c.name,
                c.isLocked,
                c.saveChats);
        }

        //users
        for(auto& u : state.users)
        {
            //add user.
            ClientUser* user = m_clientUserManager->createUser(u.id);
            qDebug() << "userid=" << u.id;
            if(user)
            {
                QString avatarPath = checkAvatar(u.id, u.avatarHash);

                qDebug() << "add user to connected: " <<  u.appVersion << "-"
                         << u.buildType << "-" << u.osName
                         << "-" << u.osVersion << "- avatar hash= " << u.avatarHash
                         << "avatar path=" << avatarPath << "channelid=" << u.channelId;

                //if user is me set myAvatarPath
                if(u.id == myId())
                {
                    qDebug() << "state received, this user is me: ";
                    setMyAvatarPath(avatarPath);
                    user->setSelf(true);
                    // setMyStatus(u.status); //for now packet doesn't support it.
                }


                user->setUsername(u.username);
                user->setIdentity(u.identity);
                user->setAvatarPath(avatarPath);
                // user->setIconsId(u.icon); //for now packet haven;t this
                user->setMuted(u.muted);
                user->setDeafened(u.deafened);
                user->setChannelId(u.channelId);
                user->setHasCamera(u.camera);
                // user->setStatus(u.status); //for now packet haven;t this
                user->setAppVersion(u.appVersion);
                user->setBuildType(u.buildType);
                user->setOsName(u.osName);
                user->setOsVersion(u.osVersion);

                if(!user->self())
                {
                    //load info about this user. from our UserRelationship
                    const UserRelationship* relationship = m_relationshipManager->find(u.identity);
                    if (relationship)
                    {
                        user->setRelationship(relationship->relationship);
                        user->setNickname(relationship->nickname);
                        user->setNote(relationship->note);
                        user->setVolume(relationship->voiceVolume);
                        user->setLocalMuted(relationship->muted);
                    }
                    else
                    {
                        user->setRelationship(Relationship::Type::None);
                        user->setNickname({});
                        user->setNote({});
                        user->setVolume(SPEAKER_DEFAULT_CHANNEL_USERS_VOLUME);
                        user->setLocalMuted(false);
                    }
                }

                //add him to list
                m_connectedUsersModel->addUser(user);

                qDebug() << "user added to connectedusrs id="<<u.id;


                //if user was in a channel, add him to our channel model
                if(u.channelId!=0)
                    m_channelModel->addUser(u.channelId, user);
                else
                    qDebug() << "user channel id ==0 we dont add him to channel model";
                qDebug() << "user added to connectedusrs id="<<u.id;
            }
            else
                qDebug() << "failed to create user, invalid id or user exists. id=" << u.id;

        }

        //send request to server for all not found avatars
        askForNotFoundAvatars();

    }
    break;

    default:
        qDebug() << "an unknown packetType received. type=" << static_cast<int>(packet.type);
        break;
    }
}

void User::loginToUdpSocket()
{
    //lookup for domain's ip, udp doesnt do this automatically. but TCP does.
    QHostInfo info = QHostInfo::fromName(m_serverIp);
    if (!info.addresses().isEmpty())
        m_serverLookedupAddress = info.addresses().first();



    UdpRegisterPacket reg;

    reg.userId = static_cast<quint64>(myId());

    QByteArray data;

    QDataStream out(
        &data,
        QIODevice::WriteOnly);

    //for now we dont login for UDP later need token/identity and .. for security
    //code here

    out << PacketType::UdpLoginRequest;
    out << reg;


    qDebug() << "just sent a login udp message to server.";
    qint64 bytes = m_udpSocket.writeDatagram(
        data,
        m_serverLookedupAddress,
        m_serverPort);

    if (bytes == -1)
    {
        qDebug() << "UDP send failed:"
                 << m_udpSocket.errorString();
    }
    else
    {
        qDebug() << "UDP sent"
                 << bytes
                 << "bytes to"
                 << m_serverIp
                 << ":"
                 << m_serverPort;
    }
}

void User::setMyAppVersion(const QString &newAppVersion)
{
    m_appVersion = newAppVersion;
    emit myAppVersionChanged();
}

quint64 User::myChannelId() const
{
    return m_myChannelId;
}

void User::setMyChannelId(quint64 newMyChannelId)
{
    if (m_myChannelId == newMyChannelId)
        return;
    m_myChannelId = newMyChannelId;
    emit myChannelIdChanged();
}

ClientUser::Status User::myStatus() const
{
    return m_myStatus;
}

void User::setMyStatus(const ClientUser::Status &newMyStatus)
{
    if (m_myStatus == newMyStatus)
        return;
    m_myStatus = newMyStatus;
    emit myStatusChanged();
}


int User::connectedServerId() const
{
    return m_connectedServerId_onDb;
}

void User::setConnectedServerId(int newConnectedServerId)
{
    if (m_connectedServerId_onDb == newConnectedServerId)
        return;
    m_connectedServerId_onDb = newConnectedServerId;
    emit connectedServerIdChanged();
}

QString User::myAvatarPath() const
{
    return m_myAvatarPath;
}

void User::setMyAvatarPath(const QString &newMyAvatarPath)
{
    if (m_myAvatarPath == newMyAvatarPath)
        return;
    m_myAvatarPath = newMyAvatarPath;
    emit myAvatarPathChanged();
}

bool User::myChannelSavesChat() const
{
    return m_myChannelSavesChat;
}

void User::setMyChannelSavesChat(bool newMyChannelSavesChat)
{
    if (m_myChannelSavesChat == newMyChannelSavesChat)
        return;
    m_myChannelSavesChat = newMyChannelSavesChat;
    emit myChannelSavesChatChanged();
}

QString User::appTitle() const
{
    return QString::fromUtf8(APP_TITLE) + " v" + QString::fromUtf8(APP_VERSION);
}

void User::updateMyProfile(const QString &username, const QString &avatarPath)
{
    qDebug()<< "update my profile username to " << username << "avatarpath to " << avatarPath;

    //save locally in files of myServers..

    //check is username changed?
    if(username != myUsername())
    {
        if(isConnectedToServer())
        {
            //send request updaet to server.
            UpdateUserInfoPacket uu;

            uu.updateType = UpdateUserInfoType::Username;
            uu.payloadValue = username;

            Packet p;
            p.type = PacketType::UpdateUserInfo;
            p.payload = PacketHelpers::pack(uu);
            socket.write(p.serialize());
        }
        else
        {
            //update local username for upcoming connections.
            setMyUsername(username);
        }
    }

    if(!avatarPath.isEmpty())
    {
        //send request update to server
        UpdateUserInfoPacket uu;
        uu.updateType = UpdateUserInfoType::Avatar;
        uu.paylaodData = m_avatarManager.imageFileToBytes(avatarPath);
        qDebug() << "avatar size=" << uu.paylaodData.size();

        Packet p;

        p.type = PacketType::UpdateUserInfo;
        p.payload = PacketHelpers::pack(uu);
        socket.write(p.serialize());
    }

}


QString User::checkAvatar(quint64 userId, const QString &avatarHash, bool askForAvatar)
{
    //check for user's avatar sotred in cache or not.
    if(!avatarHash.isEmpty())
    {
        //check cache folder
        if(m_avatarManager.avatarExists(SAVE_AVATAR_PATH+QString::number(m_connectedServerId_onDb),avatarHash)) //this method will append / at end path and .png at end of filename
        {
            QString avatarPath = SAVE_AVATAR_PATH
                                 +QString::number(m_connectedServerId_onDb)
                                 +"/"+avatarHash
                                 +".png";
            avatarPath = QUrl::fromLocalFile(avatarPath).toString();
            qDebug() << "avatar found = " << avatarPath;
            return avatarPath;
        }

        if(askForAvatar)
        {
            qDebug() << "avatar not found for  "
                     << "hash=" << avatarHash
                     << "userid=" << userId
                     << "would send request for this.";

            m_notFoundAvatars.append(userId);
        }
    }
    return "";
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

    QString errorMessage;
    switch (error)
    {
        case QAbstractSocket::NetworkError:
            errorMessage = "Network is unreachable";
            break;

        case QAbstractSocket::HostNotFoundError:
            errorMessage = "DNS failed or host doesn't exist";
            break;

        case QAbstractSocket::ConnectionRefusedError:
            errorMessage = "Server rejected the connection";
            break;

        case QAbstractSocket::RemoteHostClosedError:
            errorMessage = "Server closed the connection";
            break;

        case QAbstractSocket::SocketTimeoutError:
            errorMessage = "Connection timed out";
            break;

        default:
            errorMessage = socket.errorString();
            break;
    }

        emit notificationRequested(NotificationType::Error,
                                   "Network Error: "+errorMessage,
                                   NotificationId::ConnectionError);

    //make sure UDP socket is closed too.
    m_udpSocket.disconnectFromHost();

    //make sure reset variables for next connection
    resetVariables();
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

        switch(static_cast<PacketType>(type))
        {
        case PacketType::UdpLoginResponse:
        {
            qDebug() << "udp logged in successfully.";
            emit notificationRequested(NotificationType::Success,
                                       "Connected to "+myServerName());

            //start to expect every xSeconds ping request from server otherwise, assuming UDP connection has failed
            m_udpConnectionTimeout.start(UDP_CONNECTION_LOST_TIMER_INTERVAL); // 10 seconds
            break;
        }
        case PacketType::UdpVoiceData:  //voice
        {
            VoicePacket packet;
            in >> packet;

            if(!muteHeadphone())
            {
                ClientUser* senderUser = m_channelModel->getUser(m_myChannelId, packet.senderId);

                if(!senderUser)
                    continue;

                if(!senderUser->isTalking())
                    senderUser->setIsTalking(true);

                m_channelModel->restartVoiceTimer(senderUser->id());

                //check if user is muted locally.
                if(senderUser->localMuted())
                    break;


                //decode
#if D_PRINT_AUDIO_INFO
                QElapsedTimer t;
                t.start();
#endif
                QByteArray pcm = m_opus.decode(packet.audioData);
#if D_PRINT_AUDIO_INFO
                qDebug() << "decode =" << t.nsecsElapsed()/1000000.0 << "ms";
#endif
#if D_PRINT_VOICE_INFO
                qDebug() << "received opus=" << pcm.size() << " raw pcm=" << packet.audioData.size();
#endif

                if (!pcm.isEmpty())
                    emit voiceReceived(senderUser->id(),
                                       pcm, senderUser->volume());
            }
#if D_PRINT_AUDIO_INFO
            qDebug()
                << "Voice received from"
                << packet.senderId
                << "seq"
                << packet.sequence
                << "size"
                << packet.audioData.size();
#endif
            break;
        }

        case PacketType::UdpVideoData: //video
        {
            VideoFragment frag;

            in >> frag;

            //Create a unique key
            QString key = QString("%1:%2").arg(frag.senderId).arg(frag.frameId);

            //Get the PendingFrame
            BeanChatCommon::PendingFrame& pending = m_pendingFrames[key];

            //Initialize it once
            if (pending.fragments.isEmpty())
            {
                pending.expectedFragments =
                    frag.fragmentCount;

                pending.fragments.resize(
                    frag.fragmentCount);
            }

            //Before storing, validate it:
            if (frag.fragmentIndex >= pending.fragments.size())
            {
                qDebug() << "Invalid fragment index";
                break;
            }

            //avoid counting duplicates
            if (pending.fragments[frag.fragmentIndex].isEmpty())
            {
                pending.fragments[frag.fragmentIndex] =
                    frag.payload;

                pending.received++;
            }

            //Wait until complete
            if (pending.received != pending.expectedFragments)
            {
                break;
            }

            //Rebuild the packet
            QByteArray packetData;
            for (const QByteArray &piece : pending.fragments)
            {
                packetData += piece;
            }

            //queue the sender
            m_decodeQueue.enqueue(frag.senderId);

            //decode
            m_videoDecoder.decode(packetData);


            //cleanup
            m_pendingFrames.remove(key);

            break;
        }

        case PacketType::UdpPingRequest: //ping requst from server.
        {
            PingPacket p;
            in >> p;

            //set variables to show in QML component.
            setMyPing(p.lastPing);
            setMyVoicePacketLoss(p.voicePacketLoss);
            setMyVideoPacketLoss(p.videoPacketLoss);


            //a udp request-ping received, now reset connection timeout to later know is udp connection still alive or not
            m_udpConnectionTimeout.start(UDP_CONNECTION_LOST_TIMER_INTERVAL);


            //send back same sequence..
            QByteArray data2;

            QDataStream out(
                &data2,
                QIODevice::WriteOnly);

            out << PacketType::UdpPingResponse;
            out << p;

            m_udpSocket.writeDatagram(
                data2,
                m_serverLookedupAddress,
                m_serverPort);

            break;
            }
        }
    }
}

void User::sendVideoFrame(const QByteArray &videoData)
{
#if D_PRINT_VIDEO_INFO
    qDebug() << "sendVideoFrame:" << videoData.size();
#endif

    if(!isConnectedToServer())
        return;

    if(!isCameraOpen())
        return;

    if(myId() < 0)
        return;

    const int chunkSize = 1100;

    quint32 frameId = ++m_videoSequence;

    int count = (videoData.size() + chunkSize - 1) / chunkSize;

    for (int i = 0; i < count; i++)
    {
        VideoFragment frag;

        frag.senderId = myId();
        frag.frameId = frameId;
        frag.fragmentIndex = i;
        frag.fragmentCount = count;
        frag.payload = videoData.mid(i * chunkSize, chunkSize);

        QByteArray datagram;
        QDataStream out(&datagram, QIODevice::WriteOnly);

        out << PacketType::UdpVideoData;
        out << frag;

#if D_PRINT_VIDEO_INFO
        qDebug()
            << "Frame"
            << frameId
            << "Fragment"
            << i + 1
            << "/"
            << count
            << "Payload"
            << frag.payload.size();
#endif

        m_udpSocket.writeDatagram(
            datagram,
            m_serverLookedupAddress,
            m_serverPort);
    }
}

void User::currentIdentityChangedTo(const QString &name)
{
    //update config
    m_settingsManager->setValue(USER_SETTING_LAST_IDENTITY_NAME,name);
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

void User::resetVariables()
{
    //clear models
    m_channelModel->clear();
    m_currentChannelParticipant->clear();
    m_connectedUsersModel->clear();
    m_chatModel->clear();

    //reset variables
    setMyServerName("");
    setMyChannelName("");
    setMyChannelId(0);
    setMyChannelSavesChat(false);
    setIsConnectedToServer(false);
    setConnectedServerId(-1); //this is serverIndexDb which would use in saving user's avatar in each server's directory
    m_connectedServerId_onDb=-1;
    m_serverIp.clear();
    m_serverPort=0;
    // setConnectionStatus(UserConnectionStatus::Disconnected);
    setMyPing(-1);
    setMyVideoPacketLoss(0.0f);
    setMyVoicePacketLoss(0.0f);
    m_notFoundAvatars.clear(); //clear list for next connection
    setMyAvatarPath("");
    m_clientUserManager->clear();

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

// UserConnectionStatus User::connectionStatus() const
// {
//     return m_connectionStatus;
// }

// void User::setConnectionStatus(UserConnectionStatus newConnectionStatus)
// {
//     if (m_connectionStatus == newConnectionStatus)
//         return;
//     m_connectionStatus = newConnectionStatus;
//     emit connectionStatusChanged();
// }

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


    //try to read appversion if not exists use build version
    if(m_settingsManager->contains(APP_SETTING_VER))
        setMyAppVersion(m_settingsManager->value(APP_SETTING_VER, APP_VERSION).toString());
    else //set default setting.
    {
        m_settingsManager->setValue(APP_SETTING_VER, APP_VERSION);
        setMyAppVersion(APP_VERSION);
    }

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


    //user info
    if(m_settingsManager->contains(USER_SETTING_USERNAME))
        setMyUsername(m_settingsManager->value(USER_SETTING_USERNAME, "").toString());
    else
        qDebug()<< "username not found in config";

    if(m_settingsManager->contains(USER_SETTING_LAST_IDENTITY_NAME))
        m_identityManager->setCurrentIdentity(m_settingsManager->value(USER_SETTING_LAST_IDENTITY_NAME, "").toString());
    else
        qDebug()<< "identity last name not found in config";



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
    return m_appVersion;
}

QString User::buildType() const
{
#ifdef QT_NO_DEBUG
    return USER_BUILD_TYPE_RELEASE;
#else
    return USER_BUILD_TYPE_DEBUG;
#endif
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

QString User::myIdentity()
{
    Identity* tempId = m_identityManager->currentIdentity();
    return tempId ? tempId->publicKeyBase64() : QString();
}

void User::setMyIdentity(const QString &newIdentity)
{
    //code here
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

    //update config username
    m_settingsManager->setValue(USER_SETTING_USERNAME, newMyUsername);

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

