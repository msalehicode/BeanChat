#include "channelmodel.h"

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
    m_talkingTimer.start(CHANNEL_MODEL_TALKING_TIMER_INTERAVL);
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
    if(!index.isValid())
        return {};

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

            map["hasVideo"] =
                user.user->hasCamera();

            users.push_back(map);
        }

        return users;
    }
    }

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
    beginResetModel();

    m_channels.clear();
    m_observedUsers.clear();

    endResetModel();
}

void ChannelModel::addChannel(
    quint64 id,
    const QString& name, bool isLocked, bool saveChat)
{
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
        return;

    UserItem item;
    item.user = user;

    if (findUserInChannel(channel, user->id()))
        return;

    channel->users.append(item);

    observeUser(user);

    int row = findRow(channel);
    emit dataChanged(index(row), index(row), { UsersRole });
}


int ChannelModel::findRow(ChannelItem *channel) const
{
    if (!channel)
        return -1;

    return channel - m_channels.data();
}

int ChannelModel::findRow(ClientUser *user) const
{
    for (int row = 0; row < m_channels.size(); ++row)
    {
        for (const UserItem &item : m_channels[row].users)
        {
            if (item.user == user)
                return row;
        }
    }

    return -1;
}

void ChannelModel::observeUser(ClientUser *user)
{
    if (!user || m_observedUsers.contains(user))
        return;
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
}

void ChannelModel::removeChannel(quint64 channelId)
{
    for (int row = 0; row < m_channels.size(); ++row)
    {
        if (m_channels[row].id != channelId)
            continue;

        beginRemoveRows({}, row, row);

        //delete channel.
        m_channels.removeAt(row);
        endRemoveRows();

        if (m_currentChannelId == channelId)
            m_currentChannelId = 0;

        return;
    }
}


void ChannelModel::resetChannelTalkingStatus(quint64 channelId)
{
    ChannelItem* channel = findChannel(channelId);
    if(!channel)
        return;

    for(auto& user : channel->users)
    {
        user.user->setIsTalking(false);
    }
}



ClientUser *ChannelModel::getUser(quint64 channelId, quint64 userId)
{
    ChannelItem* channel = findChannel(channelId);
    if(channel)
        return findUserInChannel(channel,userId);
    return nullptr;
}

QString ChannelModel::getChannelName(quint64 channelId)
{
    ChannelItem* channel = findChannel(channelId);
    if(channel)
        return channel->name;
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
    auto channel = findChannelOfUser(userId);

    if(!channel)
        return;

    for(int i=0; i<channel->users.size(); ++i)
    {
        if (channel->users[i].user->id() == userId)
        {
            channel->users.removeAt(i);
            break;
        }
    }

    int row = &(*channel) - m_channels.data();

    emit dataChanged(
        index(row),
        index(row));
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
        return;

    auto newChannel =
        findChannel(
            newChannelId);

    if(!newChannel)
        return;

    newChannel->users.push_back(
        user);

    emit dataChanged(
        index(0),
        index(m_channels.size()-1));
}

void ChannelModel::updateTalkingUsers()
{
    //update that channel users' talking status on that currentChannel*
    ChannelItem* channel = findChannel(m_currentChannelId);

    if(!channel)
        return;

    for(auto& user : channel->users)
    {
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

    return nullptr;
}

ChannelItem* ChannelModel::findChannelOfUser(quint64 userId)
{
    for(auto& channel : m_channels)
    {
        for(const auto& user : channel.users)
        {
            if (user.user->id() == userId)
                return &channel;
        }
    }

    return nullptr;
}

ClientUser *ChannelModel::findUserInChannel(ChannelItem* channel, quint64 userId)
{
    for(UserItem& usr : channel->users)
    {
        if (usr.user->id() == userId)
            return usr.user;
    }
    return nullptr;
}

void ChannelModel::restartVoiceTimer(quint64 userId)
{
    if (UserItem *item = findUserItem(userId))
        item->lastVoicePacket.restart();
}

UserItem *ChannelModel::findUserItem(quint64 userId)
{
    for (ChannelItem &channel : m_channels)
    {
        for (UserItem &item : channel.users)
        {
            if (item.user->id() == userId)
                return &item;
        }
    }

    return nullptr;
}

void ChannelModel::setCurrentChannelId(quint64 channelId)
{
    m_currentChannelId = channelId;
}
