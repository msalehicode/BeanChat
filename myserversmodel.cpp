#include "myserversmodel.h"

MyServersModel::MyServersModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int MyServersModel::rowCount(
    const QModelIndex &) const
{
    return m_servers.size();
}

QVariant MyServersModel::data(
    const QModelIndex &index,
    int role) const
{
    if(!index.isValid())
        return {};

    const auto& server =
        m_servers[index.row()];

    switch(role)
    {
    case ServerIdRole:
        return server.id;

    case ServerIndexRole:
        return server.index;

    case ServerIpRole:
        return server.ip;

    case ServerIsActiveRole:
        return server.isActive;

    case ServerPortRole:
        return server.port;

    case ServerNameRole:
        return server.name;

    case ServerStatusRole:
        return static_cast<int>(server.status);
    }

    return {};
}

QHash<int,QByteArray>
MyServersModel::roleNames() const
{
    return
        {
            { ServerIdRole, "id" },
            { ServerIndexRole, "dbIndex"},
            { ServerIpRole, "ip" },
            { ServerPortRole, "port" },
            { ServerIsActiveRole, "isActive"},
            { ServerNameRole, "name"},
            { ServerStatusRole, "status"}
        };
}

void MyServersModel::clear()
{
    beginResetModel();

    m_servers.clear();

    endResetModel();
}

void MyServersModel::addServer(QString name,
                               const QString& ip,
                               const QString& port,
                               bool isActive,
                               quint64 index)
{
    beginInsertRows(QModelIndex(),
                    rowCount(),
                    rowCount());

    ServerInfo server;
    server.id = m_nextServerId++;
    server.index = index;
    server.name = name;
    server.ip = ip;
    server.port = port;


    m_servers.append(server);

    if(isActive)
        setIsActive(server.id);

    endInsertRows();
}


void MyServersModel::removeServer(quint64 serverId)
{
    for (int row = 0; row < m_servers.size(); ++row)
    {
        if (m_servers[row].id == serverId)
        {
            beginRemoveRows(QModelIndex(), row, row);

            m_servers.removeAt(row);

            endRemoveRows();
            return;
        }
    }
}

void MyServersModel::updateServer(quint64 serverId,
                                  const QString &name,
                                  const QString &ip,
                                  const QString &port)
{
    int row = findRowById(serverId);

    if (row < 0)
        return;

    auto &server = m_servers[row];

    server.name = name;
    server.ip = ip;
    server.port = port;

    emit dataChanged(
        index(row),
        index(row),
        {
            ServerNameRole,
            ServerIpRole,
            ServerPortRole
        });
}

void MyServersModel::setName(quint64 serverId, QString newName)
{
    int row = findRowById(serverId);

    if (row < 0)
        return;

    m_servers[row].name = newName;

    emit dataChanged(
        index(row),
        index(row),
        { ServerNameRole });
}

void MyServersModel::setStatus(quint64 serverId, ServerStatus newStatus)
{
    int row = findRowById(serverId);

    if (row < 0)
        return;

    m_servers[row].status = newStatus;

    emit dataChanged(
        index(row),
        index(row),
        { ServerStatusRole });
}

void MyServersModel::setAddress(quint64 serverId, QString newIp, QString newPort)
{
    int row = findRowById(serverId);

    if (row < 0)
        return;

    m_servers[row].ip= newIp;
    m_servers[row].port=newPort;

    emit dataChanged(
        index(row),
        index(row),
        {
          ServerIpRole,
          ServerPortRole
        });
}

void MyServersModel::setIsActive(quint64 serverId)
{
    resetPreviousIsActiveServer();

    int row = findRowById(serverId);

    if (row < 0)
        return;

    m_servers[row].isActive = true;
    m_lastIsActiveId = serverId;

    emit dataChanged(
        index(row),
        index(row),
        { ServerIsActiveRole });
}

void MyServersModel::resetPreviousIsActiveServer()
{
    if (m_lastIsActiveId == -1)
        return;


    int row = findRowById(m_lastIsActiveId);

    if (row >= 0)
    {
        m_servers[row].isActive = false;

        emit dataChanged(
            index(row),
            index(row),
            { ServerIsActiveRole });
    }

    m_lastIsActiveId=-1; //reset it
}

int MyServersModel::doesServerExists(const QString &ip, const QString& port)
{
    for (int i = 0; i < m_servers.size(); ++i)
    {
        //check if server is the same.
        if (m_servers[i].ip == ip && m_servers[i].port == port)
            return m_servers[i].id;
    }

    return -1;
}


int MyServersModel::findRowById(quint64 serverId) const
{
    for (int i = 0; i < m_servers.size(); ++i)
    {
        if (m_servers[i].id == serverId)
            return i;
    }

    return -1;
}
