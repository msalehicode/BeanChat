#pragma once

#include <QFile>
#include <QString>
#include <QByteArray>

struct DownloadSession
{
    quint64 attachmentId = 0;

    QString originalFilename;
    QString mimeType;

    qint64 expectedSize = 0;
    qint64 receivedSize = 0;

    QByteArray sha256;

    std::unique_ptr<QFile> file;

    float progress() const
    {
        if (expectedSize <= 0)
            return 0.0f;

        return float(receivedSize) / float(expectedSize);
    }
};
