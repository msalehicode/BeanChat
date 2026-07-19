#pragma once

#include <QObject>
#include <QSoundEffect>
#include <QElapsedTimer>
#include <QDebug>


class SoundManager : public QObject
{
    Q_OBJECT

public:
    explicit SoundManager(QObject* parent = nullptr);

    float volume() const;
    void setVolume(float newVolume);

public slots:
    void playNewMessage();

    void playConnected();
    void playDisconnected();
    void playConnectionLost();

    void playChannelSwitched();
    void playYouWereMoved();

    void playUserJoin();
    void playUserLeft();
    void playUserTimedOut();
    void playUserDisconnected();


    void playMicMuted();
    void playMicActiavted();

    void playSoundMuted();
    void playSoundResumed();

    void playCameraOn();
    void playCameraOff();


    void changeAudioOutput(QAudioDevice* output);
signals:
    void volumeChanged();

private:

    bool canPlay(
        QElapsedTimer& timer,
        int cooldownMs);

private:
    float m_volume=USER_DEFAULT_SOUNDEFFECTS_VOLUME;
    QSoundEffect m_newMessage;

    QSoundEffect m_connected;
    QSoundEffect m_disconnected;
    QSoundEffect m_connectionLost;


    QSoundEffect m_channelSwitched;
    QSoundEffect m_youWereMoved;

    QSoundEffect m_userJoin;
    QSoundEffect m_userLeft;
    QSoundEffect m_userTimedOut;
    QSoundEffect m_userDisconnected;

    QSoundEffect m_micMuted;
    QSoundEffect m_micActivated;

    QSoundEffect m_soundMuted;
    QSoundEffect m_soundResumed;

    QSoundEffect m_cameraOn;
    QSoundEffect m_cameraOff;



    //cooldown
    QElapsedTimer m_newMessageCooldown;

    QElapsedTimer m_connectedCooldown;
    QElapsedTimer m_disconnectedCooldown;
    QElapsedTimer m_connectionLostCooldown;
    QElapsedTimer m_lastConnectionLost; //to prevent play disconneted after this

    QElapsedTimer m_channelSwitchedCooldown;
    QElapsedTimer m_youWereMovedCooldown;

    QElapsedTimer m_userJoinCooldown;
    QElapsedTimer m_userLeftCooldown;
    QElapsedTimer m_userDisconnectedCooldown;
    QElapsedTimer m_userTimedOutCooldown;


    QElapsedTimer m_micMutedCooldown;
    QElapsedTimer m_micActivatedCooldown;

    QElapsedTimer m_soundMutedCooldown;
    QElapsedTimer m_soundResumedCooldown;

    QElapsedTimer m_cameraOnCooldown;
    QElapsedTimer m_cameraOffCooldown;


    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged FINAL)
};
