#pragma once

#include <QAbstractListModel>

#include <QElapsedTimer>

#include "clientuser.h"

class ConnectedUsersModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:

    enum Roles
    {
        UserIdRole = Qt::UserRole + 1,
        UserNameRole,
        UserAvatarPathRole,
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


    void removeUser(quint64 userId);

    void setUsername(quint64 userId, const QString& username);

    void setUserAvatarPath(quint64 userId, const QString& path);

    void setStatus(quint64 userId, ClientUser::Status status);

    void setIcons(quint64 userId, const QString& iconsId);

    ClientUser *findUser(quint64 userId);


    void addUser(quint64 id, QString username, QString avatarPath, QString iconsId,
                 bool talking, bool muted, bool deafened, bool camera,
                 QString version, QString buildType, QString osName, QString osVersion,
                 ClientUser::Status status);
private:
    int findRowById(quint64 userId) const;
    QList<ClientUser*> m_connectedUsers;
};
