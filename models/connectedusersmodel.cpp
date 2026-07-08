#include "connectedusersmodel.h"

ConnectedUsersModel::ConnectedUsersModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ConnectedUsersModel::rowCount(
    const QModelIndex &) const
{
    return m_connectedUsers.size();
}

int ConnectedUsersModel::count() const
{
    return m_connectedUsers.size();
}

QVariant ConnectedUsersModel::data(
    const QModelIndex &index,
    int role) const
{
    if(!index.isValid())
        return {};

    ClientUser *user = m_connectedUsers.at(index.row());
    if(!user)
        return{};

    switch(role)
    {
    case UserIdRole:
        return user->id();

    case UserNameRole:
        return user->username();

    case UserAvatarPathRole:
        return user->avatarPath();

    case UserStatusRole:
        return static_cast<int>(user->status());

    case UserRelationRole:
        return static_cast<int>(user->relationship());

    case UserIconsRole:
        return user->iconsId();

    case UserOsVersionRole:
        return user->osVersion();

    case USerOsNameRole:
        return user->osName();

    case UserAppVersionRole:
        return user->appVersion();

    case UserAppBuildTypeRole:
        return user->buildType();

    }

    return {};
}

QHash<int,QByteArray>
ConnectedUsersModel::roleNames() const
{
    return
        {
            { UserIdRole, "userId" },
            { UserNameRole, "userName" },
            { UserAvatarPathRole, "userAvatarPath"},
            { UserStatusRole, "userStatus" },
            { UserRelationRole, "userRelation"},
            { UserIconsRole, "userIcons"},
            { UserOsVersionRole, "userOsVersion"},
            { USerOsNameRole, "userOsName"},
            { UserAppVersionRole, "userAppVersion"},
            { UserAppBuildTypeRole, "userAppBuildType" }
        };
}

void ConnectedUsersModel::clear()
{
    beginResetModel();

    m_connectedUsers.clear();
    m_observedUsers.clear();

    endResetModel();

    emit countChanged();
}

void ConnectedUsersModel::addUser(ClientUser* user)
{
    if(!user)
    {
        qDebug() << "fialed to add user to connected users model. invalid user";
        return;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    m_connectedUsers.append(user);

    endInsertRows();

    emit countChanged();

    observeUser(user);
}

void ConnectedUsersModel::observeUser(ClientUser* user)
{
    if (!user || m_observedUsers.contains(user))
        return;
    m_observedUsers.insert(user);

    connect(user,
            &ClientUser::idChanged,
            this,
            [this, user]()
            {
                int row = m_connectedUsers.indexOf(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UserIdRole });
            });

    connect(user,
            &ClientUser::usernameChanged,
            this,
            [this, user]()
            {
                int row = m_connectedUsers.indexOf(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UserNameRole });
            });


    connect(user,
            &ClientUser::relationshipChanged,
            this,
            [this, user]()
            {
                int row = m_connectedUsers.indexOf(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UserRelationRole });
            });


    connect(user,
            &ClientUser::avatarPathChanged,
            this,
            [this, user]()
            {
                int row = m_connectedUsers.indexOf(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UserAvatarPathRole });
            });

    connect(user,
            &ClientUser::statusChanged,
            this,
            [this, user]()
            {
                int row = m_connectedUsers.indexOf(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UserStatusRole });
            });

    connect(user,
            &ClientUser::iconsIdChanged,
            this,
            [this, user]()
            {
                int row = m_connectedUsers.indexOf(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UserIconsRole });
            });

    connect(user,
            &ClientUser::osVersionChanged,
            this,
            [this, user]()
            {
                int row = m_connectedUsers.indexOf(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UserOsVersionRole });
            });

    connect(user,
            &ClientUser::osNameChanged,
            this,
            [this, user]()
            {
                int row = m_connectedUsers.indexOf(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { USerOsNameRole });
            });

    connect(user,
            &ClientUser::appVersionChanged,
            this,
            [this, user]()
            {
                int row = m_connectedUsers.indexOf(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UserAppVersionRole });
            });


    connect(user,
            &ClientUser::buildTypeChanged,
            this,
            [this, user]()
            {
                int row = m_connectedUsers.indexOf(user);

                if (row >= 0)
                    emit dataChanged(index(row), index(row), { UserAppBuildTypeRole });
            });

}


void ConnectedUsersModel::removeUser(quint64 userId)
{
    for (int row = 0; row < m_connectedUsers.size(); ++row)
    {
        if (m_connectedUsers[row]->id() == userId)
        {
            beginRemoveRows(QModelIndex(), row, row);


            m_connectedUsers.removeAt(row);

            endRemoveRows();
            emit countChanged();
            return;
        }
    }
}

int ConnectedUsersModel::findRowById(quint64 userId) const
{
    for (int i = 0; i < m_connectedUsers.size(); ++i)
    {
        if (m_connectedUsers[i]->id() == userId)
            return i;
    }

    return -1;
}


ClientUser* ConnectedUsersModel::findUser(quint64 userId)
{
    int row = findRowById(userId);

    if (row < 0)
        return nullptr;

    return m_connectedUsers[row];
}

