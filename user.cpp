#include "user.h"

#include "logging/loggingcategories.h"

User::User(ChannelModel *channelModel, ChatModel *chatModel,
           ParticipantModel* currentChannelParticipant, ConnectedUsersModel *connectedUsersModel, MyServersModel* myServersModel,
           SoundManager* sounderManager, SettingsManager* settingsManager,
           ClientUserManager *clientuserManager, IdentityManager *identityManager,
           RelationshipManager* relationshipManager, Database* database,
           AttachmentImageProvider* attachmentImageProvider,
           CameraCapture* cam, AudioCapture *mic, AudioSpeaker* speaker,
           QObject *parent)
    : QObject{parent}, m_channelModel(channelModel), m_chatModel(chatModel),
    m_currentChannelParticipant(currentChannelParticipant), m_connectedUsersModel(connectedUsersModel),
    m_myServersModel(myServersModel),
    m_soundManager(sounderManager), m_settingsManager(settingsManager),
    m_clientUserManager(clientuserManager), m_identityManager(identityManager),
    m_relationshipManager(relationshipManager), m_database(database),
    m_attachmentImageProvider(attachmentImageProvider),
    m_cam(cam), m_mic(mic), m_speaker(speaker)
{
    qCInfo(_app) << "starting app";
    qCInfo(_app) << "using BeanChatCommon version " << BeanChatCommon::Protocol::Version;


    //read or generate identitiy if not exsit
    if(!m_identityManager->load())
    {
        qCFatal(_identity) << "failed to load and generate default identity.";
    }

    qCInfo(_identity) << "Loaded identities count= " << m_identityManager->identities().count();
    qCDebug(_identity) << "found identities:";
    for(auto& identity : m_identityManager->identities())
    {
        qCDebug(_identity)  << "name= " <<identity.name
                          << " pub:" <<identity.publicKeyBase64()
                          << "created at=" <<identity.createdAt;
    }
    //notify QML identity loaded/changed
    emit myIdentityChanged();


    if (!m_opus.initialize(OPUS_DEFAULT_SAMPLE_RATE,
                           OPUS_DEFAULT_CHANNELS,
                           OPUS_DEFAULT_BITRATE))
    {
        qCFatal(_opus) << "Failed to initialize Opus";
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

            sink->setImage(image);
        });


    //settings
    initOrLoadSettings();


    //load all saved servers from database.
    qCInfo(_app) << "loading all saved servers.";
    QVariantList servers = m_database->getAll("MyServers");
    for (const QVariant &v : servers)
    {
        QVariantMap row = v.toMap();

        qCInfo(_database)
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



    //connect clientUserManager to models when user removed, models obey
    connect(
        m_clientUserManager,
        &ClientUserManager::userRemoved,
        m_channelModel,
        &ChannelModel::removeUser);

    connect(
        m_clientUserManager,
        &ClientUserManager::userRemoved,
        m_connectedUsersModel,
        &ConnectedUsersModel::removeUser);

    connect(
        m_clientUserManager,
        &ClientUserManager::userRemoved,
        m_currentChannelParticipant,
        &ParticipantModel::removeUser);



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
    if(!m_udpSocket.bind())
        qCFatal(_udp) << "failed to bind socket";


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
                    qCInfo(_udp) << "server didn't send ping request for a while, so assuming connection has lost.";
                    emit notificationRequested(NotificationType::Error,
                                               "Connection Lost",
                                               NotificationId::ConnectionLost,
                                               NotificationDuration::Long);
                    emit youConnectionLost();
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





    //setup update checker
    connect(&m_updateChecker,
            &UpdateChecker::updateAvailable,
            this,
            [this](const LatestResponse &response)
            {
                qCInfo(_app) << "update available, update to " << response.latestVersion().toString() << "current version = " << myAppVersion();
                emit showImportantNotifierBar("Update "+ response.latestVersion().toString() + " is available",
                                                                         ImportantNotificationColor::Blue);
            });

    connect(&m_updateChecker,
            &UpdateChecker::noUpdateAvailable,
            this,
            []()
            {
                qCInfo(_app) << "Already up to date.";
            });

    connect(&m_updateChecker,
            &UpdateChecker::errorOccurred,
            this,
            [this](const QString &err)
            {
                qCWarning(_app) << "error to check for update err=" << err;
                // emit showImportantNotifierBar("Failed to check for update, "+err,
                //                               ImportantNotificationColor::Red);
            });


    //check for update at startup
    QString targetPlatform =  platformName();
    if(targetPlatform=="Windows") targetPlatform="windows-x64";
    else if(targetPlatform=="Linux") targetPlatform="linux-x64";
    else if(targetPlatform=="Android") targetPlatform="android-arm8";

    m_updateChecker.checkForUpdates(targetPlatform, myAppVersion());
}


void User::joinChannel(quint64 channelId, const QString& password)
{
    qCInfo(_tcp) << "sending join channel request";
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
        qCWarning(_app) << "invalid channel id to check isLocked.";
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

QString User::serverMaxUsers() const
{
    return QString::number(m_receivedServerInfo.maxUsers);
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
    qCInfo(_tcp) << "sending move user request";
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
    qCInfo(_app) << "connect to server.";
    //if user is connected to somewhere, disconnect before new connection
    if(isConnectedToServer())
        disconnect();


    //check is server saved or is temporary?
    if(m_connectedServerId_onDb==-1) //server is temporary
    {
        qCInfo(_app) << "server connection is termporary. connectedServerId DB=" << m_connectedServerId_onDb;
    }

    //convert ports to quint64
    bool ok = false;
    quint64 serverPort = str_serverPort.toULongLong(&ok);
    if(!ok)
    {
        qCWarning(_app) << "Invalid port number, port=" << str_serverPort;
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
        QString serverName= USER_DEFAULT_SERVER_NAME;
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
                qCInfo(_app) << "server saved to myServers";
                QVariantMap serverInfo = m_database->getServer(serverIp,str_serverPort);

                if (serverInfo.isEmpty())
                {
                    qCWarning(_app) << "while reading data from recently added server got: Server not found";
                    return;
                }

                serverDbIndex = serverInfo["id"].toInt();
                m_connectedServerId_onDb =serverDbIndex;
                serverName = serverInfo["name"].toString();
                setMyServerName(serverName);
            }
            else
                qCCritical(_app) <<  "failed to save server to myServers.";
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
    login.status =myStatus();
    if(!m_identityManager->currentIdentity())
    {
        qCInfo(_app) << "connect failed. no identity selected... create one";
        if(m_identityManager->createIdentity("Default"+QString::number(QRandomGenerator::global()->bounded(100))))
        {
            qCInfo(_app) << "we create one new identity for you";
        }
        else
        {
            qCWarning(_app) << "you didnt have an identity and sadly we couldn't create one for you!";
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


    qCInfo(_tcp) << "sending login request.. will wait for response.. connecting server is "
             << m_serverIp << ":" << m_serverPort  << " name=" << myUsername() << "identity=" << myIdentity() ;

    socket.write(p.serialize());


    //reset flag for next use.
    m_switchingServer=false;
}

void User::updateSavedServer(quint64 serverId, quint64 dbIndex, const QString& name, const QString& ip, const QString& port)
{
    qCInfo(_app) << "update saved server.";
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

        qCInfo(_app) << "myserver updated.";
        //update model data.
        m_myServersModel->updateServer(serverId,name,ip,port);

        //update servername on local variable too
        setMyServerName(name);
    }
    else
    {
        qCWarning(_app) << "failed to update MyServer.";
        emit notificationRequested(NotificationType::Error,
                                   "Failed to update MyServer.");
    }
}

void User::deleteSavedServer(quint64 serverId, quint64 serverDbIndex)
{
    qCInfo(_app) << "delete saved server.";
    if(serverDbIndex!=-1) //server is not saved in database. just delete it from model.
    {
        bool res = m_database->remove("MyServers",serverDbIndex);
        if(res)
        {
            qCInfo(_app) << "server deleted from MyServers.";
            emit notificationRequested(NotificationType::Success,
                                       "Server deleted from MyServers.");

            //delete saved avatars in that server's avatar directory
            qCInfo(_app) << "trying to delete avatars of that server: target path = " << SAVE_AVATAR_PATH+QString::number(serverDbIndex);

            QDir dir(SAVE_AVATAR_PATH+QString::number(serverDbIndex));
            if (dir.exists())
            {
                if (!dir.removeRecursively())
                    qCWarning(_app) << "Failed to remove avatar.";
            }
            else
                qCInfo(_app) << "that path avatar doesn't exists.";
        }
        else
        {
            qCWarning(_app) << "Failed to delete server from MyServers.";
            emit notificationRequested(NotificationType::Error,
                                       "Failed to delete server from MyServers.");
        }
    }

    //anyway delete from model
    m_myServersModel->removeServer(serverId);
}

void User::switchOrConnectToServer(const QString &serverIp, const QString &str_serverPort, int serverId)
{
    qCInfo(_app) << "switch OR connectToServer server-id: " << serverId;
    //tell myServers model im connected to this server.
    m_myServersModel->setIsActive(serverId);

    m_switchingServer=true;

    //do normal connectToServer things
    connectToServer(false, serverIp,str_serverPort);
}

void User::disconnect()
{
    qCInfo(_app) << "disconnect.";
    if(!isConnectedToServer()) //to prevent double run this function, first user do disconnect manually/switched to antoher server, then QTCPSocket::Disconnect would run this again..
        return;

    emit notificationRequested(NotificationType::Error,
                               "Disconnected",
                               NotificationId::Disconnected,
                               NotificationDuration::Short);

    emit youDisconnected();

    //disocnnect sockets.
    socket.disconnectFromHost();
    m_udpSocket.disconnectFromHost();

    resetVariables();

}

void User::createChannel(QString channelName, QString password, bool saveMessages)
{
    qCInfo(_tcp) << "sending create channel request";
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
        // qCCritical(_app) << "microphone or headphone is muted, send voice abort..";
        return;
    }

    if(m_myId < 0)
    {
        qCCritical(_app) << "myId is invalid.";
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

void User::sendMessage(const QString& message)
{
    qCInfo(_tcp) << "sending simple message ";
    SendMessagePacket sm;
    sm.text = message;
    sm.type = Msg::Type::Text;
    sm.attachmentId=0;

    Packet p;
    p.type = PacketType::ChatMessage;
    p.payload = PacketHelpers::pack(sm);

    socket.write(p.serialize());
}

void User::sendMessage(const QString& message,quint64 attachId, const QUrl &url)
{
    qCInfo(_tcp) << "sending message request with attachid="<<attachId;
    SendMessagePacket sm;
    sm.text = message;

    QMimeDatabase db;
    QString name = db.mimeTypeForFile(url.toLocalFile()).name();
    if (name.startsWith("image/"))
    {
        if(name.contains("gif"))
            sm.type=Msg::Type::AnimatedImage;
        else
            sm.type = Msg::Type::Image;
    }
    else if (name.startsWith("video/"))
    {
        sm.type = Msg::Type::Video;
    }
    else if (name.startsWith("audio/"))
    {
        sm.type = Msg::Type::Audio;
    }
    else
    {
        sm.type = Msg::Type::File;
    }
    sm.attachmentId=attachId;

    Packet p;
    p.type = PacketType::ChatMessage;
    p.payload = PacketHelpers::pack(sm);

    socket.write(p.serialize());
}

void User::sendFile(const QString &filePath)
{
    if (m_uploadFile.isOpen())
    {
        qWarning() << "Already uploading a file.";
        emit sendFileResult(false, "Alread uploading a file... try later",0);
        return;
    }
    QString localPath = QUrl(filePath).toLocalFile();

    QFile file(localPath);

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot open file:" << localPath;
        emit sendFileResult(false, "cannot open file: "+localPath,0);
        return;
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);

    while (!file.atEnd())
        hash.addData(file.read(64 * 1024));

    QByteArray sha256 = hash.result();

    file.seek(0);

    m_uploadFile.setFileName(localPath);

    if (!m_uploadFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot reopen file.";
        emit sendFileResult(false, "cannot reopen file",0);
        return;
    }

    QFileInfo info(localPath);

    QMimeDatabase db;

    UploadFileBeginPacket up;

    up.filename = info.fileName();
    up.fileSize = m_uploadFile.size();
    up.mimeType = db.mimeTypeForFile(localPath).name();
    up.sha256 = sha256;

    qDebug()
        << "Beginning upload:"
        << up.filename
        << up.fileSize
        << up.mimeType;

    Packet p;
    p.type = PacketType::UploadFileBegin;
    p.payload = PacketHelpers::pack(up);

    qCInfo(_tcp) << "sending UploadFileBegin to server.";
    socket.write(p.serialize());
}

void User::downloadAttachment(quint64 attachId)
{
    DownloadAttachmentPacket packet;
    packet.attachmentId = attachId;

    Packet p;
    p.type = PacketType::DownloadAttachment;
    p.payload = PacketHelpers::pack(packet);

    qCInfo(_tcp) << "Sending DownloadAttachment request. attachId =" << attachId;
    socket.write(p.serialize());
}

bool User::hasAttachmentImage(quint64 attachmentId) const
{
    return m_attachmentImageProvider->hasImage(attachmentId);
}

QUrl User::attachmentUrl(quint64 id)
{
    QString path = m_attachmentImageProvider->imagePath(id);

    if (path.isEmpty())
        return {};

    return QUrl::fromLocalFile(path);
}


void User::updateChannel(quint64 channelId, const QString &name, const QString &pass, bool saveMessages)
{
    qCInfo(_tcp) << "sending update channel request";
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
    qCInfo(_tcp) << "sending delete channel request";
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

void User::updateApp()
{
    qCInfo(_updater) << "updateApp clicked";
    QString exe = QCoreApplication::applicationDirPath() + "/BeanChatUpdater.exe";
    qCInfo(_updater) << "trying to launch updater, path=" << exe;
    if (!QProcess::startDetached(exe))
    {
        qCCritical(_updater) << "failed to launch updater.";
        return;
    }

    qCInfo(_updater) << "updater launched, lets close self.";
    QCoreApplication::quit();
}

void User::askForServerState()
{
    qCInfo(_tcp) << "asking for server State packet..";
    ServerStatePacket ssp;

    Packet p;
    p.type = PacketType::RequestServerState;
    p.payload = PacketHelpers::pack(ssp);

    socket.write(p.serialize());
}

void User::askForNotFoundAvatars()
{
    if(m_notFoundAvatars.count()>0)
    {
        qCInfo(_tcp) << "asking for not found avatars... not found avatars count=" << m_notFoundAvatars.count();
        RequestAvatarsPacket ra;
        ra.notFoundIds = m_notFoundAvatars;

        Packet p;
        p.type = PacketType::RequestAvatars;
        p.payload = PacketHelpers::pack(ra);

        socket.write(p.serialize());
    }
}

void User::newAvatarArrived(quint64 userId,
                            const QString& avatarHash,
                            const QString& oldAvatarHash,
                            const QByteArray& avatarData)
{
    qCInfo(_tcp) << "Avatars response arrived";

    //check whether that received avatarHash is valid?
    if(avatarHash.isEmpty())
        return;


    if(m_avatarManager.saveAvatar(SAVE_AVATAR_PATH+QString::number(m_connectedServerId_onDb)
                                   ,avatarHash,avatarData))
    {
        qCInfo(_app) << "avatar saved for that user, "
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
            qCInfo(_app) << "avatar path is empty!";
        }

        //check is it server's avatar or not
        if(userId == BeanChatCommon::ReservedIds::ServerAvatar)
        {
            //apply new avatar to myServers model
            if(m_myServersModel->setAvatarPath(avatarPath))
                qCInfo(_app) << "myServersModel avatar updated for that server avatarPath=" << avatarPath;
            else
                qCWarning(_app) << "failed to update avatar for that server on myServersModel avatarPath=" << avatarPath;

            //update datbase for that myServer id. to when didnt connected to servers load their avatar if found.
            QVariantMap values;
            values["avatarPath"] = avatarPath;
            if(m_database->update("MyServers", m_connectedServerId_onDb, values))
                qCInfo(_app) << "server avatarPath updated on myServers' table.";
            else
                qCWarning(_app) << "failed to update avatarPath on myServers' table.";


            return;
        }

        //notify models to update..
        ClientUser* user = m_clientUserManager->user(userId);
        user->setAvatarPath(avatarPath);

        //check if its me, set this to my variable to later load different parts like modifyProfile, userStuff's avatar
        if(user->self())
        {
            setMyAvatarPath(avatarPath);
            qCInfo(_app) << "my avatar has updated successfully.";
            emit notificationRequested(NotificationType::Success,
                                       "Avatar has updated successfully.");
        }

    }
    else
        qCWarning(_app) <<  "failed to save avatar for that user...";
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
            qCInfo(_tcp) <<"WAITING... to packet completes";
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
    qCInfo(_app) << "processPacket for type:" << static_cast<int>(packet.type);
    switch(packet.type)
    {
    case PacketType::IsEverythingsOk:
    {
        qCInfo(_app) << "IsEverythingsOk received";

        Packet p;
        p.type = PacketType::YesEverythingIsOk;

        qCInfo(_tcp) << "sending response YesEverythingIsOk to server.";
        socket.write(p.serialize());
        break;
    }

    case PacketType::LoginChallenge:
    {
        auto challange = PacketHelpers::unpack<LoginPacket>(packet.payload);
        qCInfo(_app) << "login challenge received =" << packet.payload.toBase64();

        const Identity* identity = m_identityManager->currentIdentity();

        if(!identity)
            return;


        LoginPacket proof;

        // proof.payload = BeanChatCommon::Crypto::sign(identity->privateKey, packet.payload);
        QByteArray signature = Crypto::sign(identity->privateKey, challange.payload);
        proof.payload = signature;

        qCInfo(_app) <<  "proofing Signature:" << signature.toBase64();

        Packet p;
        p.type = PacketType::LoginProof;
        p.payload = PacketHelpers::pack(proof);

        qCInfo(_tcp) << "sending proof to server.";
        socket.write(p.serialize());

        break;
    }

    case PacketType::UploadFileBeginResponse:
    {
        auto resp = PacketHelpers::unpack<UploadFileBeginResponsePacket>(packet.payload);

        if (!resp.success)
        {
            qWarning() << "upload file error=" << resp.error;
            emit sendFileResult(false, "upload file failed, "+resp.error,0);
            m_uploadFile.close();

            break;
        }

        m_currentUploadId = resp.uploadId;

        constexpr qint64 ChunkSize = 64 * 1024;

        while (!m_uploadFile.atEnd())
        {
            UploadFileChunkPacket chunk;

            chunk.uploadId = m_currentUploadId;
            chunk.payload = m_uploadFile.read(ChunkSize);

            Packet p;
            p.type = PacketType::UploadFileChunk;
            p.payload = PacketHelpers::pack(chunk);

            qCInfo(_tcp) << "sending UploadFileChunk to server.";
            socket.write(p.serialize());
        }

        UploadFileFinishPacket finish;

        finish.uploadId = m_currentUploadId;

        Packet p;
        p.type = PacketType::UploadFileFinish;
        p.payload = PacketHelpers::pack(finish);

        qCInfo(_tcp) << "sending UploadFileFinish to server.";
        socket.write(p.serialize());

        break;
    }


    case PacketType::DownloadAttachmentBegin:
    {
        auto resp =
            PacketHelpers::unpack<DownloadAttachmentBeginPacket>(
                packet.payload);

        if (!resp.success)
        {
            qCWarning(_tcp) << "Download failed:" << resp.error;
            emit attachmentDownloadFailed(resp.attachmentId, resp.error);
            break;
        }

        auto *session = new DownloadSession;
        session->attachmentId      = resp.attachmentId;
        session->originalFilename  = resp.filename;
        session->mimeType          = resp.mimeType;
        session->expectedSize      = resp.size;
        session->receivedSize      = 0;
        session->sha256            = resp.sha256;

        QString dir =
            QStandardPaths::writableLocation(
                QStandardPaths::AppDataLocation)
            + "/attachments";

        QDir().mkpath(dir);

        // session.file.setFileName(dir + "/" + QString::number(resp.attachmentId));
        session->file = std::make_unique<QFile>(dir + "/" + QString::number(resp.attachmentId) + "_" + resp.filename);

        if (!session->file->open(QIODevice::WriteOnly))
        {
            qCWarning(_app) << "Cannot create file:" << session->file->fileName();
            emit attachmentDownloadFailed(resp.attachmentId, "cannot save file error:"+session->file->fileName());
            break;
        }

        m_downloadSessions.insert(resp.attachmentId, session);

        break;
    }

    case PacketType::DownloadAttachmentChunk:
    {
        auto resp =
            PacketHelpers::unpack<DownloadAttachmentChunkPacket>(
                packet.payload);

        auto it = m_downloadSessions.find(resp.attachmentId);

        if (it == m_downloadSessions.end())
        {
            qCWarning(_tcp) << "Unknown download session.";
            emit attachmentDownloadFailed(resp.attachmentId, "unknown download session");
            break;
        }

        it.value()->file->write(resp.payload);
        it.value()->receivedSize += resp.payload.size();

        emit attachmentDownloadProgress(
            resp.attachmentId,
            it.value()->progress());

        break;
    }

    case PacketType::DownloadAttachmentFinish:
    {
        auto resp =
            PacketHelpers::unpack<DownloadAttachmentFinishPacket>(
                packet.payload);

        auto it = m_downloadSessions.find(resp.attachmentId);

        if (it == m_downloadSessions.end())
        {
            qCWarning(_tcp) << "Unknown download session.";
            emit attachmentDownloadFailed(resp.attachmentId, "unknown download session");
            break;
        }

        it.value()->file->close();

        qCInfo(_tcp) << "Attachment downloaded:"
                     << resp.attachmentId;


        // TODO:
        // verify SHA256 here

        m_attachmentImageProvider->setImagePath(
            resp.attachmentId,
            it.value()->file->fileName());

        emit attachmentDownloaded(resp.attachmentId);

        delete it.value();
        m_downloadSessions.erase(it);
        break;
    }



    case PacketType::UploadFileFinishResponse:
    {
        auto resp =
            PacketHelpers::unpack<
                UploadFileFinishResponsePacket>(
                packet.payload);

        if (!resp.success)
        {
            qWarning() << "upload file result was not succ err=" << resp.error;
            emit sendFileResult(false, "upload file failed, "+resp.error,0);
            m_uploadFile.close();

            break;
        }

        qDebug()
            << "Upload finished."
            << resp.attachmentId;
        emit sendFileResult(true, "upload done, ",resp.attachmentId);

        m_uploadFile.close();
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
                qCInfo(_app) << "a username changed";
                ClientUser* user = m_clientUserManager->user(info.userId);
                if(!user)
                    return;

                user->setUsername(info.payloadValue);
                if(user->self())
                {
                    emit notificationRequested(NotificationType::Info,
                                               "Your username has been updated.");
                    qCInfo(_app) << "Your username has been updated.";
                    setMyUsername(info.payloadValue);
                }

                break;
            }
            case UpdateUserInfoType::Avatar:
            {
                qCInfo(_app) <<  "a avatar changed,  hash="  << info.payloadValue << " avatar size=" << info.payloadData.size();

                //save this new avatar and apply it to models
                newAvatarArrived(info.userId, info.payloadValue, info.payloadSecondValue, info.payloadData);

               break;
            }
            case UpdateUserInfoType::ActivityStatus:
            {
                qCInfo(_app) <<  "a activity status changed";

                ClientUser* user = m_clientUserManager->user(info.userId);
                if(!user)
                    return;

                bool ok=false;
                int val = info.payloadValue.toInt(&ok);
                if(ok)
                {
                    BeanChatCommon::Presence::Status st = static_cast<BeanChatCommon::Presence::Status>(val);

                    //check if status became offline (invisible) decrease connectedUsers
                    if(st==BeanChatCommon::Presence::Status::Offline)
                        setConnectedUsersCount(1,false); //decrease one
                    else if(user->status()==BeanChatCommon::Presence::Status::Offline) //check if user is offline (invisible) now and became something else increase count
                        setConnectedUsersCount(1,true); //increase one

                    user->setStatus(st);
                    if(user->self())
                    {
                        emit notificationRequested(NotificationType::Info,
                                                   "Your activity status has been updated.");
                        qCInfo(_app) << "Your activity status has been updated.";
                        setMyStatus(st);
                    }
                }

                break;
            }
            default:
                qCWarning(_app) << "unkown UpdateUserInfoType received. code=" << static_cast<int>(info.updateType);
        }
    }break;


    case PacketType::ResponseAvatars:
    {
        qCInfo(_app) << "response avatars received lets update users' avatars";

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
        qCInfo(_app) << "a channel updated";
        auto resp =
            PacketHelpers::unpack<ChannelUpdatedPacket>(
                packet.payload);

        m_channelModel->updateChannel(resp.channelId, resp.name, resp.isLocked, resp.saveChats);

        if(resp.channelId == m_myChannelId)
        {
            emit notificationRequested(NotificationType::Info,
                                       "Your channel has been updated.");
            qCInfo(_app) << "your channel has been updated.";
            setMyChannelName(resp.name);
            setMyChannelSavesChat(resp.saveChats);
        }
        break;
    }

    case PacketType::ChannelDeleted:
    {
        qCInfo(_app) << "a channel deleted";
        auto resp =
            PacketHelpers::unpack<DeleteChannelPacket>(
                packet.payload);

        m_channelModel->removeChannel(resp.channelId);

        if(resp.channelId == m_myChannelId)
        {
            emit notificationRequested(NotificationType::Warning,
                                       "Your channel has been deleted.");
            qCInfo(_app) << "your channel has been deleted.";
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
        qCInfo(_app) << "a channel created";
        auto resp =
            PacketHelpers::unpack<ChannelCreatedPacket>(
                packet.payload);

        m_channelModel->addChannel(resp.id,resp.name,resp.isLocked, resp.saveChats);
    }break;

    case PacketType::UserCameraClosed:
    case PacketType::UserCameraOpened:
    {
        qCInfo(_app) << "a camera opened/closeed";;
        auto resp =
            PacketHelpers::unpack<UserStatusChangedPacket>(
                packet.payload);

        if(resp.userId==myId()) //if it's himself update locaol user's variable therefore change icon at userStuff (user mic,speaker,...)
        {
            qCInfo(_app) << "its mine.";
            m_isCameraOpen=resp.status;


            //play sound pack
            if(resp.status)
                emit cameraOpened();
            else
                emit cameraClosed();

            // start or stop camera
            if(m_cam)
            {
                if(resp.status)
                {
                    m_cam->start();


                    //feed this user's vidoeSink with preview.
                    qCInfo(_app) << "show camera preview (feed sink via camera)";
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
                    qCInfo(_app) << "stop showing camera preview (feed sink via camera)";
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
                qCWarning(_app) << "failed to open camera.";
            }


            emit isCameraOpenChanged();
        }
        else
        {
            qCInfo(_app) << "update camera status for user:" << resp.userId << " to " << resp.status;
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
        qCInfo(_app) << "a microphone muted/unmuted to " << resp.status ;

        if(resp.userId==myId()) //if it's himself update locaol user's variable therefore change icon at userStuff (user mic,speaker,...)
        {
            qCInfo(_app) << "it's mine.";
            m_muteMicrophone=resp.status;
            emit muteMicrophoneChanged(); //because we used setMuteMic for send request mute mic.


            //play sound pack
            if(!resp.status)
                emit micOpened();
            else
                emit micClosed();

        }
        else
        {
            qCInfo(_app) <<  "update mute for user:" << resp.userId << " to " << resp.status;
        }

        if (ClientUser *user = m_clientUserManager->user(resp.userId))
            user->setMuted(resp.status);
    }break;


    case PacketType::UserDeafened:
    case PacketType::UserUndeafened:
    {
        qCInfo(_app) << "a headphone deafened/undeafened" ;
        auto resp =
            PacketHelpers::unpack<UserStatusChangedPacket>(
                packet.payload);

        if(resp.userId==myId()) //if it's himself update locaol user's variable therefore change icon at userStuff (user mic,speaker,...)
        {
            qCInfo(_app) << "it's mine.";
            m_muteHeadphone=resp.status;
            emit muteHeadphoneChanged(); //because we used setMuteHeadphone to send request to server.

            //play sound pack
            if(!resp.status)
                emit speakerOpened();
            else
                emit speakerClosed();

        }
        else
        {
            qCInfo(_app) << "update speaker/headphone for user:" << resp.userId << " to " << resp.status;
        }

        if (ClientUser *user = m_clientUserManager->user(resp.userId))
            user->setDeafened(resp.status);
    }break;


    case PacketType::UserMoved:
    case PacketType::UserJoinedChannel:
    {
        qCInfo(_app) << "a user joined/moved to a channel" ;
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
                                           "You were moved.",
                                           NotificationId::YouAreMoved);
                qCInfo(_app) << "you were moved";
                emit youWereMoved();
            }
            else
            {
                qCInfo(_app) << "you switched/join a channel.";
                emit youChannelSwitched();
            }

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
                qCWarning(_app) << "could not find channel to add user into, channel-id:" << resp.channelId;
        }

        //check did user join into my channel?
        else if(resp.channelId == m_myChannelId)
        {
            if(packet.type==PacketType::UserMoved)
                qCInfo(_app) << "user moved into your channel.";
            else
                qCInfo(_app) << "user joined to your channel.";

            //find channel and that user info and add into participantModel
            ChannelItem* channel = m_channelModel->findChannel(resp.channelId);
            if(channel)
            {
                ClientUser* jointUser = m_channelModel->findUserInChannel(channel,resp.userId);
                if(jointUser)
                    if (ClientUser *clientUser = m_clientUserManager->user(jointUser->id()))
                        m_currentChannelParticipant->addUser(clientUser);
                else
                    qCWarning(_app) << "user joined, but could not find user inside channelModel, channel-id:" << resp.channelId << " userid=" << resp.userId;
            }
            else
                qCWarning(_app) << "user joined, but could not find that channel inside channelModel, channel-id::" << resp.channelId;

            //for soundmanager to play effect.
            emit userJoined();
        }

        //check did user left my channel?
        else if(resp.oldChannelId==m_myChannelId)
        {
            if(packet.type==PacketType::UserMoved)
                qCInfo(_app) << "user moved from your channel.";
            else
                qCInfo(_app) << "user left your channel.";


            //rest leaved user talkin status
            if (ClientUser *user = m_clientUserManager->user(resp.userId))
                user->setIsTalking(false);

            //for soundmanager to play effect
            emit userLeft();
        }
        else //user's action is not my concern, no sound effect or additional actions
            qCInfo(_app) << "user (" << resp.userId << ") has left " << resp.oldChannelId << " and joined to " << resp.channelId ;



        //update user's channelId
        if (ClientUser *user = m_clientUserManager->user(resp.userId))
            user->setChannelId(resp.channelId);
        break;
    }

    case PacketType::LoginResponse:
    {
        qCInfo(_app) << "login response received.";
        auto resp =
            PacketHelpers::unpack<LoginResponsePacket>(
                packet.payload);

        if(!resp.accepted)
        {
            emit notificationRequested(NotificationType::Error,
                                       "Connection Rejected. ("+resp.message+")",
                                       NotificationId::ConnectionRejected,
                                       NotificationDuration::Long);
            qCWarning(_app) << "login response said: connection rejected, msg=" << resp.message;
            disconnect();
        }
        else
        {
            setMyId(resp.id); //server just told us our name, to know when e.g: user connected to that channel is that same channel as us? what is my id? so here is it.
            qCInfo(_app) << "login accepted, myId is=" << myId();


            loginToUdpSocket();

            //ask for server channels, users, ...
            askForServerState();
        }

        break;
    }

    case PacketType::UserConnected:
    {
        qCInfo(_app) << "a user connected";
        auto u =
            PacketHelpers::unpack<UserConnectedPacket>(
                packet.payload);

        qCDebug(_app) <<  "User connected:" << u.username << " identity:" << u.identity;

        bool userExists = false;
        //check if user is on offline users (server may have showOfflineUsers ON)
        ClientUser* user= m_clientUserManager->user(u.id);
        if(!user)
        {
            user = m_clientUserManager->createUser(u.id);
            if(!user)
            {
                qCWarning(_app) << "failed to create user, invalid id=" << u.id;
                break;
            }
        }
        else
            userExists=true;

        QString avatarPath = checkAvatar(u.id, u.avatarHash);
        if(avatarPath.isEmpty())
        {
            //send request to server for all not found avatars
            askForNotFoundAvatars();
        }
        qCDebug(_app) << "add user to connected list: " <<  u.appVersion << "-"
                      << u.buildType << "-" << u.osName
                      << "-" << u.osVersion << "- avatar hash= " << u.avatarHash
                      << "avatar path=" << avatarPath << "channelid=";

        //if user is me set myAvatarPath
        if(u.id == myId())
        {
            qCInfo(_app) << "it's me";
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
        user->setStatus(u.status);
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
                qCInfo(_app) <<"relation found, setting for him..";
                user->setRelationship(relationship->relationship);
                user->setNickname(relationship->nickname);
                user->setNote(relationship->note);
                user->setVolume(relationship->voiceVolume);
                user->setLocalMuted(relationship->muted);
            }
            else
            {
                qCInfo(_app) <<"relation not found for this user setting defaults";
                user->setRelationship(Relationship::Type::None);
                user->setNickname({});
                user->setNote({});
                user->setVolume(SPEAKER_DEFAULT_CHANNEL_USERS_VOLUME);
                user->setLocalMuted(false);
            }
        }

        if(!userExists)
        {
            //add him to list.
            m_connectedUsersModel->addUser(user);
        }

        //update connectUsersCount (SERVER INFO)
        if(user->status()!=BeanChatCommon::Presence::Status::Offline)
            setConnectedUsersCount(1,true); //one increased
        break;
    }

    case PacketType::UserConnectionLost:
        // qCInfo(_app) << "a user connection lost:";
    case PacketType::UserDisconnected:
    {
        // qCInfo(_app) << "a user disconnected:";
        auto resp =
            PacketHelpers::unpack<UserDisconnectedPacket>(
                packet.payload);


        qCInfo(_app) << "User disconnected:" << resp.id << " wasConnectinLost?" << resp.wasConnectionLost;

        ChannelItem* channel = m_channelModel->findChannelOfUser(resp.id);
        if(channel)
        {
            //check whether user was on our channel, if was play sound effect
            if(channel->id == m_myChannelId)
            {
                if(resp.wasConnectionLost)
                    emit userTimedOut();
                else
                    emit userLeft(); //play user left channel sound effect
            }
        }
        else
            qCWarning(_app) << "invalid channel id, not found cant find usr was in our channel or not to remove him";


        //set some status for that user. e.g set him offline to show user status as offline on chat
        ClientUser* user = m_clientUserManager->user(resp.id);
        if(user)
        {
            //update connectUsersCount (SERVER INFO) if user wans't invisible
            if(user->status()!=BeanChatCommon::Presence::Status::Offline) //invisbile
                setConnectedUsersCount(1,false); //one decreased

            user->setStatus(BeanChatCommon::Presence::Status::Offline);
        }

        //remove user.
        // m_clientUserManager->removeUser(resp.id);

        //instead removeUser by manager, only remove user from channel and participant
        m_channelModel->removeUser(resp.id);
        m_currentChannelParticipant->removeUser(resp.id);

        break;
    }

    case PacketType::ChatMessage:
    {
        qCInfo(_app) << "a new chat message";
        auto msg = PacketHelpers::unpack<ChatMessagePacket>(packet.payload);

        //show notification dot near chat indicator when chat isn't open
        if(!m_isChatOpen)
            setChatUnreadMessages(chatUnreadMessages()+1); //increase unread messages count

        if(msg.senderId==myId())
        {
            qCInfo(_app) << "message sent successfully";
            // emit messageSent(); //play message sent effect.
        }
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
            // qCInfo(_app) << "new message received";
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
        qCInfo(_app) << "server state received.";
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
                arrived apply avatars for users or in this case if userId==BeanChatCommon::ReservedIds:ServerAvatar \
                would apply it for myServers' avatarPath



        //channels
        m_channelModel->clear();
        qCInfo(_app) << "server channels count= "<< state.channels.count();
        for(auto& c : state.channels)
        {
            m_channelModel->addChannel(
                c.id,
                c.name,
                c.isLocked,
                c.saveChats);
        }

        //users
        qCInfo(_app) << "server users count= "<< state.users.count();
        for(auto& u : state.users)
        {
            //add user.
            ClientUser* user = m_clientUserManager->createUser(u.id);
            if(user)
            {
                QString avatarPath = checkAvatar(u.id, u.avatarHash);

                qCDebug(_app) << "adding user to connected: "
                        <<" id=" << u.id
                        << "identity=" << u.identity
                        << " appVersion="<<  u.appVersion
                        << " buildtype=" << u.buildType
                        << " osName=" << u.osName
                        << " osVersion=" << u.osVersion
                        << " avatar hash= " << u.avatarHash
                        << "avatar path=" << avatarPath
                        << "channelid=" << u.channelId;

                //if user is me set myAvatarPath
                if(u.id == myId())
                {
                    qCInfo(_app) << "this user is me: ";
                    setMyAvatarPath(avatarPath);
                    user->setSelf(true);
                    setMyStatus(u.status);
                }


                user->setUsername(u.username);
                user->setIdentity(u.identity);
                user->setAvatarPath(avatarPath);
                // user->setIconsId(u.icon); //for now packet haven;t this
                user->setMuted(u.muted);
                user->setDeafened(u.deafened);
                user->setChannelId(u.channelId);
                user->setHasCamera(u.camera);
                user->setStatus(u.status);
                user->setAppVersion(u.appVersion);
                user->setBuildType(u.buildType);
                user->setOsName(u.osName);
                user->setOsVersion(u.osVersion);

                //load info about this user. from our UserRelationship database, don't apply for self, (maybe via other idnetities user added currentIdentity so just avoid self)
                if(!user->self())
                {
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

                //when user isn't offline  or invisible update connectUsersCount (SERVER INFO)
                if(user->status()!=BeanChatCommon::Presence::Status::Offline)
                    setConnectedUsersCount(1,true); //one increase

                qCInfo(_app) << "user added to connectedusrs id="<<u.id;

                //if user was in a channel, add him to our channel model
                if(u.channelId!=0)
                    m_channelModel->addUser(u.channelId, user);
                // else
                    // qCInfo(_app) << "user channel id ==0 we dont add him to channel model, he is channel less";

            }
            else
                qCWarning(_app) << "failed to create user, invalid id or user exists. id=" << u.id;

        }

        //send request to server for all not found avatars
        askForNotFoundAvatars();

    }
    break;

    default:
        qCWarning(_app) << "an unknown packetType received. type=" << static_cast<int>(packet.type);
        break;
    }
}

void User::loginToUdpSocket()
{
    //detect if its domain need to lookup
    QHostAddress address;
    if (address.setAddress(m_serverIp))
    {
        m_serverLookedupAddress = address;

        qCInfo(_udp) << "Using IP directly:" << m_serverLookedupAddress.toString();
    }
    else // It's a domain, perform DNS lookup
    {
        qCInfo(_udp) << "using domain.. need to dns lookup..";
        //lookup for domain's ip, udp doesnt do this automatically. but TCP does.
        QHostInfo info = QHostInfo::fromName(m_serverIp);
        if (!info.addresses().isEmpty())
        {
            m_serverLookedupAddress = info.addresses().first();
            qCInfo(_udp) << "Resolved"
                         << m_serverIp
                         << "to"
                         << m_serverLookedupAddress.toString();
        }
        else
        {
            qCCritical(_udp) << "Failed to resolve host:" << m_serverIp;
            emit notificationRequested(NotificationType::Error,"failed to resolve domain");
            disconnect();
            return;
        }
    }


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


    qCInfo(_udp) << "sending udp login request";
    qint64 bytes = m_udpSocket.writeDatagram(
        data,
        m_serverLookedupAddress,
        m_serverPort);

    if (bytes == -1)
    {
        qCInfo(_udp) << "send failed:" << m_udpSocket.errorString();
    }
    else
    {
        qCInfo(_udp) << "sent"
                 << bytes
                 << "bytes to"
                 << m_serverIp
                 << ":"
                 << m_serverPort;
    }
}

quint64 User::connectedUsersCount() const
{
    return m_connectedUsersCount;
}

void User::setConnectedUsersCount(quint64 newConnectedUsersCount)
{
    if (m_connectedUsersCount == newConnectedUsersCount)
        return;
    m_connectedUsersCount = newConnectedUsersCount;
    emit connectedUsersCountChanged();
}

void User::setConnectedUsersCount(quint64 newConnectedUsersCount, bool increase)
{
    if (m_connectedUsersCount == (increase? m_connectedUsersCount+newConnectedUsersCount
                                                                                : m_connectedUsersCount -newConnectedUsersCount))
        return;

    if(increase)
        m_connectedUsersCount += newConnectedUsersCount;
    else
        m_connectedUsersCount -= newConnectedUsersCount;

    emit connectedUsersCountChanged();
}

void User::setMyAppVersion(const QString &newAppVersion)
{
    const QVersionNumber savedVersion = QVersionNumber::fromString(newAppVersion);
    const QVersionNumber buildVersion = QVersionNumber::fromString(APP_VERSION);

    //check if saved version is lower than build version?
    if (savedVersion < buildVersion)
    {
        qCInfo(_app) << "set app version to build's version.";
        m_appVersion = APP_VERSION;

        //also update setting's version
        m_settingsManager->setValue(APP_SETTING_VER, APP_VERSION);
    }
    else
    {
        m_appVersion = newAppVersion;
        qCInfo(_app) << "set app version to qsetting's version.";
    }
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
    qCInfo(_app) << "set my channel id to " << newMyChannelId;
    m_myChannelId = newMyChannelId;
    emit myChannelIdChanged();
}

BeanChatCommon::Presence::Status User::myStatus() const
{
    return m_myStatus;
}

void User::setMyStatus(const Presence::Status &newMyStatus)
{
    if (m_myStatus == newMyStatus)
        return;

    qCDebug(_app) << "set My Activity Status to " << newMyStatus << " and save it to settings";
    m_settingsManager->setValue(USER_SETTING_LAST_ACTIVITY_STATUS, newMyStatus);

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
    qCInfo(_app) << "set connected server id to " << newConnectedServerId;
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
    qCInfo(_app) << "set my avatar path to " << newMyAvatarPath;
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
    qCInfo(_app) << "set mychannel saves chat to " << newMyChannelSavesChat;
    m_myChannelSavesChat = newMyChannelSavesChat;
    emit myChannelSavesChatChanged();
}

QString User::appTitle() const
{
    qCInfo(_app) << "reading appTitle()";
    return QString::fromUtf8(APP_TITLE) + " v" + QString::fromUtf8(APP_VERSION);
}

void User::updateMyActivityStatus(BeanChatCommon::Presence::Status status)
{
    if(isConnectedToServer())
    {
        //send request updaet to server.
        qCInfo(_tcp) << "sending update activity status request";
        UpdateUserInfoPacket uu;

        uu.updateType = UpdateUserInfoType::ActivityStatus;
        uu.payloadValue = QString::number(static_cast<int>(status));

        Packet p;
        p.type = PacketType::UpdateUserInfo;
        p.payload = PacketHelpers::pack(uu);
        socket.write(p.serialize());
    }
    else
    {
        //update local username for upcoming connections.
        setMyStatus(status);
    }

}
void User::updateMyProfile(const QString &username, const QString &avatarPath)
{
    qCInfo(_app) << "try to update my profile, username to " << username
                 << " avatarpath to " << avatarPath;

    //save locally in files of myServers..
    //code here


    //check is username changed?
    if(username != myUsername())
    {
        if(isConnectedToServer())
        {
            //send request updaet to server.
            qCInfo(_tcp) << "sending update username request";
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
        if(isConnectedToServer())
        {
            //send request update to server
            qCInfo(_tcp) << "sending update avatar request";
            UpdateUserInfoPacket uu;
            uu.updateType = UpdateUserInfoType::Avatar;
            uu.paylaodData = m_avatarManager.imageFileToBytes(avatarPath);
            qCInfo(_tcp) << "avatar size=" << uu.paylaodData.size();

            Packet p;

            p.type = PacketType::UpdateUserInfo;
            p.payload = PacketHelpers::pack(uu);
            socket.write(p.serialize());
        }
        else
            qCInfo(_app) << "you are not connected to any server therefore can't send request update avatar.";
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
            qCInfo(_app) << "avatar found = " << avatarPath;
            return avatarPath;
        }

        if(askForAvatar)
        {
            qCInfo(_app) << "avatar not found for  "
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
    qCInfo(_tcp) << "Server disconnected";
    disconnect();
}

void User::onSocketError(QAbstractSocket::SocketError error)
{
    qCWarning(_tcp) << "error:" << error  << " socketError:"<< socket.errorString();

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
    qCInfo(_udp) << "tcp connection has error so force disconnect udp from host.";
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

        switch(static_cast<PacketType>(type))
        {
        case PacketType::UdpLoginResponse:
        {
            qCInfo(_udp) << "udp logged in successfully.";
            emit notificationRequested(NotificationType::Success,
                                       "Connected to "+myServerName());
            emit youConnected();
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
                qCDebug(_udp) << "decode =" << t.nsecsElapsed()/1000000.0 << "ms";
#endif
#if D_PRINT_VOICE_INFO
                qCDebug(_udp) << "received opus=" << pcm.size() << " raw pcm=" << packet.audioData.size();
#endif

                if (!pcm.isEmpty())
                    emit voiceReceived(senderUser->id(),
                                       pcm, senderUser->volume());
            }
#if D_PRINT_AUDIO_INFO
            qCDebug(_udp)
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
                qCWarning(_udp) << "VideoData: Invalid fragment index";
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
            // qCDebug(_udp) << "ping request received.";
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

            // qCDebug(_udp) << "sending pong to server.";
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
    qCDebug(_udp) << "sendVideoFrame:" << videoData.size();
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
        qCDebug(_udp)
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
    qCInfo(_app) << "current identity changed to " << name << " so lets update it in settings too.";
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
    qCInfo(_app) << "reset variables";
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
    setConnectedUsersCount(0);
    m_notFoundAvatars.clear(); //clear list for next connection
    setMyAvatarPath("");
    m_clientUserManager->clear();
    m_channelModel->setCurrentChannelId(0); //set current channel to zeor therefore, stop timer for check user isTalking

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
    // qCDebug(_udp) << "set my video packet loss to " << newMyVideoPacketLoss;
    m_myVideoPacketLoss = newMyVideoPacketLoss;
    emit myVideoPacketLossChanged();
}

void User::initOrLoadSettings()
{
    qCInfo(_app) << "init or load settings.";
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
        qCWarning(_app) << "username not found in config";

    if(m_settingsManager->contains(USER_SETTING_LAST_IDENTITY_NAME))
        m_identityManager->setCurrentIdentity(m_settingsManager->value(USER_SETTING_LAST_IDENTITY_NAME, "").toString());
    else
        qCWarning(_app) << "last identity name not found in config";

    if(m_settingsManager->contains(USER_SETTING_LAST_ACTIVITY_STATUS))
    {
        auto status = static_cast<BeanChatCommon::Presence::Status>(
            m_settingsManager->value(
                                 USER_SETTING_LAST_ACTIVITY_STATUS,
                                 static_cast<int>(BeanChatCommon::Presence::Status::Online)).toInt());
        if (!BeanChatCommon::isValidPresenceStatus(status))
            status = BeanChatCommon::Presence::Status::Online;

        setMyStatus(status);
    }
    else
        setMyStatus(BeanChatCommon::Presence::Status::Online);




    //check for input device exists or not
    if(m_mic->devicesCount() > 0)
    {
        //read from saved settings and convert to valid id,
        int micIndex = m_mic->audioInputIndexFromId(m_settingsManager->value(MIC_SETTING_DEVICE, "").toString());
        if(micIndex != -1)
            m_mic->setCurrentAudioInput(micIndex); //m_soundManager->outputDevice would automatically obey this change
        else
        {
            qCInfo(_app) << "saved audioInput device not found, we did reset it to default.";

            //set to default (first device)
            QString deviceId = m_mic->audioInputId(MIC_DEFUALT_DEVICE_INDEX);
            m_mic->setCurrentAudioInput(m_mic->audioInputIndexFromId(deviceId));
            m_settingsManager->setValue(MIC_SETTING_DEVICE, deviceId);
        }
    }
    else
    {
        m_settingsManager->setValue(MIC_SETTING_DEVICE, "");
        qCWarning(_app) << "ERROR there is no microphone at all.. plug something in";
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
            qCInfo(_app) << "saved audioOutput device not found, we did reset it to default.";
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
        qCInfo(_app) << "ERROR there is no speaker at all.. plug something in";
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
            qCInfo(_app) << "saved cameraInput device not found, we did reset it to default.";

            //set to default (first device)
            QString deviceId = m_cam->cameraIntputId(CAMERA_DEFAULT_DEVICE_INDEX);
            m_cam->setCurrentCameraInput(m_cam->cameraInputIndexFromId(deviceId));
            m_settingsManager->setValue(CAMERA_SETTING_DEVICE, deviceId);
        }
    }
    else
    {
        m_settingsManager->setValue(CAMERA_SETTING_DEVICE, "");
        qCWarning(_app) << "ERROR there is no cameraInput at all.. plug something in";
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
    // qCDebug(_udp) << "set my voice packet loss to " << newMyVoicePacketLoss;
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
    // qCDebug(_udp) << "set my ping to " << newMyPing;
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
    qCDebug(_app) << "set chat unread messages to " << newChatUnreadMessages;
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
    qCDebug(_app) << "set Is Chat Open to " << newIsChatOpen;
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
    //code here, i think this became useless and manage is by m_identityManager.
    qCInfo(_app) << "set my identity to " << newIdentity;
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
    qCDebug(_app) << "set Is Connected To Server to " << newConnectedToServer;
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
    qCDebug(_app) << "set My Server Name to " << newMyServerName;
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
    qCInfo(_tcp) << "sending open/close camera request.";
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
    qCInfo(_tcp) << "sending mute/unmute microphone request.";
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
    qCInfo(_tcp) << "sending deafened/undeafened headphone request.";
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
    qCDebug(_app) << "set My UserName to " << newMyUsername << " and save it to settings";
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
    qCDebug(_app) << "set My Id to " << newMyId;
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

    qCDebug(_app) << "set My Channel Name to " << name;
    m_myChannelName = name;
    emit myChannelNameChanged();
}

