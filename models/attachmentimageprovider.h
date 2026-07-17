#pragma once

#include <QHash>
#include <QMutex>
#include <QQuickImageProvider>

class AttachmentImageProvider : public QQuickImageProvider
{
public:
    AttachmentImageProvider();

    QImage requestImage(
        const QString &id,
        QSize *size,
        const QSize &requestedSize) override;

    void setImagePath(
        quint64 attachmentId,
        const QString &path);

    bool hasImage(quint64 attachmentId) const;

    void removeImage(quint64 attachmentId);

    void clear();

    QString imagePath(quint64 attachmentId) const;

private:
    mutable QMutex m_mutex;

    QHash<quint64, QString> m_imagePaths;
};
