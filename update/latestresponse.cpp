#include "latestresponse.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

#include "logging/loggingcategories.h"

bool LatestResponse::load(const QByteArray &json)
{
    qCInfo(_updater) << "loading latest response..";
    m_success = false;
    m_updateAvailable = false;
    m_mandatory = false;
    m_latestVersion = {};
    m_manifestUrl.clear();
    m_badgesUrl.clear();

    QJsonParseError error;

    QJsonDocument document =
        QJsonDocument::fromJson(
            json,
            &error);

    if(document.isNull())
    {
        qCCritical(_updater) << "latest response is null, error:" << error.errorString();
        return false;
    }

    if(!document.isObject())
    {
        qCCritical(_updater) << "Latest response is not an object.";
        return false;
    }

    QJsonObject root =
        document.object();

    m_success =
        root["success"].toBool();

    m_updateAvailable =
        root["updateAvailable"].toBool();

    m_mandatory =
        root["mandatory"].toBool();

    m_latestVersion =
        QVersionNumber::fromString(
            root["latestVersion"].toString());

    m_manifestUrl =
        root["manifest"].toString();

    m_badgesUrl =
        root["badges"].toString();

    qCInfo(_updater) << "====== Latest Response ======";
    qCInfo(_updater) << "Success :" << m_success;
    qCInfo(_updater) << "Update? :" << m_updateAvailable;
    qCInfo(_updater) << "Mandatory:" << m_mandatory;
    qCInfo(_updater) << "Version :" << m_latestVersion.toString();
    qCInfo(_updater) << "Manifest:" << m_manifestUrl;
    qCInfo(_updater) << "Badges:" << m_badgesUrl;
    qCInfo(_updater) << "=============================";

    return true;
}


bool LatestResponse::success() const
{
    return m_success;
}

bool LatestResponse::updateAvailable() const
{
    return m_updateAvailable;
}

bool LatestResponse::mandatory() const
{
    return m_mandatory;
}

QVersionNumber LatestResponse::latestVersion() const
{
    return m_latestVersion;
}

QString LatestResponse::manifestUrl() const
{
    return m_manifestUrl;
}

QString LatestResponse::badgesUrl() const
{
    return m_badgesUrl;
}
