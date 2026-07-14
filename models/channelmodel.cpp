#include "channelmodel.h"
#include "logging/loggingcategories.h"

ChannelModel::ChannelModel(
    QObject* parent)
    :
    QAbstractListModel(parent)
{
    //check and update talking status for current channel users.
    connect(
        &m_talkingTimer,
        &QTimer::timeout,
        this,
        &ChannelModel::updateTalkingUsers);
}

int ChannelModel::rowCount(
    const QModelIndex&) const
{
    return m_channels.size();
}

QVariant ChannelModel::data(
    const QModelIndex& index,
    int role) const
{
    if (!index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_channels.size())
    {
        qCCritical(_models) << "invalid index to get channel data";
        return {};
    }

    const auto& channel =
        m_channels[index.row()];

    switch(role)
    {
    case IdRole:
        return channel.id;

    case NameRole:
        return channel.name;

    case SaveChatsRole:
        return channel.saveChats;

    case IsLcokedRole:
        return channel.isLocked;

    case UsersRole:
    {
        QVariantList users;

        for(const auto& user :
             channel.users)
        {

            if(!user.user)
                continue;

            QVariantMap map;

            map["userid"] =
                user.user->id();

            map["username"] =
                user.user->username();

            map["avatarPath"] =
                user.user->avatarPath();

            map["muted"] =
                user.user->muted();

            map["isTalking"] =
                user.user->isTalking();

            map["deafened"] =
                user.user->deafened();

            map["relationship"] =
                user.user->relationship();

            map["isLocalMuted"] =
                user.user->localMuted();

            map["hasVideo"] =
                user.user->hasCamera();

            users.push_back(map);
        }

        return users;
    }
    }

    qCWarning(_models) << "channel data, return {}";
    return {};
}

QHash<int,QByteArray>
ChannelModel::roleNames() const
{
    return
        {
            { IdRole, "channelId" },
            { NameRole, "channelName" },
            { SaveChatsRole, "saveChats"},
            { IsLcokedRole, "isLocked"},
            { UsersRole, "users" }
        };
}

void ChannelModel::clear()
{
    qCInfo(_models) << "clear channelModel";
    beginResetModel();

    m_channels.clear();
    m_observedUsers.clear();

    endResetModel();
}

void ChannelModel::addChannel(
    quint64 id,
    const QString& name, bool isLocked, bool saveChat)
{
    qCInfo(_models) << "add channel, channel id=" << id;
    beginInsertRows(
        QModelIndex(),
        m_channels.size(),
        m_channels.size());

    ChannelItem channel;

    channel.id = id;
    channel.name = name;
    channel.isLocked = isLocked;
    channel.saveChats = saveChat;

    m_channels.push_back(
        channel);

    endInsertRows();
}

void ChannelModel::addUser(quint64 channelId, ClientUser *user)
{
    ChannelItem *channel = findChannel(channelId);


    if (!channel || !user)
    {
        qCCritical(_models) << " add user to channel failed, invalid user OR channel not found.";
        return;
    }
    qCCritical(_models)
        << "ADD user to channel"
        << user
        << user->id();

    qCInfo(_models) << "add user to channel, target channel id=" << channelId;

    UserItem item;
    item.user = user;

    if (findUserInChannel(channel, user->id()))
    {
        qCCritical(_models) << "add user to channel failed, user's already in that channel";
        return;
    }

    channel->users.append(item);

    observeUser(user);

    int row = findRow(channel);
    emit dataChanged(index(row), index(row), { UsersRole });
}


int ChannelModel::findRow(ChannelItem *channel) const
{
    if (!channel)
    {
        qCCritical(_models) << "failed to find channel row invalid channel object.";
        return -1;
    }

    return channel - m_channels.data();
}

int ChannelModel::findRow(ClientUser *user) const
{
    if(!user)
    {
        qCCritical(_models) << "failed to find user row invalid user object.";
        return -1;
    }

    for (int row = 0; row < m_channels.size(); ++row)
    {
        for (const UserItem &item : m_channels[row].users)
        {
            if (item.user == user)
                return row;
        }
    }

    qCWarning(_models) << "couldn't find user id=(" << user->id() << ") in channels.";
    return -1;
}

void ChannelModel::observeUser(ClientUser *user)
{
    if (!user || m_observedUsers.contains(user))
    {
        // qCCritical(_models) << "failed to observe user, invalid user OR user has already exist in observedUsers.";
        return;
    }
    qCInfo(_models) << "add user to observe for channelModel";
    m_observedUsers.insert(user);


    auto updateRoles = [this, user]()
    {
        int row = findRow(user);

        if (row >= 0)
            emit dataChanged(index(row), index(row), { UsersRole });
    };
    connect(user,
            &ClientUser::idChanged,
            this,
            updateRoles);

    connect(user,
            &ClientUser::usernameChanged,
            this,
            updateRoles);

    connect(user,
            &ClientUser::avatarPathChanged,
            this,
            updateRoles);

    connect(user,
            &ClientUser::localMutedChanged,
            this,
            updateRoles);

    connect(user,
            &ClientUser::relationshipChanged,
            this,
            updateRoles);

    connect(user,
            &ClientUser::mutedChanged,
            this,
            updateRoles);

    connect(user,
            &ClientUser::deafenedChanged,
            this,
            updateRoles);

    connect(user,
            &ClientUser::hasCameraChanged,
            this,
            updateRoles);

    connect(user,
            &ClientUser::isTalkingChanged,
            this,
            updateRoles);
}


void ChannelModel::updateChannel(quint64 id, const QString &name, bool isLocked, bool saveChats)
{
    qCInfo(_models) << "update channel, target id=" << id;
    for (int row = 0; row < m_channels.size(); ++row)
    {
        ChannelItem &channel = m_channels[row];

        if (channel.id == id)
        {
            channel.name = name;
            channel.isLocked = isLocked;
            channel.saveChats = saveChats;

            QModelIndex index = createIndex(row, 0);
            emit dataChanged(index, index,
                             {
                                 NameRole,
                                 IsLcokedRole,
                                 SaveChatsRole
                             });

            return;
        }
    }
    qCWarning(_models) << "couldn't find channel to update, target id=" << id;
}

void ChannelModel::removeChannel(quint64 channelId)
{
    qCInfo(_models) << "remove channel, target id=" << channelId;
    for (int row = 0; row < m_channels.size(); ++row)
    {
        if (m_channels[row].id != channelId)
            continue;

        beginRemoveRows({}, row, row);

        //delete channel.
        m_channels.removeAt(row);
        endRemoveRows();

        //check if living channel remove/deleted stop check for talking
        if (m_currentChannelId == channelId)
        {
            setCurrentChannelId(0); //set to non and stop timer for check isTalking
        }

        return;
    }
    qCWarning(_models) << "couldn't find channel to remove, target id=" << channelId;
}


void ChannelModel::resetChannelTalkingStatus(quint64 channelId)
{
    qCInfo(_models) << "reset channel talking status, target id=" << channelId;
    ChannelItem* channel = findChannel(channelId);
    if(!channel)
    {
        qCCritical(_models) << "failed to reset channel talking status, invalid channel object";
        return;
    }

    for(auto& user : channel->users)
    {
        if (!user.user)
            continue;

        user.user->setIsTalking(false);
    }
}



ClientUser *ChannelModel::getUser(quint64 channelId, quint64 userId)
{
    ChannelItem* channel = findChannel(channelId);
    if(channel)
        return findUserInChannel(channel,userId);

    qCWarning(_models) << "returning nullptr, couldn't find user to return, channel id= " << channelId << " user id=" << userId;
    return nullptr;
}

QString ChannelModel::getChannelName(quint64 channelId)
{
    ChannelItem* channel = findChannel(channelId);
    if(channel)
        return channel->name;

    qCWarning(_models) << "returning empty channel name, couldn't find channel to return channel name, target id= " <<channelId;
    return "";
}

bool ChannelModel::getChannelSaveChats(quint64 channelId)
{
    ChannelItem* channel = findChannel(channelId);
    return channel ? channel->saveChats : false;
}



void ChannelModel::removeUser(
    quint64 userId)
{
    qCCritical(_models)
    << "REMOVE user from channel"
    << userId;

    auto channel = findChannelOfUser(userId);

    if(!channel)
    {
        qCCritical(_models) << "failed to remove user from channel, invalid channel object, userid=" << userId;
        return;
    }

    for(int i=0; i<channel->users.size(); ++i)
    {
        if (!channel->users[i].user)
            continue;

        if (channel->users[i].user->id() == userId)
        {
            channel->users.removeAt(i);
            break;
        }
    }

    int row = &(*channel) - m_channels.data();

    emit dataChanged(index(row),
                     index(row),
                     { UsersRole });
}

void ChannelModel::moveUser(
    quint64 userId,
    quint64 newChannelId)
{
    UserItem user;

    bool found = false;

    for(auto& channel : m_channels)
    {
        for(int i=0; i<channel.users.size(); ++i)
        {
            if (!channel.users[i].user)
                continue;

            if(channel.users[i].user->id() == userId)
            {
                user = channel.users[i];

                channel.users.removeAt(i);

                found = true;

                break;
            }
        }

        if(found)
            break;
    }

    if(!found)
    {
        qCCritical(_models) <<  "move user to channel failed, user id= "<< userId << " not found in any channel to move.";
        return;
    }

    auto newChannel =
        findChannel(
            newChannelId);

    if(!newChannel)
    {
        qCCritical(_models) <<  "move user to channel failed, target channel id= "<< newChannelId << " not found.";
        return;
    }

    newChannel->users.push_back(
        user);

    if (!m_channels.isEmpty())
    {
        emit dataChanged(
            index(0),
            index(m_channels.size()-1));
    }
}

void ChannelModel::updateTalkingUsers()
{
    //update that channel users' talking status on that currentChannel*
    ChannelItem* channel = findChannel(m_currentChannelId);

    if(!channel)
    {
        qCCritical(_models) << "failed to update talking users, invalid channel obj, current channel=" << m_currentChannelId;
        return;
    }

    for(auto& user : channel->users)
    {
        // if (!user.user)
        // {
        //     qCWarning(_models) << "failed to update user's talking status, invalid user obj in current channel";
        //     continue;
        // }
        if (!user.user)
        {
            qCCritical(_models)
            << "NULL user!"
            << "channel =" << channel->id
            << "index =" << (&user - channel->users.data())
            << "users =" << channel->users.size();

            continue;
        }

        if (user.user->isTalking() && user.lastVoicePacket.elapsed() > CHANNEL_MODEL_TALKING_TIMEOUT)
        {
            user.user->setIsTalking(false);
            emit userTalkingStatus(user.user->id(),false);
        }
    }

}

ChannelItem* ChannelModel::findChannel(quint64 id)
{
    for(auto& channel : m_channels)
    {
        if(channel.id == id)
            return &channel;
    }

    qCWarning(_models) << "returning nullptr, failed to find channel, channel id=" << id<< " not found";
    return nullptr;
}

ChannelItem* ChannelModel::findChannelOfUser(quint64 userId)
{
    for(auto& channel : m_channels)
    {
        for(const auto& user : channel.users)
        {
            if (!user.user)
                continue;

            if (user.user->id() == userId)
                return &channel;
        }
    }

    qCWarning(_models) << "returning nullptr, failed to find channel of user, target user id= " <<userId;
    return nullptr;
}

ClientUser *ChannelModel::findUserInChannel(ChannelItem* channel, quint64 userId)
{
    if(!channel)
    {
        qCCritical(_models) << "returning nullptr, failed to find user in channel, invalid channel object";
        return nullptr;
    }
    for(UserItem& usr : channel->users)
    {
        if (!usr.user)
            continue;

        if (usr.user->id() == userId)
            return usr.user;
    }

    qCWarning(_models) << "returning nullptr, failed to find user in channel, target user ="<<userId;
    return nullptr;
}

void ChannelModel::restartVoiceTimer(quint64 userId)
{
    if (UserItem *item = findUserItem(userId))
        item->lastVoicePacket.restart();
}

void ChannelModel::setTimerChannelTalkingStatus(bool status)
{
    if(status)
        m_talkingTimer.start(CHANNEL_MODEL_TALKING_TIMER_INTERAVL);
    else
        m_talkingTimer.stop();
}

UserItem *ChannelModel::findUserItem(quint64 userId)
{
    for (ChannelItem &channel : m_channels)
    {
        for (UserItem &item : channel.users)
        {
            if (!item.user)
                continue;

            if (item.user->id() == userId)
                return &item;
        }
    }
    qCWarning(_models) <<  "returning nullptr, failed to find user in any channel, target user id=" << userId;
    return nullptr;
}

void ChannelModel::setCurrentChannelId(quint64 channelId)
{
    m_currentChannelId = channelId;
    qCInfo(_models) << "set current channel id to " << channelId;
    if(channelId==0)
        setTimerChannelTalkingStatus(false); //stop check for talking status
    else
        setTimerChannelTalkingStatus(true); //start check for talking status
}
