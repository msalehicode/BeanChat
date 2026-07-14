#pragma once

#include <QDateTime>
#include <QString>
#include <QVector>

struct LogFile
{
    QString filePath;
    QString fileName;

    QDateTime created;


    qint64 size = 0;

    bool current = false;
    bool crashed = false;
    bool uploaded = false;
};

class LogManager
{
public:
    explicit LogManager(const QString &logsDirectory);

    QVector<LogFile> logs() const;

    QVector<LogFile> crashLogs() const;

    void cleanupOldLogs(int keepCount = 5) const;

    bool isCrashLog(const QString &filePath) const;

    bool wasUploaded(const QString &filePath) const;

    bool markUploaded(const QString &filePath) const;

private:
    QVector<LogFile> scanLogs() const;

private:
    QString m_logsDirectory;
};
