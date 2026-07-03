#pragma once

#include <QObject>

class ClientUser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint64 id READ id NOTIFY idChanged FINAL)
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged FINAL)
    Q_PROPERTY(QString avatarPath READ avatarPath NOTIFY avatarPathChanged FINAL)
    Q_PROPERTY(QString iconsId READ iconsId NOTIFY iconsIdChanged FINAL)
    Q_PROPERTY(QString appVersion READ appVersion NOTIFY appVersionChanged FINAL)
    Q_PROPERTY(QString buildType READ buildType NOTIFY buildTypeChanged FINAL)
    Q_PROPERTY(QString osName READ osName NOTIFY osNameChanged FINAL)
    Q_PROPERTY(QString osVersion READ osVersion NOTIFY osVersionChanged FINAL)
    Q_PROPERTY(bool isTalking READ isTalking NOTIFY isTalkingChanged FINAL)
    Q_PROPERTY(bool muted READ muted NOTIFY mutedChanged FINAL)
    Q_PROPERTY(bool deafened READ deafened NOTIFY deafenedChanged FINAL)
    Q_PROPERTY(bool isAdmin READ isAdmin NOTIFY isAdminChanged FINAL)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged FINAL)
    Q_PROPERTY(quint64 channelId READ channelId NOTIFY channelIdChanged FINAL)
    Q_PROPERTY(float volume READ volume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(bool self READ self NOTIFY selfChanged FINAL)
    Q_PROPERTY(bool hasCamera READ hasCamera NOTIFY hasCameraChanged FINAL)

public:
    explicit ClientUser(QObject *parent = nullptr);


    enum class Status
    {
        Unknown = 0,
        Offline,
        Online,
        Idle
    };
    Q_ENUM(Status) //make it usebale in qml



    quint64 id() const;
    void setId(quint64 newId);

    QString username() const;
    void setUsername(const QString &newUsername);

    QString avatarPath() const;
    void setAvatarPath(const QString &newAvatarPath);

    QString iconsId() const;
    void setIconsId(const QString &newIconsId);

    QString appVersion() const;
    void setAppVersion(const QString &newAppVersion);

    QString buildType() const;
    void setBuildType(const QString &newBuildType);

    QString osName() const;
    void setOsName(const QString &newOsName);

    QString osVersion() const;
    void setOsVersion(const QString &newOsVersion);

    bool isTalking() const;
    void setIsTalking(bool newIsTalking);

    bool muted() const;
    void setMuted(bool newMuted);

    bool deafened() const;
    void setDeafened(bool newDeafened);

    bool isAdmin() const;
    void setIsAdmin(bool newIsAdmin);

    Status status() const;
    void setStatus(Status newStatus);

    quint64 channelId() const;
    void setChannelId(quint64 newChannelId);

    float volume() const;
    void setVolume(float newVolume);

    bool self() const;
    void setSelf(bool newSelf);

    bool hasCamera() const;
    void setHasCamera(bool newHasCamera);

signals:


    void idChanged();

    void usernameChanged();

    void avatarPathChanged();

    void iconsIdChanged();

    void appVersionChanged();

    void buildTypeChanged();

    void osNameChanged();

    void osVersionChanged();

    void isTalkingChanged();

    void mutedChanged();

    void deafenedChanged();

    void isAdminChanged();

    void statusChanged();

    void channelIdChanged();

    void volumeChanged();

    void selfChanged();

    void hasCameraChanged();

private:

    //info
    quint64 m_id=0;
    quint64 m_channelId=0;
    QString m_username;
    QString m_avatarPath;
    QString m_iconsId="";
    float m_volume = 1.0f;
    bool m_self=false;

    //user's app info
    QString m_appVersion;
    QString m_buildType;
    QString m_osName;
    QString m_osVersion;

    //status
    bool m_isTalking = false;
    bool m_muted = false;
    bool m_deafened = false;
    bool m_hasCamera = false;
    bool m_isAdmin = false;
    Status m_status= Status::Online;
};
