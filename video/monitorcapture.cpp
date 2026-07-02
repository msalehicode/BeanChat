#include "monitorcapture.h"

MonitorCapture::MonitorCapture(QObject *parent)
    : QObject(parent)
{
    m_capture = new QScreenCapture(this);
    m_sink = new QVideoSink(this);

    m_session.setScreenCapture(m_capture);
    m_session.setVideoSink(m_sink);

    connect(m_sink,
            &QVideoSink::videoFrameChanged,
            this,
            &MonitorCapture::frameReady);

    connect(m_capture,
            &QScreenCapture::errorOccurred,
            this,
            [this](QScreenCapture::Error error, const QString &msg)
            {
                Q_UNUSED(error);
                emit errorOccurred(msg);
            });
}

QStringList MonitorCapture::monitors() const
{
    QStringList result;

    const auto screens = QGuiApplication::screens();

    for (QScreen *screen : screens)
    {
        result << screen->name();
    }

    return result;
}

bool MonitorCapture::selectMonitor(int index)
{
    const auto screens = QGuiApplication::screens();

    if (index < 0 || index >= screens.size())
        return false;

    m_capture->setScreen(screens[index]);
    return true;
}

void MonitorCapture::start()
{
    m_capture->setActive(true);
}

void MonitorCapture::stop()
{
    m_capture->setActive(false);
}
