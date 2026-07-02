#pragma once

#include <QAbstractListModel>

enum class ServerStatus
{
    Unknown,
    Favorite,
    Normal,
    Archived
};

struct MyServerInfo
{
    quint64 id; //runtime id, server id in datbase named index
    int index;//index in database!
    bool isActive=false; //when user is connected to server would be on and show indicator active server
    QString name; //this is local (user set it for himself)
    QString avatarPath; //since connected to server, if avatar not found, server would send his avatar to clients in response to RESERVED_TO_ASK_SERVERS_AVATAR
    QString ip;
    QString port=USER_DEFAULT_SERVER_PORT_STR;
    ServerStatus status=ServerStatus::Normal; //to know how to list them for user.
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
        ServerAvatarPathRole,
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

    void addServer(QString name, const QString &avatarPath,
                   const QString &ip, const QString &port,
                   bool isActive=false, quint64 index=-1);

    void removeServer(quint64 serverId);

    void updateServer(quint64 serverId, const QString &name,
                      const QString &ip, const QString &port);

    void setStatus(quint64 serverId, ServerStatus newStatus);

    void setName(quint64 serverId, QString newName); //this runs when server changed his name. user cant change name

    void setAddress(quint64 serverId, QString newIp, QString newPort);
    void setIsActive(quint64 serverId);
    void setAvatarPath(quint64 serverId, const QString& path);
    bool setAvatarPath(const QString &path);
    void resetPreviousIsActiveServer();

    int doesServerExists(const QString& ip, const QString &port);


private:
    quint64 m_lastIsActiveId=-1; //to reset last item when new item is activated
    quint64 m_nextServerId = 1; //runtime id, server id in datbase named index
    int findRowById(quint64 serverId) const;
    QList<MyServerInfo> m_servers;
};
