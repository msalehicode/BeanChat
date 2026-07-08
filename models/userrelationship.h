#pragma once

#include <QString>

class Relationship
{
    Q_GADGET

public:
    enum Type
    {
        None = 0,
        Friend,
        Blocked
    };
    Q_ENUM(Type)
};

struct UserRelationship
{
    QString identity;

    Relationship::Type relationship = Relationship::Type::None;

    bool muted = false;
    int voiceVolume = SPEAKER_DEFAULT_CHANNEL_USERS_VOLUME;

    QString nickname;
    QString note;
};
