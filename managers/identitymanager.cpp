#include "identitymanager.h"

#include <crypto/Crypto.h>

#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStandardPaths>

#include <QDateTime>
#include <QDebug>

IdentityManager::IdentityManager(QObject *parent)
    : QObject(parent)
{

}

QString IdentityManager::identityFile() const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir().mkpath(dir);

    return dir + "/identities.json";
}


const QList<Identity>& IdentityManager::identities() const
{
    return m_identities;
}



Identity* IdentityManager::currentIdentity()
{
    for(Identity& identity : m_identities)
    {
        if(identity.name == m_currentIdentityName)
        {
            return &identity;
        }
    }

    return nullptr;
}

Identity* IdentityManager::findIdentity(const QString& name)
{
    for(Identity& identity : m_identities)
    {
        if(identity.name == name)
            return &identity;
    }

    return nullptr;
}


bool IdentityManager::createIdentity(const QString& name)
{
    QString identityName = name.trimmed();

    if(identityName.isEmpty())
        return false;

    if(findIdentity(identityName))
        return false;


    QByteArray publicKey;
    QByteArray privateKey;

    if(!BeanChatCommon::Crypto::generateKeyPair(
            publicKey,
            privateKey))
    {
        qDebug() << "Failed to generate identity.";
        return false;
    }


    Identity identity;

    identity.name = identityName;

    identity.publicKey = publicKey;

    identity.privateKey = privateKey;

    identity.createdAt = QDateTime::currentSecsSinceEpoch();


    m_identities.push_back(identity);


    //first identity becomes active
    if(m_currentIdentityName.isEmpty())
    {
        m_currentIdentityName = identityName;
        emit currentIdentityChanged();
        emit currentIdentityChangedTo(identityName); //to update config file
    }


    return save();
}

bool IdentityManager::renameIdentity(
    const QString& oldName,
    const QString& newName)
{
    Identity* identity =
        findIdentity(oldName);

    if(!identity)
        return false;


    QString name =
        newName.trimmed();

    if(name.isEmpty())
        return false;

    if(findIdentity(name))
        return false;


    identity->name =
        name;


    if(m_currentIdentityName ==
        oldName)
    {
        m_currentIdentityName =
            name;
    }

    return save();
}

bool IdentityManager::removeIdentity(const QString& name)
{
    for(int i = 0; i < m_identities.size(); ++i)
    {
        if(m_identities[i].name != name)
            continue;

        m_identities.removeAt(i);

        if(m_currentIdentityName == name)
        {
            if(m_identities.isEmpty())
                m_currentIdentityName.clear();
            else
                m_currentIdentityName = m_identities.first().name;
        }

        return save();
    }

    return false;
}
bool IdentityManager::setCurrentIdentity(const QString& name)
{
    if(!findIdentity(name))
        return false;

    m_currentIdentityName = name;
    emit currentIdentityChangedTo(name); //to update config file

    return save();
}


bool IdentityManager::save()
{
    QJsonObject root;

    root["current"] = m_currentIdentityName;


    QJsonArray identities;

    for(const Identity& identity : m_identities)
    {
        QJsonObject obj;

        obj["name"] = identity.name;

        obj["publicKey"] = identity.publicKeyBase64();

        obj["privateKey"] = identity.privateKeyBase64();

        obj["createdAt"] = static_cast<qint64>(identity.createdAt);

        identities.append(obj);
    }

    root["identities"] = identities;


    QFile file(identityFile());

    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Failed to save identities.";

        return false;
    }

    file.write(QJsonDocument(root).toJson());

    emit identitiesChanged();
    return true;
}



bool IdentityManager::load()
{
    QFile file(identityFile());

    m_identities.clear();
    m_currentIdentityName.clear();

    if(!file.exists())
    {
        qDebug() << "identity file not found. lets generate one.";
        if(createIdentity("Default"))
            return true;

        qDebug() << "faild to create identity Default";
        return false;
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open identity file.";
        return false;
    }


    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());

    if(!doc.isObject())
    {
        qDebug() << "Invalid identity file.";
        return false;
    }


    QJsonObject root = doc.object();

    m_currentIdentityName = root["current"].toString();


    QJsonArray array = root["identities"].toArray();

    for(const QJsonValue& value : array)
    {
        QJsonObject obj = value.toObject();

        Identity identity;

        identity.name = obj["name"].toString();

        identity.setPublicKeyBase64(obj["publicKey"].toString());

        identity.setPrivateKeyBase64(obj["privateKey"].toString());

        identity.createdAt = static_cast<qint64>(obj["createdAt"].toDouble());

        m_identities.push_back(identity);
    }

    emit identitiesChanged();
    return true;
}


QStringList IdentityManager::identityNames() const
{
    QStringList list;

    for(const Identity& identity : m_identities)
        list << identity.name;

    return list;
}


int IdentityManager::currentIdentityIndex() const
{
    for(int i = 0; i < m_identities.size(); ++i)
    {
        if(m_identities[i].name == m_currentIdentityName)
        {
            return i;
        }
    }

    return -1;
}

void IdentityManager::setCurrentIdentityIndex(int index)
{
    if(index < 0 ||
        index >= m_identities.size())
    {
        return;
    }

    setCurrentIdentity(m_identities[index].name);

    emit currentIdentityChanged();
}

void IdentityManager::removeCurrentIdentity()
{
    qDebug() << "remove current identity";
    removeIdentity(m_currentIdentityName);
}

void IdentityManager::renameCurrentIdentity(const QString &newName)
{
    qDebug() << "rename current identity to " << newName;
    renameIdentity(m_currentIdentityName,newName);
}
