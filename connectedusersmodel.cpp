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

    const auto& usr =
        m_connectedUsers[index.row()];

    switch(role)
    {
    case UserIdRole:
        return usr.id;

    case UserNameRole:
        return usr.username;

    case UserStatusRole:
        return static_cast<int>(usr.status);

    case UserIconsRole:
        return usr.iconsId;

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
            { UserStatusRole, "userStatus" },
            { UserIconsRole, "userIcons"}
        };
}

void ConnectedUsersModel::clear()
{
    beginResetModel();

    m_connectedUsers.clear();

    endResetModel();
}

void ConnectedUsersModel::addUser(quint64 id, QString username, QString iconsId,
                                  bool talking, bool muted, bool deafened, bool camera,
                                  UserActivityStatus status)
{
    beginInsertRows(
        QModelIndex(),
        rowCount(),
        rowCount());

    ConnectedUser user;
    user.id=id;
    user.username=username;
    user.iconsId=iconsId;
    user.isTalking=talking;
    user.muted=muted;
    user.deafened=deafened;
    user.hasVideo=camera;
    user.status=status;


    m_connectedUsers.append(user);

    endInsertRows();

    emit countChanged();
}

void ConnectedUsersModel::removeUser(quint64 userId)
{
    for (int row = 0; row < m_connectedUsers.size(); ++row)
    {
        if (m_connectedUsers[row].id == userId)
        {
            beginRemoveRows(QModelIndex(), row, row);

            m_connectedUsers.removeAt(row);

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

    m_connectedUsers[row].username = username;

    emit dataChanged(
        index(row),
        index(row),
        { UserNameRole });
}

int ConnectedUsersModel::findRowById(quint64 userId) const
{
    for (int i = 0; i < m_connectedUsers.size(); ++i)
    {
        if (m_connectedUsers[i].id == userId)
            return i;
    }

    return -1;
}


ConnectedUser* ConnectedUsersModel::findUser(quint64 userId)
{
    int row = findRowById(userId);

    if (row < 0)
        return nullptr;

    return &m_connectedUsers[row];
}


void ConnectedUsersModel::setStatus(
    quint64 userId,
    UserActivityStatus status)
{
    int row = findRowById(userId);

    if (row < 0)
        return;

    m_connectedUsers[row].status = status;

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

    m_connectedUsers[row].iconsId = iconsId;

    emit dataChanged(
        index(row),
        index(row),
        { UserIconsRole });
}
