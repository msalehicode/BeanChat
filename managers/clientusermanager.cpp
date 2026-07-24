#include "clientusermanager.h"

#include "logging/loggingcategories.h"


ClientUserManager::ClientUserManager(QObject *parent)
    : QObject(parent)
{
    qCInfo(_clientUser) << "client user constructed";
    connect(this,
            &QObject::destroyed,
            []()
            {
                qCCritical(_clientUser) << "!!!!!!!! ClientUserManager DESTROYED !!!!!!!!";
            });
}

ClientUserManager::~ClientUserManager()
{
    qCWarning(_clientUser) << "clientUserManager destructor called.";
}

ClientUser *ClientUserManager::createUser(quint64 id)
{
    qCInfo(_clientUser) << "createUser id=" <<id;
    if (id == 0)
    {
        qDebug() << "createUser error: id ==0";
        return nullptr;
    }

    if (m_users.contains(id))
    {
        qCWarning(_clientUser) << "createUser error: user exists, id=" <<id;
        return nullptr;
    }
    else
        qCInfo(_clientUser) << "id not exists creating User. id=" << id;


    ClientUser *user = new ClientUser(this);

    user->setId(id);

    //test to know whne destoryed..
    connect(user, &QObject::destroyed, this,
            [id]()
            {
                qCCritical(_clientUser)  << "======================= =CLIENT USER DESTROYED id =" << id;
            });

    m_users.insert(id, user);

    qCInfo(_clientUser) << "created user id=" << id << " add=" <<user;
    emit userCreated(user);

    return user;
}

ClientUser *ClientUserManager::user(quint64 id) const
{
    ClientUser* found = m_users.value(id, nullptr);
    if(!found)
        qCInfo(_clientUser) << "id not exists id=" << id;
    // else
        // qCInfo(_clientUser) << "user found, id=" << found->id() << " add=" << found;
    return found;
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
    qCInfo(_clientUser) << "remove user id=" <<id;
    auto it = m_users.find(id);

    if (it == m_users.end())
        return;

    ClientUser *user = it.value();

    // m_users.erase(it);

    emit userRemoved(user);

    // user->deleteLater(); //-> cause crash. it removes later
    // delete user; //for test delete obj immediately. but for now we store all users and dont delete them.
}

void ClientUserManager::clear()
{
    emit cleared();

    qCInfo(_clientUser) << "clear ClientUserManager";
    qDeleteAll(m_users);
    m_users.clear();
}
