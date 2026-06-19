#pragma once

#include <QObject>
#include <QSoundEffect>
#include <QElapsedTimer>

class SoundManager : public QObject
{
    Q_OBJECT

public:
    explicit SoundManager(QObject* parent = nullptr);

    float volume() const;
    void setVolume(float newVolume);

public slots:

    void playMessage();
    void playUserJoin();
    void playUserLeave();
    void playChannelJoin();

    void changeAudioOutput(QAudioDevice* output);
signals:
    void volumeChanged();

private:

    bool canPlay(
        QElapsedTimer& timer,
        int cooldownMs);

private:
    float m_volume=1.0;
    QSoundEffect m_message;
    QSoundEffect m_userJoin;
    QSoundEffect m_userLeave;
    QSoundEffect m_channelJoin;

    QElapsedTimer m_messageCooldown;
    QElapsedTimer m_joinCooldown;
    QElapsedTimer m_leaveCooldown;
    QElapsedTimer m_channelCooldown;
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged FINAL)
};
