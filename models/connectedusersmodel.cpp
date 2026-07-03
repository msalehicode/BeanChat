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

    qDeleteAll(m_connectedUsers);
    m_connectedUsers.clear();

    endResetModel();

    emit countChanged();
}

void ConnectedUsersModel::addUser(quint64 id,
                                  QString username,
                                  QString avatarPath,
                                  QString iconsId,
                                  bool talking,
                                  bool muted,
                                  bool deafened,
                                  bool camera,
                                  QString version,
                                  QString buildType,
                                  QString osName,
                                  QString osVersion,
                                  ClientUser::Status status)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    ClientUser *user = new ClientUser(this);

    user->setId(id);
    user->setUsername(username);
    user->setAvatarPath(avatarPath);
    user->setIconsId(iconsId);
    user->setIsTalking(talking);
    user->setMuted(muted);
    user->setDeafened(deafened);
    user->setHasCamera(camera);
    user->setStatus(status);
    user->setAppVersion(version);
    user->setBuildType(buildType);
    user->setOsName(osName);
    user->setOsVersion(osVersion);

    m_connectedUsers.append(user);

    endInsertRows();

    emit countChanged();
}


void ConnectedUsersModel::setUserAvatarPath(quint64 userId, const QString &path)
{
    int row = findRowById(userId);

    if (row < 0)
        return;

    m_connectedUsers[row]->setAvatarPath(path);

    emit dataChanged(
        index(row),
        index(row),
        { UserAvatarPathRole });
}

void ConnectedUsersModel::removeUser(quint64 userId)
{
    for (int row = 0; row < m_connectedUsers.size(); ++row)
    {
        if (m_connectedUsers[row]->id() == userId)
        {
            beginRemoveRows(QModelIndex(), row, row);

            ClientUser *user = m_connectedUsers.takeAt(row);

            user->deleteLater();

            endRemoveRows();
            emit countChanged();
            return;
        }
    }
}

void ConnectedUsersModel::setUsername(
    quint64 userId,
    const QString& username)
{
    int row = findRowById(userId);

    if (row < 0)
        return;

    m_connectedUsers[row]->setUsername(username);

    emit dataChanged(
        index(row),
        index(row),
        { UserNameRole });
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


void ConnectedUsersModel::setStatus(
    quint64 userId,
    ClientUser::Status status)
{
    int row = findRowById(userId);

    if (row < 0)
        return;

    m_connectedUsers[row]->setStatus(status);

    emit dataChanged(
        index(row),
        index(row),
        { UserStatusRole });
}

void ConnectedUsersModel::setIcons(
    quint64 userId,
    const QString& iconsId)
{
    int row = findRowById(userId);

    if (row < 0)
        return;

    m_connectedUsers[row]->setIconsId(iconsId);

    emit dataChanged(
        index(row),
        index(row),
        { UserIconsRole });
}
