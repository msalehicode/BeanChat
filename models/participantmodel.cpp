#include "participantmodel.h"

#include "logging/loggingcategories.h"

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
    if (!index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_users.size())
    {
        qCCritical(_models) << "invalid index to get participant data";
        return {};
    }


    const ParticipantData &entry = m_users[index.row()];

    if (!entry.user)
    {
        qCCritical(_models) << "Participant has null user";
        return {};
    }

    switch(role)
    {
    case UsernameRole:
        return entry.user->username();

    case UserAvatarPathRole:
        return entry.user->avatarPath();

    case RelationRole:
        return entry.user->relationship();

    case IsTalkingRole:
        return entry.user->isTalking();

    case IsMuted:
        return entry.user->muted();

    case IsLocalMuteRole:
        return entry.user->localMuted();

    case IsDeafened:
        return entry.user->deafened();

    case IsCameraOpenRole:
        return entry.user->hasCamera();

    case UserId:
        return entry.user->id();

    case VideoSinkRole:
        return QVariant::fromValue(entry.videoSink);
    }

    qCWarning(_models) << "participant data, return {}";
    return {};
}

QHash<int, QByteArray> ParticipantModel::roleNames() const
{
    return {
        {UsernameRole,"username"},
        {UserAvatarPathRole, "avatarPath"},
        {IsTalkingRole,"isTalking"},
        {IsCameraOpenRole,"isCameraOpen"},
        {RelationRole, "relation"},
        {VideoSinkRole,"videoSink"},
        {UserId,"userId"},
        {IsLocalMuteRole, "isLocalMuted"},
        {IsDeafened, "isDeafened"},
        {IsMuted, "isMuted"}
    };
}

void ParticipantModel::addUser(ClientUser *user)
{
    if (!user)
    {
        // qCCritical(_models) << "failed to add user to participant, invalid user obj";
        return;
    }

    if (findUser(user->id()))
    {
        // qCCritical(_models) << "failed to add user to participant, user exists";
        return;
    }
    qCInfo(_models) << "add user to participant.";

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
    qCInfo(_models) <<  "remove user from participant, id=" << userId;
    for(int i=0;i<m_users.size();i++)
    {
        if (!m_users[i].user)
            continue;

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

    qCWarning(_models) << "couldn't find user to remove from participant uid=" << userId;
}

void ParticipantModel::clear()
{
    qCInfo(_models) << "clear participantModel";
    beginResetModel();

    for (auto &entry : m_users)
    {
        delete entry.videoSink;
    }

    m_users.clear();
    m_observedUsers.clear();

    endResetModel();
}

void ParticipantModel::clearExcept(quint64 keepUserId)
{
    qCInfo(_models) << "clear participantModel but keep id=" <<keepUserId;
    beginResetModel();

    auto it = m_users.begin();

    while (it != m_users.end())
    {
        if (!it->user)
        {
            delete it->videoSink;
            it = m_users.erase(it);
            continue;
        }
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

    m_observedUsers.clear();

    for (const auto &entry : m_users)
        m_observedUsers.insert(entry.user);

    endResetModel();
}
ClientUser* ParticipantModel::findUser(quint64 id) const
{
    for (auto &entry : m_users)
    {
        if (!entry.user)
            continue;

        if (entry.user->id() == id)
            return entry.user;
    }

    qCWarning(_models) << "couldn't find user id=(" << id << ") in participants.";
    return nullptr;
}

int ParticipantModel::findRow(ClientUser *user) const
{
    if(!user)
        return -1;

    for (int i = 0; i < m_users.size(); ++i)
    {
        if (!m_users[i].user)
            continue;

        if (m_users[i].user == user)
            return i;
    }

    qCWarning(_models) << "couldn't find user id=(" << user->id() << ") in participants.";
    return -1;
}

VideoSink *ParticipantModel::videoSink(quint64 userId) const
{
    for (auto &entry : m_users)
    {
        if (!entry.user)
            continue;

        if (entry.user->id() == userId)
            return entry.videoSink;
    }

    qCWarning(_models) << "couldn't find user id=(" << userId << ") in participants to get videoSink.";
    return nullptr;
}

void ParticipantModel::observeUser(ClientUser *user)
{
    qCInfo(_models) << "add user to observe for participantModel";
    if (!user || m_observedUsers.contains(user))
    {
        qCCritical(_models) << "failed to observe user, invalid user OR user has already exist in observedUsers.";
        return;
    }
    m_observedUsers.insert(user);

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
            &ClientUser::relationshipChanged,
            this,
            [this, user]()
            {
                int row = findRow(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { RelationRole });
            });


    connect(user,
            &ClientUser::localMutedChanged,
            this,
            [this, user]()
            {
                int row = findRow(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { IsLocalMuteRole });
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

