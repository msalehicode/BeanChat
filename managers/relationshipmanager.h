#pragma once

#include <QObject>
#include <QHash>
#include "../models/userrelationship.h"
#include "database/database.h"

class RelationshipManager : public QObject
{
    Q_OBJECT

public:
    explicit RelationshipManager(Database *database, QObject *parent = nullptr);

    void load();

    bool contains(const QString &identity) const;

    bool isFriend(const QString &identity) const;
    bool isBlocked(const QString &identity) const;
    bool isMuted(const QString &identity) const;

    float voiceVolume(const QString &identity) const;

    QString nickname(const QString &identity) const;
    QString note(const QString &identity) const;

    const UserRelationship *find(const QString &identity) const;

    Q_INVOKABLE bool addFriend(const QString &identity);
    Q_INVOKABLE bool removeFriend(const QString &identity);
    Q_INVOKABLE bool blockUser(const QString &identity);
    Q_INVOKABLE bool unblockUser(const QString &identity);
    Q_INVOKABLE bool setNickname(const QString &identity, const QString &nickname);
    Q_INVOKABLE bool setNote(const QString &identity, const QString &note);
    Q_INVOKABLE bool setMuted(const QString &identity, bool muted);
    Q_INVOKABLE bool setVoiceVolume(const QString &identity, int volume);


    UserRelationship *find(const QString &identity);
private:
    UserRelationship &getOrCreate(const QString &identity);

    Database *m_database = nullptr;

    QHash<QString, UserRelationship> m_relationships;
};
