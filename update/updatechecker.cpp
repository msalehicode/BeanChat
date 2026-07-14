#include "updatechecker.h"

#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#include "logging/loggingcategories.h"

UpdateChecker::UpdateChecker(QObject *parent)
    : QObject(parent)
{
}

void UpdateChecker::checkForUpdates(const QString &platform,
                                    const QString &currentVersion)
{
    QUrl url(m_latestPath);

    QUrlQuery query;
    query.addQueryItem("platform", platform);
    query.addQueryItem("version", currentVersion);
    url.setQuery(query);

    qCInfo(_updater) << "checkForUpdates url=" <<url.toString() << " query=" << query.toString();
    QNetworkReply *reply = m_network.get(QNetworkRequest(url));

    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, reply]()
            {
                if (reply->error() != QNetworkReply::NoError)
                {
                    qCWarning(_updater) << "server replied error=" << reply->errorString();
                    emit errorOccurred(reply->errorString());
                    reply->deleteLater();
                    return;
                }

                if (!m_latestResponse.load(reply->readAll()))
                {
                    qCWarning(_updater) << "Failed to parse server response.";
                    emit errorOccurred("Failed to parse server response.");
                    reply->deleteLater();
                    return;
                }

                if (!m_latestResponse.success())
                {
                    qCWarning(_updater) << "server returned not success flag.";
                    emit errorOccurred("Server returned an error.");
                    reply->deleteLater();
                    return;
                }

                if (m_latestResponse.updateAvailable())
                {
                    qCInfo(_updater) << "update is available, latest version = " << m_latestResponse.latestVersion();
                    emit updateAvailable(m_latestResponse);
                }
                else
                {
                    qCInfo(_updater) << "no update available.";
                    emit noUpdateAvailable();
                }

                reply->deleteLater();
            });
}
