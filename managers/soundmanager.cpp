#include "soundmanager.h"

SoundManager::SoundManager(QObject* parent)
    :
    QObject(parent)
{
    m_message.setSource(
        QUrl("qrc:/ui/soundeffects/message.wav"));

    m_userJoin.setSource(
        QUrl("qrc:/ui/soundeffects/user-join.wav"));

    m_userLeave.setSource(
        QUrl("qrc:/ui/soundeffects/user-left.wav"));

    m_messageBack.setSource(
        QUrl("qrc:/ui/soundeffects/message-back.wav"));

    m_message.setVolume(m_volume);
    m_userJoin.setVolume(m_volume);
    m_userLeave.setVolume(m_volume);
    m_messageBack.setVolume(m_volume);
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
    m_messageBack.setVolume(m_volume);

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

void SoundManager::playMessageBack()
{
    if(!canPlay(
            m_messageBackCooldown,
            500))
    {
        return;
    }

    m_messageBack.play();
}

void SoundManager::changeAudioOutput(QAudioDevice *output)
{
    m_message.setAudioDevice(*output);
    m_userJoin.setAudioDevice(*output);
    m_userLeave.setAudioDevice(*output);
    m_messageBack.setAudioDevice(*output);
    qDebug() << "soundmanager audio output changed.";
}
