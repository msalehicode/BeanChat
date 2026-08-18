#include "audiomixer.h"

AudioMixer::AudioMixer(QObject *parent)
    : QObject(parent)
{
    m_mixTimer.setTimerType(Qt::PreciseTimer);
    m_mixTimer.setInterval(20);

    connect(&m_mixTimer,
            &QTimer::timeout,
            this,
            &AudioMixer::mix);

    m_mixTimer.start();
}


void AudioMixer::addVoice(
    quint64 userId,
    const QByteArray &pcm,
    int volume)
{
    QMutexLocker locker(&m_mutex);

    auto &queue = m_pendingFrames[userId];

    // Drop old frames if we're falling behind.
    while (queue.size() >= 2)
        queue.dequeue();

    VoiceFrame frame;
    frame.pcm = pcm;
    frame.volume = volume;

    queue.enqueue(std::move(frame));

#if D_PRINT_AUDIO_INFO
    qDebug()
        << "user"
        << userId
        << "queue size:"
        << queue.size();
#endif
}


void AudioMixer::mix()
{
#if D_PRINT_AUDIO_INFO
    static QElapsedTimer timer;

    if (!timer.isValid())
        timer.start();

    qDebug() << "mix interval =" << timer.restart();
#endif

    QByteArray mixed;

    int sampleCount = OPUS_DEFAULT_FRAMESIZE;

    mixed.resize(sampleCount * sizeof(qint16));
    mixed.fill(0);

    auto mixedSamples =
        reinterpret_cast<qint16*>(mixed.data());

#if D_PRINT_AUDIO_INFO
    int mixedUsers = 0;
#endif
    {
        QMutexLocker locker(&m_mutex);

        for (auto it = m_pendingFrames.begin();
             it != m_pendingFrames.end();)
        {
            auto &queue = it.value();

            if (queue.isEmpty())
            {
                ++it;
                continue;
            }


            VoiceFrame frame = queue.dequeue();

#if D_PRINT_AUDIO_INFO
            mixedUsers++;
#endif

            auto samples =
                reinterpret_cast<const qint16*>(
                    frame.pcm.constData());

            int count =
                frame.pcm.size() /
                sizeof(qint16);

            for (int i = 0; i < count; ++i)
            {
                int s =
                    mixedSamples[i] +
                    samples[i] * frame.volume / 100;

                s = qBound(-32768, s, 32767);

                mixedSamples[i] = static_cast<qint16>(s);
            }

            if (queue.isEmpty())
                it = m_pendingFrames.erase(it);
            else
                ++it;
        }
    }

#if D_PRINT_AUDIO_INFO
    qDebug() << "mixed users:" << mixedUsers;
#endif

    emit mixedFrameReady(mixed);
}












