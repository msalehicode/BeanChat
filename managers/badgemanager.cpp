#include "badgemanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include "logging/loggingcategories.h"

BadgeManager::BadgeManager(QObject *parent)
    : QObject(parent)
{
}

void BadgeManager::clear()
{
    m_badges.clear();
}

bool BadgeManager::load(const QByteArray &json)
{
    clear();

    QJsonParseError error;

    QJsonDocument doc =
        QJsonDocument::fromJson(json, &error);

    if (doc.isNull() || !doc.isObject())
        return false;

    QJsonObject root = doc.object();

    QJsonObject badges =
        root["badges"].toObject();

    QJsonValue version = root["version"];
    if (!version.isDouble() || version.toInt() != 1)
        return false;

    for (auto it = badges.begin();
         it != badges.end();
         ++it)
    {
        if (!it.value().isDouble())
            continue;

        qCInfo(_app) << "Loaded badge:"
                     << it.key()
                     << "mask:" << it.value().toInt();

        setBadges(it.key(),
                  quint32(it.value().toInt()));
    }

    qCInfo(_app) << "loaded badges count:" << m_badges.count();

    return true;
}

void BadgeManager::setBadges(const QString &identity, quint32 badgeMask)
{
    if (badgeMask == 0)
        m_badges.remove(identity);
    else
        m_badges.insert(identity, badgeMask);
}

void BadgeManager::remove(const QString &identity)
{
    m_badges.remove(identity);
}

quint32 BadgeManager::badges(const QString &identity) const
{
    return m_badges.value(identity, 0);
}

bool BadgeManager::hasBadge(const QString &identity, quint32 badge) const
{
    qCInfo(_app)
    << "badge Lookup identity:" << identity
    << "contains:" << m_badges.contains(identity)
    << "mask:" << m_badges.value(identity, 0);

    return (m_badges.value(identity, 0) & badge) != 0;
}

bool BadgeManager::contains(const QString &identity) const
{
    return m_badges.contains(identity);
}
