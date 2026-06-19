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
    m_talkingTimer.start(100);
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

    case UsersRole:
    {
        QVariantList users;

        for(const auto& user :
             channel.users)
        {
            QVariantMap map;

            map["userid"] =
                user.id;

            map["username"] =
                user.username;

            map["muted"] =
                user.muted;

            map["isTalking"] =
                user.isTalking;

            map["deafened"] =
                user.deafened;

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
            { UsersRole, "users" }
        };
}

void ChannelModel::clear()
{
    beginResetModel();

    m_channels.clear();

    endResetModel();
}

void ChannelModel::addChannel(
    quint64 id,
    const QString& name)
{
    beginInsertRows(
        QModelIndex(),
        m_channels.size(),
        m_channels.size());

    ChannelItem channel;

    channel.id = id;
    channel.name = name;

    m_channels.push_back(
        channel);

    endInsertRows();
}

void ChannelModel::addUser(
    quint64 channelId,
    quint64 userId,
    const QString& username,
    bool muted,
    bool deafened)
{
    auto channel =
        findChannel(channelId);

    if(!channel)
        return;

    UserItem user;

    user.id = userId;
    user.username = username;

    user.muted = muted;
    user.deafened = deafened;

    channel->users.push_back(
        user);

    int row =
        &(*channel) - m_channels.data();

    emit dataChanged(
        index(row),
        index(row));
}

void ChannelModel::updateUserStatus(quint64 userId, bool isTalking, bool isMuted, bool isDefened)
{
    auto channel = findChannelOfUser(userId);

    if(!channel)
        return;


    UserItem* user = findUserInChannel(channel,userId);
    if(!user)
        return;

    user->isTalking = isTalking;
    user->muted = isMuted;
    user->deafened = isDefened;


    int row =
        &(*channel) - m_channels.data();

    emit dataChanged(
        index(row),
        index(row));
}


UserItem *ChannelModel::getUser(quint64 channelId, quint64 userId)
{
    ChannelItem* channel = findChannel(channelId);
    return findUserInChannel(channel,userId);
}

QString ChannelModel::getChannelName(quint64 channelId)
{
    ChannelItem* channel = findChannel(channelId);
    if(channel)
        return channel->name;
    return "";
}

void ChannelModel::removeUser(
    quint64 userId)
{
    auto channel =
        findChannelOfUser(
            userId);

    if(!channel)
        return;

    for(int i=0;
         i<channel->users.size();
         ++i)
    {
        if(channel->users[i].id
            == userId)
        {
            channel->users.removeAt(i);
            break;
        }
    }

    int row =
        &(*channel) - m_channels.data();

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

    for(auto& channel :
         m_channels)
    {
        for(int i=0;
             i<channel.users.size();
             ++i)
        {
            if(channel.users[i].id
                == userId)
            {
                user =
                    channel.users[i];

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
        if(user.isTalking && user.lastVoicePacket.elapsed() > 100)
        {
            updateUserStatus(
                user.id,
                false,
                user.muted,
                user.deafened);
        }
    }

}

ChannelItem*
ChannelModel::findChannel(
    quint64 id)
{
    for(auto& channel :
         m_channels)
    {
        if(channel.id == id)
            return &channel;
    }

    return nullptr;
}

ChannelItem*
ChannelModel::findChannelOfUser(
    quint64 userId)
{
    for(auto& channel :
         m_channels)
    {
        for(const auto& user :
             channel.users)
        {
            if(user.id == userId)
                return &channel;
        }
    }

    return nullptr;
}

UserItem *ChannelModel::findUserInChannel(ChannelItem* channel, quint64 userId)
{
    for(UserItem& usr : channel->users)
    {
        if(usr.id == userId)
            return &usr;
    }
    return nullptr;
}

void ChannelModel::setCurrentChannelId(quint64 channelId)
{
    m_currentChannelId = channelId;
}
