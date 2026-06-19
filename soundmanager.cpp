#include "soundmanager.h"
#include <QDebug>

SoundManager::SoundManager(QObject* parent)
    :
    QObject(parent)
{
    m_message.setSource(
        QUrl("qrc:/soundeffects/message.wav"));

    m_userJoin.setSource(
        QUrl("qrc:/soundeffects/user-join.wav"));

    m_userLeave.setSource(
        QUrl("qrc:/soundeffects/user-left.wav"));

    // m_channelJoin.setSource(
        // QUrl("qrc:/sounds/channel_join.wav"));

    m_message.setVolume(m_volume);
    m_userJoin.setVolume(m_volume);
    m_userLeave.setVolume(m_volume);
    m_channelJoin.setVolume(m_volume);
}

bool SoundManager::canPlay(
    QElapsedTimer& timer,
    int cooldownMs)
{
    if(!timer.isValid())
    {
        timer.start();
        return true;
    }

    if(timer.elapsed() < cooldownMs)
        return false;

    timer.restart();
    return true;
}

float SoundManager::volume() const
{
    return m_volume;
}

void SoundManager::setVolume(float newVolume)
{
    if (qFuzzyCompare(m_volume, newVolume))
        return;
    m_volume = newVolume;

    m_message.setVolume(m_volume);
    m_userJoin.setVolume(m_volume);
    m_userLeave.setVolume(m_volume);
    m_channelJoin.setVolume(m_volume);

    emit volumeChanged();
}

void SoundManager::playMessage()
{
    if(!canPlay(
            m_messageCooldown,
            300))
    {
        return;
    }

    m_message.play();
}

void SoundManager::playUserJoin()
{
    if(!canPlay(
            m_joinCooldown,
            500))
    {
        return;
    }

    m_userJoin.play();
}

void SoundManager::playUserLeave()
{
    if(!canPlay(
            m_leaveCooldown,
            500))
    {
        return;
    }

    m_userLeave.play();
}

void SoundManager::playChannelJoin()
{
    if(!canPlay(
            m_channelCooldown,
            500))
    {
        return;
    }

    m_channelJoin.play();
}

void SoundManager::changeAudioOutput(QAudioDevice *output)
{
    m_message.setAudioDevice(*output);
    m_userJoin.setAudioDevice(*output);
    m_userLeave.setAudioDevice(*output);
    m_channelJoin.setAudioDevice(*output);
    // qDebug() << "soundmanager audio output changed.";
}
