#include "logger.h"

#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>

#include "loggingcategories.h"
#include "systeminfo.h"

Logger *Logger::s_instance = nullptr;

Logger::Logger(QObject *parent)
    : QObject(parent)
{
}

bool Logger::initialize(const QString &logsDirectory)
{
    if (s_instance)
        return false;

    QDir dir(logsDirectory);

    if (!dir.exists() && !dir.mkpath("."))
        return false;

    m_logFilePath = dir.filePath(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".log");

    m_file.setFileName(m_logFilePath);

    if (!m_file.open(QIODevice::WriteOnly |
                     QIODevice::Append |
                     QIODevice::Text))
    {
        return false;
    }

    s_instance = this;

    qInstallMessageHandler(Logger::messageHandler);


    //add system info to log file
    QTextStream stream(&m_file);
    stream << SystemInfo::diagnosticReport();
    stream << "=== SESSION START ===";
    stream << "\n\n";
    stream.flush();

    qInfo() << "Logger initialized, path=" << m_logFilePath;
    return true;
}

void Logger::shutdown()
{
    if (s_instance != this)
        return;

    qInfo() << "Logger shutting down.";
    qInfo() << "=== SESSION END ===";
    qInstallMessageHandler(nullptr);

    if (m_file.isOpen())
        m_file.close();

    s_instance = nullptr;
}

QString Logger::currentLogFile() const
{
    return m_logFilePath;
}

void Logger::messageHandler(QtMsgType type,
                            const QMessageLogContext &context,
                            const QString &message)
{
    if (s_instance)
        s_instance->writeMessage(type, context, message);
}

void Logger::writeMessage(QtMsgType type,
                          const QMessageLogContext &context,
                          const QString &message)
{
    QMutexLocker locker(&m_mutex);

    if (!m_file.isOpen())
        return;

    QString level;

    switch (type)
    {
    case QtDebugMsg:
        level = "DEBUG";
        break;

    case QtInfoMsg:
        level = "INFO";
        break;

    case QtWarningMsg:
        level = "WARNING";
        break;

    case QtCriticalMsg:
        level = "CRITICAL";
        break;

    case QtFatalMsg:
        level = "FATAL";
        break;
    }


#ifdef QT_NO_DEBUG
    const QString fileName =
        context.file
            ? QFileInfo(context.file).fileName()
            : "?";

    const QString function =
        context.function
            ? QString::fromUtf8(context.function)
            : "?";
#endif

    QTextStream stream(&m_file);


    stream << '['
           << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")
           << "] "
           << '['
           << level.leftJustified(8)
           << "] "
           << '['
           << QString(context.category).leftJustified(12)
           << "] "
#ifdef QT_NO_DEBUG
           << '('
           << fileName
           << "::"
           << context.line
           << " ("
           << function
           << "):"
#endif
           <<"\n\n"
           << message
           << "\n"
           << Qt::endl;

    stream.flush();

    // fprintf(stderr,
    //         "[%s] [%s] [%s] (%s::%d):\n %s\n",
    //         qPrintable(QDateTime::currentDateTime().toString("HH:mm:ss.zzz")),
    //         qPrintable(level),
    //         context.category,
    //         context.file ? context.file : "?",
    //         context.line,
    //         qPrintable(message));

    //simple
#ifdef QT_NO_DEBUG
    fprintf(stderr,
            "[%s] %s\n",
            qPrintable(level),
            qPrintable(message));
#endif

    if (type == QtFatalMsg)
        abort();
}


void Logger::debug(const QString &category,
                   const QString &message)
{
    qCDebug(_ui).noquote()
    << '[' << category << ']'
    << message;
}

void Logger::info(const QString &category,
                  const QString &message)
{
    qCInfo(_ui).noquote()
    << '[' << category << ']'
    << message;
}

void Logger::warning(const QString &category,
                     const QString &message)
{
    qCWarning(_ui).noquote()
    << '[' << category << ']'
    << message;
}

void Logger::critical(const QString &category,
                      const QString &message)
{
    qCCritical(_ui).noquote()
    << '[' << category << ']'
    << message;
}

void Logger::action(const QString &message)
{
    qCInfo(_ui).noquote()
    << "[ACTION] "
    << message;
}


