#pragma once

#include <QObject>
#include <QFile>
#include <QMutex>

class Logger : public QObject
{
    Q_OBJECT

public:
    explicit Logger(QObject *parent = nullptr);

    bool initialize(const QString &logsDirectory);

    QString currentLogFile() const;

    Q_INVOKABLE void debug(const QString &category,
                           const QString &message);

    Q_INVOKABLE void info(const QString &category,
                          const QString &message);

    Q_INVOKABLE void warning(const QString &category,
                             const QString &message);

    Q_INVOKABLE void critical(const QString &category,
                              const QString &message);

    Q_INVOKABLE void action(const QString &message);

public slots:
    void shutdown();

private:
    Q_DISABLE_COPY_MOVE(Logger)

    static void messageHandler(QtMsgType type,
                               const QMessageLogContext &context,
                               const QString &message);

    void writeMessage(QtMsgType type,
                      const QMessageLogContext &context,
                      const QString &message);

private:
    static Logger* s_instance;

    QFile m_file;
    QString m_logFilePath;
    QMutex m_mutex;
};
