#pragma once

#include <QObject>
#include <QHash>
#include <QDebug>
#include "../models/clientuser.h"


class ClientUser;

class ClientUserManager : public QObject
{
    Q_OBJECT

public:
    explicit ClientUserManager(QObject *parent = nullptr);
    ~ClientUserManager();

    ClientUser *createUser(quint64 id);
    ClientUser *user(quint64 id) const;

    bool contains(quint64 id) const;

    void removeUser(quint64 id);

    void clear();

    int count() const;

signals:
    void userCreated(ClientUser *user);
    void userRemoved(ClientUser* user);
    void cleared();

private:
    QHash<quint64, ClientUser*> m_users;
};
