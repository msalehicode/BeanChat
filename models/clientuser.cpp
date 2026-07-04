#include "clientuser.h"

ClientUser::ClientUser(QObject *parent)
{

}

quint64 ClientUser::id() const
{
    return m_id;
}

void ClientUser::setId(quint64 newId)
{
    if (m_id == newId)
        return;
    m_id = newId;
    emit idChanged();
}

QString ClientUser::username() const
{
    return m_username;
}

void ClientUser::setUsername(const QString &newUsername)
{
    if (m_username == newUsername)
        return;
    m_username = newUsername;
    emit usernameChanged();
}

QString ClientUser::avatarPath() const
{
    return m_avatarPath;
}

void ClientUser::setAvatarPath(const QString &newAvatarPath)
{
    if (m_avatarPath == newAvatarPath)
        return;
    m_avatarPath = newAvatarPath;
    emit avatarPathChanged();
}

QString ClientUser::iconsId() const
{
    return m_iconsId;
}

void ClientUser::setIconsId(const QString &newIconsId)
{
    if (m_iconsId == newIconsId)
        return;
    m_iconsId = newIconsId;
    emit iconsIdChanged();
}

QString ClientUser::appVersion() const
{
    return m_appVersion;
}

void ClientUser::setAppVersion(const QString &newAppVersion)
{
    if (m_appVersion == newAppVersion)
        return;
    m_appVersion = newAppVersion;
    emit appVersionChanged();
}

QString ClientUser::buildType() const
{
    return m_buildType;
}

void ClientUser::setBuildType(const QString &newBuildType)
{
    if (m_buildType == newBuildType)
        return;
    m_buildType = newBuildType;
    emit buildTypeChanged();
}

QString ClientUser::osName() const
{
    return m_osName;
}

void ClientUser::setOsName(const QString &newOsName)
{
    if (m_osName == newOsName)
        return;
    m_osName = newOsName;
    emit osNameChanged();
}

QString ClientUser::osVersion() const
{
    return m_osVersion;
}

void ClientUser::setOsVersion(const QString &newOsVersion)
{
    if (m_osVersion == newOsVersion)
        return;
    m_osVersion = newOsVersion;
    emit osVersionChanged();
}

bool ClientUser::isTalking() const
{
    return m_isTalking;
}

void ClientUser::setIsTalking(bool newIsTalking)
{
    if (m_isTalking == newIsTalking)
        return;
    m_isTalking = newIsTalking;
    emit isTalkingChanged();
}

bool ClientUser::muted() const
{
    return m_muted;
}

void ClientUser::setMuted(bool newMuted)
{
    if (m_muted == newMuted)
        return;
    m_muted = newMuted;
    emit mutedChanged();
}

bool ClientUser::deafened() const
{
    return m_deafened;
}

void ClientUser::setDeafened(bool newDeafened)
{
    if (m_deafened == newDeafened)
        return;
    m_deafened = newDeafened;
    emit deafenedChanged();
}

bool ClientUser::isAdmin() const
{
    return m_isAdmin;
}

void ClientUser::setIsAdmin(bool newIsAdmin)
{
    if (m_isAdmin == newIsAdmin)
        return;
    m_isAdmin = newIsAdmin;
    emit isAdminChanged();
}

ClientUser::Status ClientUser::status() const
{
    return m_status;
}

void ClientUser::setStatus(Status newStatus)
{
    if (m_status == newStatus)
        return;
    m_status = newStatus;
    emit statusChanged();
}

quint64 ClientUser::channelId() const
{
    return m_channelId;
}

void ClientUser::setChannelId(quint64 newChannelId)
{
    if (m_channelId == newChannelId)
        return;
    m_channelId = newChannelId;
    emit channelIdChanged();
}

float ClientUser::volume() const
{
    return m_volume;
}

void ClientUser::setVolume(float volume)
{
    volume = std::clamp(volume, 0.f, 2.f);

    if (qFuzzyCompare(m_volume, volume))
        return;

    m_volume = volume;
    emit volumeChanged();
}

bool ClientUser::self() const
{
    return m_self;
}

void ClientUser::setSelf(bool newSelf)
{
    if (m_self == newSelf)
        return;
    m_self = newSelf;
    emit selfChanged();
}

bool ClientUser::hasCamera() const
{
    return m_hasCamera;
}

void ClientUser::setHasCamera(bool newHasCamera)
{
    if (m_hasCamera == newHasCamera)
        return;
    m_hasCamera = newHasCamera;
    emit hasCameraChanged();
}
