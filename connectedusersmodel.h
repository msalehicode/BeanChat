#pragma once

#include <QAbstractListModel>

#include <QElapsedTimer>

enum class UserActivityStatus
{
    Unknown=0,
    Offline,
    Online,
    Idle
};

struct ConnectedUser
{
    quint64 id;
    QString username;
    QString iconsId="";

    bool isTalking = false;
    bool muted = false;
    bool deafened = false;
    bool hasVideo = false;

    UserActivityStatus status= UserActivityStatus::Online;


    // Client information
    QString appVersion;      // "1.2.5"
    QString buildType;       // "Release", "Debug"

    // Operating system
    QString osName;          // "Windows", "Linux", "macOS"
    QString osVersion;       // "11", "Ubuntu 24.04"

};


class ConnectedUsersModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:

    enum Roles
    {
        UserIdRole = Qt::UserRole + 1,
        UserNameRole,
        UserStatusRole,
        UserIconsRole,
        UserOsVersionRole,
        USerOsNameRole,
        UserAppVersionRole,
        UserAppBuildTypeRole
    };


    explicit ConnectedUsersModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int,QByteArray>
    roleNames() const override;

    int count() const;
signals:
    void countChanged();
public:

    void clear();

    void addUser(quint64 id, QString username, QString iconsId,
                 bool talking, bool muted, bool deafened, bool camera,
                 QString version, QString buildType, QString osName, QString osVersion,
                 UserActivityStatus status=UserActivityStatus::Online);

    void removeUser(quint64 userId);

    void setUsername(quint64 userId, const QString& username);

    void setStatus(quint64 userId, UserActivityStatus status);

    void setIcons(quint64 userId, const QString& iconsId);

    ConnectedUser* findUser(quint64 userId);


private:
    int findRowById(quint64 userId) const;
    QList<ConnectedUser> m_connectedUsers;
};
