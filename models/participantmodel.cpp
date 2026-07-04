#include "participantmodel.h"

ParticipantModel::ParticipantModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ParticipantModel::rowCount(const QModelIndex &) const
{
    return m_users.size();
}

QVariant ParticipantModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    if (index.row() >= m_users.size())
        return {};

    const ParticipantData &entry = m_users[index.row()];

    switch(role)
    {
    case UsernameRole:
        return entry.user->username();

    case UserAvatarPathRole:
        return entry.user->avatarPath();

    case IsTalkingRole:
        return entry.user->isTalking();

    case IsMuted:
        return entry.user->muted();

    case IsDeafened:
        return entry.user->deafened();

    case IsCameraOpenRole:
        return entry.user->hasCamera();

    case UserId:
        return entry.user->id();

    case VideoSinkRole:
        return QVariant::fromValue(entry.videoSink);
    }

    return {};
}

QHash<int, QByteArray> ParticipantModel::roleNames() const
{
    return {
        {UsernameRole,"username"},
        {UserAvatarPathRole, "avatarPath"},
        {IsTalkingRole,"isTalking"},
        {IsCameraOpenRole,"isCameraOpen"},
        {VideoSinkRole,"videoSink"},
        {UserId,"userId"},
        {IsDeafened, "isDeafened"},
        {IsMuted, "isMuted"}
    };
}

void ParticipantModel::addUser(ClientUser *user)
{
    if (!user)
        return;

    if (findUser(user->id()))
        return;

    ParticipantData entry;
    entry.user = user;
    entry.videoSink = new VideoSink(this);

    observeUser(user);

    beginInsertRows(QModelIndex(),
                    rowCount(),
                    rowCount());
    m_users.append(entry);
    endInsertRows();
}

void ParticipantModel::removeUser(const quint64 &userId)
{
    for(int i=0;i<m_users.size();i++)
    {
        if (m_users[i].user->id() == userId)
        {
            beginRemoveRows(
                QModelIndex(),
                i,
                i);

            delete m_users[i].videoSink;
            m_users.removeAt(i);

            endRemoveRows();
            return;
        }
    }
}

void ParticipantModel::clear()
{
    beginResetModel();

    for (auto &entry : m_users)
        delete entry.videoSink;

    m_users.clear();

    endResetModel();
}

void ParticipantModel::clearExcept(quint64 keepUserId)
{
    beginResetModel();

    auto it = m_users.begin();

    while (it != m_users.end())
    {
        if (it->user->id() == keepUserId)
        {
            ++it;
        }
        else
        {
            delete it->videoSink;
            it = m_users.erase(it);
        }
    }

    endResetModel();
}
ClientUser* ParticipantModel::findUser(quint64 id)
{
    for (auto &entry : m_users)
    {
        if (entry.user->id() == id)
            return entry.user;
    }

    return nullptr;
}

int ParticipantModel::findRow(ClientUser *user) const
{
    for (int i = 0; i < m_users.size(); ++i)
    {
        if (m_users[i].user == user)
            return i;
    }

    return -1;
}

VideoSink *ParticipantModel::videoSink(quint64 userId)
{
    for (auto &entry : m_users)
    {
        if (entry.user->id() == userId)
            return entry.videoSink;
    }

    return nullptr;
}

void ParticipantModel::observeUser(ClientUser *user)
{
    if(!user)
        return;

    connect(user,
            &ClientUser::usernameChanged,
            this,
            [this, user]
            {
                int row = findRow(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UsernameRole });
            });


    connect(user,
            &ClientUser::mutedChanged,
            this,
            [this, user]()
            {

                int row = findRow(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { IsMuted });
            });


    connect(user,
            &ClientUser::deafenedChanged,
            this,
            [this, user]()
            {
                int row = findRow(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { IsDeafened });
            });

    connect(user,
            &ClientUser::idChanged,
            this,
            [this, user]()
            {
                int row = findRow(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UserId });
            });


    connect(user,
            &ClientUser::avatarPathChanged,
            this,
            [this, user]()
            {
                int row = findRow(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UserAvatarPathRole });
            });


    connect(user,
            &ClientUser::hasCameraChanged,
            this,
            [this, user]()
            {
                int row = findRow(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { IsCameraOpenRole });
            });



    connect(user,
            &ClientUser::isTalkingChanged,
            this,
            [this, user]()
            {
                int row = findRow(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { IsTalkingRole });
            });
}

