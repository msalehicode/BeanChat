#include "clientusermanager.h"

ClientUserManager::ClientUserManager(QObject *parent)
    : QObject(parent)
{

}

ClientUser *ClientUserManager::createUser(quint64 id)
{
    // if (id == 0)
    // {
        // qDebug() << "createUser error: id ==0";
        // return nullptr;
    // }

    if (m_users.contains(id))
    {
        qDebug() << "createUser error: user exists.";
        return nullptr;
    }


    ClientUser *user = new ClientUser(this);

    user->setId(id);

    m_users.insert(id, user);

    emit userCreated(user);

    return user;
}

ClientUser *ClientUserManager::user(quint64 id) const
{
    return m_users.value(id, nullptr);
}


bool ClientUserManager::contains(quint64 id) const
{
    return m_users.contains(id);
}

int ClientUserManager::count() const
{
    return m_users.size();
}

void ClientUserManager::removeUser(quint64 id)
{
    auto it = m_users.find(id);

    if (it == m_users.end())
        return;

    ClientUser *user = it.value();

    m_users.erase(it);

    user->deleteLater();

    emit userRemoved(id);
}

void ClientUserManager::clear()
{
    qDeleteAll(m_users);

    m_users.clear();

    emit cleared();
}
