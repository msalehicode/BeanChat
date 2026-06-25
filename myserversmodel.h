#pragma once

#include <QAbstractListModel>

enum class ServerStatus
{
    Unknown,
    Favorite,
    Normal,
    Archived
};

struct ServerInfo
{
    quint64 id; //runtime id, server id in datbase named index
    int index;//index in database!
    bool isActive=false;
    QString name;
    QString ip;
    QString port="9987";
    ServerStatus status=ServerStatus::Normal;
};


class MyServersModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Roles
    {
        ServerIdRole = Qt::UserRole + 1,
        ServerIndexRole,
        ServerNameRole,
        ServerIsActiveRole,
        ServerIpRole,
        ServerPortRole,
        ServerStatusRole
    };


    explicit MyServersModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int,QByteArray>
    roleNames() const override;

public:

    void clear();

    void addServer(QString name, const QString &ip, const QString &port, bool isActive=false, quint64 index=-1);
    void removeServer(quint64 serverId);
    void updateServer(quint64 serverId, const QString &name, const QString &ip, const QString &port);
    void setStatus(quint64 serverId, ServerStatus newStatus);
    void setName(quint64 serverId, QString newName); //this runs when server changed his name. user cant change name
    void setAddress(quint64 serverId, QString newIp, QString newPort);
    void setIsActive(quint64 serverId);
    void resetPreviousIsActiveServer();

    int doesServerExists(const QString& ip, const QString &port);

private:
    quint64 m_lastIsActiveId=-1; //to reset last item when new item is activated
    quint64 m_nextServerId = 1; //runtime id, server id in datbase named index
    int findRowById(quint64 serverId) const;
    QList<ServerInfo> m_servers;
};
