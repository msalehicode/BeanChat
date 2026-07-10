#pragma once

#include <QObject>
#include <QNetworkAccessManager>

#include "latestresponse.h"

class UpdateChecker : public QObject
{
    Q_OBJECT

public:
    explicit UpdateChecker(QObject *parent = nullptr);

    void checkForUpdates(const QString &platform,
                         const QString &currentVersion);

signals:
    void updateAvailable(const LatestResponse &response);
    void noUpdateAvailable();
    void errorOccurred(const QString &error);

private:
    QNetworkAccessManager m_network;
    LatestResponse m_latestResponse;

    const QString m_latestPath =
        "https://beanchat.ir/bc/api/latest.php";
};
