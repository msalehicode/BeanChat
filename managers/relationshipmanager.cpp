#include "relationshipmanager.h"

RelationshipManager::RelationshipManager(
    Database *database,
    QObject *parent)
    : QObject(parent),
    m_database(database)
{
    load();
}

void RelationshipManager::load()
{
    m_relationships.clear();

    const auto relationships =
        m_database->loadUserRelationships();

    for (const auto &relationship : relationships)
    {
        m_relationships.insert(
            relationship.identity,
            relationship);
    }
}

bool RelationshipManager::contains(const QString &identity) const
{
    return m_relationships.contains(identity);
}


bool RelationshipManager::isFriend(const QString &identity) const
{
    auto it = m_relationships.find(identity);

    return it != m_relationships.end() && it->relationship == Relationship::Type::Friend;
}

bool RelationshipManager::isBlocked(const QString &identity) const
{
    auto it = m_relationships.find(identity);

    return it != m_relationships.end() && it->relationship == Relationship::Type::Blocked;
}



bool RelationshipManager::isMuted(const QString &identity) const
{
    auto it = m_relationships.find(identity);

    return it != m_relationships.end() && it->muted;
}



const UserRelationship* RelationshipManager::find(const QString &identity) const
{
    auto it = m_relationships.find(identity);

    if (it == m_relationships.end())
        return nullptr;

    return &it.value();
}




UserRelationship& RelationshipManager::getOrCreate(const QString &identity)
{
    auto it = m_relationships.find(identity);

    if (it == m_relationships.end())
    {
        UserRelationship relationship;
        relationship.identity = identity;

        it = m_relationships.insert(identity, relationship);
    }

    return it.value();
}

bool RelationshipManager::addFriend(const QString &identity)
{
    auto &relationship = getOrCreate(identity);

    relationship.relationship = Relationship::Type::Friend;

    return m_database->saveUserRelationship(relationship);
}

bool RelationshipManager::blockUser(const QString &identity)
{
    auto &relationship = getOrCreate(identity);

    relationship.relationship = Relationship::Type::Blocked;

    return m_database->saveUserRelationship(relationship);
}

bool RelationshipManager::removeFriend(const QString &identity)
{
    auto &relationship = getOrCreate(identity);

    relationship.relationship = Relationship::Type::None;

    return m_database->saveUserRelationship(relationship);
}

bool RelationshipManager::unblockUser(const QString &identity)
{
    auto &relationship = getOrCreate(identity);

    relationship.relationship = Relationship::Type::None;

    return m_database->saveUserRelationship(relationship);
}

bool RelationshipManager::setMuted(const QString &identity,
                                   bool muted)
{
    auto &relationship = getOrCreate(identity);

    relationship.muted = muted;

    return m_database->saveUserRelationship(relationship);
}

bool RelationshipManager::setVoiceVolume(
    const QString &identity,
    int volume)
{
    auto &relationship = getOrCreate(identity);

    relationship.voiceVolume = volume;

    return m_database->saveUserRelationship(relationship);
}


bool RelationshipManager::setNickname(
    const QString &identity,
    const QString &nickname)
{
    auto &relationship = getOrCreate(identity);

    relationship.nickname = nickname;

    return m_database->saveUserRelationship(relationship);
}

bool RelationshipManager::setNote(
    const QString &identity,
    const QString &note)
{
    auto &relationship = getOrCreate(identity);

    relationship.note = note;

    return m_database->saveUserRelationship(relationship);
}

float RelationshipManager::voiceVolume(
    const QString &identity) const
{
    auto it = m_relationships.find(identity);

    if (it == m_relationships.end())
        return 1.0f;

    return it->voiceVolume;
}

QString RelationshipManager::nickname(
    const QString &identity) const
{
    auto it = m_relationships.find(identity);

    if (it == m_relationships.end())
        return {};

    return it->nickname;
}

QString RelationshipManager::note(
    const QString &identity) const
{
    auto it = m_relationships.find(identity);

    if (it == m_relationships.end())
        return {};

    return it->note;
}


UserRelationship *RelationshipManager::find(const QString &identity)
{
    auto it = m_relationships.find(identity);

    if (it == m_relationships.end())
        return nullptr;

    return &it.value();
}
