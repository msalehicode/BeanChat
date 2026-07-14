#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QFile>
#include <QFileInfo>


class CrashReporter : public QObject
{
    Q_OBJECT

public:
    explicit CrashReporter(QObject *parent = nullptr);

    void setUploadUrl(const QString &url);
    void setApiKey(const QString &apiKey);

    void upload(const QString &logFile);

signals:
    void uploadFinished(const QString &logFile,
                        bool success,
                        const QString &errorString);

private:
    QNetworkAccessManager m_network;

    QString m_url;
    QString m_apiKey;
};
