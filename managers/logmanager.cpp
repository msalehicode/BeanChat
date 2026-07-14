#include "logmanager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

namespace
{
    constexpr qint64 CrashCheckBytes = 4096;
    constexpr auto SessionEndMarker = "=== SESSION END ===";
}


LogManager::LogManager(const QString &logsDirectory)
    : m_logsDirectory(logsDirectory)
{
}


QVector<LogFile> LogManager::logs() const
{
    return scanLogs();
}


QVector<LogFile> LogManager::crashLogs() const
{
    QVector<LogFile> result;

    const auto allLogs = scanLogs();

    for (const LogFile &log : allLogs)
    {
        if (log.crashed &&
            !log.uploaded)
        {
            result.push_back(log);
        }
    }

    return result;
}


QVector<LogFile> LogManager::scanLogs() const
{
    QVector<LogFile> logs;

    QDir directory(m_logsDirectory);

    if (!directory.exists())
        return logs;

    const QFileInfoList files =
        directory.entryInfoList(
            { "*.log" },
            QDir::Files,
            QDir::Time);

    logs.reserve(files.size());

    for (const QFileInfo &info : files)
    {
        LogFile log;

        log.filePath = info.absoluteFilePath();
        log.fileName = info.fileName();

        log.created = info.birthTime().isValid()
                          ? info.birthTime()
                          : info.lastModified();

        log.size = info.size();

        // log.current = (info.absoluteFilePath() == Logger::currentLogFile());

        log.uploaded = wasUploaded(log.filePath);

        log.crashed = isCrashLog(log.filePath);

        logs.push_back(std::move(log));
    }

    return logs;
}

bool LogManager::wasUploaded(const QString &filePath) const
{
    return QFile::exists(filePath + ".sent");
}

bool LogManager::markUploaded(const QString &filePath) const
{
    QFile marker(filePath + ".sent");

    if (marker.exists())
        return true;

    if (!marker.open(QIODevice::WriteOnly))
        return false;

    marker.close();

    return true;
}

bool LogManager::isCrashLog(const QString &filePath) const
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    const qint64 size = file.size();

    if (size > CrashCheckBytes)
        file.seek(size - CrashCheckBytes);

    const QString tail =
        QString::fromUtf8(file.readAll());

    return !tail.contains(SessionEndMarker);
}


void LogManager::cleanupOldLogs(int keepCount) const
{
    auto allLogs = scanLogs();

    if (allLogs.size() <= keepCount)
        return;

    for (int i = keepCount; i < allLogs.size(); ++i)
    {
        QFile::remove(allLogs[i].filePath);

        QFile::remove(allLogs[i].filePath + ".sent");
    }
}






