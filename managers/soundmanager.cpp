#include "soundmanager.h"

#include "logging/loggingcategories.h"

SoundManager::SoundManager(QObject* parent)
    :
    QObject(parent)
{
    qCInfo(_soundEffect) << "loading sound manager";

    m_newMessage.setSource(QUrl("qrc:/soundpack/speech/shimmer/newMessage.wav"));
    m_connected.setSource(QUrl("qrc:/soundpack/speech/shimmer/connected.wav"));
    m_disconnected.setSource(QUrl("qrc:/soundpack/speech/shimmer/disconnected.wav"));
    m_connectionLost.setSource(QUrl("qrc:/soundpack/speech/shimmer/connection-lost.wav"));
    m_channelSwitched.setSource(QUrl("qrc:/soundpack/speech/shimmer/channel-switched.wav"));
    m_youWereMoved.setSource(QUrl("qrc:/soundpack/speech/shimmer/you-were-moved.wav"));
    m_userJoin.setSource(QUrl("qrc:/soundpack/speech/shimmer/user-joined-your-channel.wav"));
    m_userLeft.setSource(QUrl("qrc:/soundpack/speech/shimmer/user-left-your-channel.wav"));
    m_userTimedOut.setSource(QUrl("qrc:/soundpack/speech/shimmer/user-timed-out.wav"));


    m_newMessage.setVolume(m_volume);
    m_connected.setVolume(m_volume);
    m_disconnected.setVolume(m_volume);
    m_connectionLost.setVolume(m_volume);
    m_channelSwitched.setVolume(m_volume);
    m_youWereMoved.setVolume(m_volume);
    m_userJoin.setVolume(m_volume);
    m_userLeft.setVolume(m_volume);
    m_userTimedOut.setVolume(m_volume);
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

    m_newMessage.setVolume(m_volume);
    m_connected.setVolume(m_volume);
    m_disconnected.setVolume(m_volume);
    m_connectionLost.setVolume(m_volume);
    m_channelSwitched.setVolume(m_volume);
    m_youWereMoved.setVolume(m_volume);
    m_userJoin.setVolume(m_volume);
    m_userLeft.setVolume(m_volume);
    m_userTimedOut.setVolume(m_volume);

    qCInfo(_soundEffect) << "set volume to " << newVolume;
    emit volumeChanged();
}



void SoundManager::playNewMessage()
{
    if(!canPlay(
            m_newMessageCooldown,
            500))
    {
        return;
    }

    m_newMessage.play();
}

void SoundManager::playConnected()
{
    if(!canPlay(
            m_connectedCooldown,
            500))
    {
        return;
    }

    m_connected.play();
}

void SoundManager::playDisconnected()
{
    // Suppress if Connection Lost was played recently
    if (m_lastConnectionLost.isValid() &&
        m_lastConnectionLost.elapsed() < 2000)
    {
        return;
    }

    if (!canPlay(m_disconnectedCooldown, 500))
        return;

    m_disconnected.play();
}

void SoundManager::playConnectionLost()
{
    if(!canPlay(
            m_connectionLostCooldown,
            500))
    {
        return;
    }

    m_lastConnectionLost.restart();

    m_connectionLost.play();
}

void SoundManager::playChannelSwitched()
{
    if(!canPlay(
            m_channelSwitchedCooldown,
            500))
    {
        return;
    }

    m_channelSwitched.play();
}

void SoundManager::playYouWereMoved()
{
    if(!canPlay(
            m_youWereMovedCooldown,
            500))
    {
        return;
    }

    m_youWereMoved.play();
}

void SoundManager::playUserJoin()
{
    if(!canPlay(
            m_userJoinCooldown,
            500))
    {
        return;
    }

    m_userJoin.play();
}

void SoundManager::playUserLeft()
{
    if(!canPlay(
            m_userLeftCooldown,
            500))
    {
        return;
    }

    m_userLeft.play();
}

void SoundManager::playUserTimedOut()
{
    if(!canPlay(
            m_userTimedOutCooldown,
            500))
    {
        return;
    }

    m_userTimedOut.play();
}


void SoundManager::changeAudioOutput(QAudioDevice *output)
{
    m_newMessage.setAudioDevice(*output);
    m_connected.setAudioDevice(*output);
    m_disconnected.setAudioDevice(*output);
    m_connectionLost.setAudioDevice(*output);
    m_channelSwitched.setAudioDevice(*output);
    m_youWereMoved.setAudioDevice(*output);
    m_userJoin.setAudioDevice(*output);
    m_userLeft.setAudioDevice(*output);
    m_userTimedOut.setAudioDevice(*output);

    qCInfo(_soundEffect) << "audio output changed.";
}
