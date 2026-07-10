#include "updatechecker.h"

#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

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

    QNetworkReply *reply = m_network.get(QNetworkRequest(url));

    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, reply]()
            {
                if (reply->error() != QNetworkReply::NoError)
                {
                    emit errorOccurred(reply->errorString());
                    reply->deleteLater();
                    return;
                }

                if (!m_latestResponse.load(reply->readAll()))
                {
                    emit errorOccurred("Failed to parse server response.");
                    reply->deleteLater();
                    return;
                }

                if (!m_latestResponse.success())
                {
                    emit errorOccurred("Server returned an error.");
                    reply->deleteLater();
                    return;
                }

                if (m_latestResponse.updateAvailable())
                    emit updateAvailable(m_latestResponse);
                else
                    emit noUpdateAvailable();

                reply->deleteLater();
            });
}
