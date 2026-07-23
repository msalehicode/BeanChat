#pragma once

#include <QObject>
#include <QNetworkAccessManager>

#include "latestresponse.h"
#include "managers/badgemanager.h"

class UpdateChecker : public QObject
{
    Q_OBJECT

public:
    explicit UpdateChecker(BadgeManager *badgeManager, QObject *parent = nullptr);

    void checkForUpdates(const QString &platform,
                         const QString &currentVersion);

signals:
    void updateAvailable(const LatestResponse &response);
    void noUpdateAvailable();
    void errorOccurred(const QString &error);

    void errorLoadingBadges(const QString& error);
    void badgesDownloaded();

private:
    void downloadBadges(const QString &url);
    QNetworkAccessManager m_network;
    LatestResponse m_latestResponse;
    BadgeManager *m_badgeManager = nullptr;

    const QString m_latestPath =
        "https://beanchat.ir/bc/api/latest.php";
};
