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
        UserRelationRole,
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

    void addUser(ClientUser* user);

    void clear();

    void removeUser(quint64 userId);

    ClientUser *findUser(quint64 userId);

private:
    void observeUser(ClientUser *user);
    int findRowById(quint64 userId) const;
    QList<ClientUser*> m_connectedUsers;
    QSet<ClientUser*> m_observedUsers;
};
