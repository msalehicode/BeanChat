#include "crashreporter.h"

#include <QJsonDocument>
#include <QJsonObject>

CrashReporter::CrashReporter(QObject *parent)
    : QObject(parent)
{
}

void CrashReporter::setUploadUrl(const QString &url)
{
    m_url = url;
}

void CrashReporter::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void CrashReporter::upload(const QString &logFile)
{
    if (m_url.isEmpty())
    {
        emit uploadFinished(
            logFile,
            false,
            "Upload URL has not been configured.");

        return;
    }

    QFile *file = new QFile(logFile);

    if (!file->open(QIODevice::ReadOnly))
    {
        delete file;

        emit uploadFinished(
            logFile,
            false,
            "Failed to open log file.");

        return;
    }

    auto *multipart =
        new QHttpMultiPart(
            QHttpMultiPart::FormDataType);

    QHttpPart filePart;

    filePart.setHeader(
        QNetworkRequest::ContentDispositionHeader,
        QString("form-data; name=\"log\"; filename=\"%1\"")
            .arg(QFileInfo(logFile).fileName()));

    filePart.setBodyDevice(file);

    file->setParent(multipart);

    multipart->append(filePart);

    QNetworkRequest request = QNetworkRequest(QUrl(m_url));

    request.setHeader(
        QNetworkRequest::UserAgentHeader,
        QStringLiteral("BeanChat/%1").arg(APP_VERSION));

    if (!m_apiKey.isEmpty())
    {
        request.setRawHeader(
            "X-BeanChat-Key",
            m_apiKey.toUtf8());
    }

    QNetworkReply *reply =
        m_network.post(
            request,
            multipart);

    multipart->setParent(reply);

    connect(reply,
            &QNetworkReply::finished,
            this,
            [this, reply, logFile]()
            {
                const QByteArray response = reply->readAll();

                QString errorString;
                bool success = false;

                if (reply->error() != QNetworkReply::NoError)
                {
                    errorString = reply->errorString();
                }
                else
                {
                    const int statusCode =
                        reply->attribute(
                                 QNetworkRequest::HttpStatusCodeAttribute)
                            .toInt();

                    if (statusCode != 200)
                    {
                        errorString =
                            QStringLiteral("HTTP %1")
                                .arg(statusCode);
                    }
                    else
                    {
                        QJsonParseError parseError;

                        const QJsonDocument json =
                            QJsonDocument::fromJson(
                                response,
                                &parseError);

                        if (parseError.error != QJsonParseError::NoError)
                        {
                            errorString =
                                QStringLiteral("Invalid server response.");
                        }
                        else
                        {
                            const QJsonObject object =
                                json.object();

                            success =
                                object.value("success")
                                    .toBool(false);

                            if (!success)
                            {
                                errorString =
                                    object.value("error")
                                        .toString("Upload failed.");
                            }
                        }
                    }
                }

                reply->deleteLater();

                emit uploadFinished(
                    logFile,
                    success,
                    errorString);
            });
}
