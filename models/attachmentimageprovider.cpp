#include "attachmentimageprovider.h"

AttachmentImageProvider::AttachmentImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage AttachmentImageProvider::requestImage(
    const QString &id,
    QSize *size,
    const QSize &requestedSize)
{
    const quint64 attachmentId = id.toULongLong();

    QString path;

    {
        QMutexLocker locker(&m_mutex);

        auto it = m_imagePaths.constFind(attachmentId);
        if (it == m_imagePaths.constEnd())
            return {};

        path = *it;
    }

    QImage image(path);

    if (image.isNull())
        return {};

    if (requestedSize.isValid())
    {
        image = image.scaled(
            requestedSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation);
    }

    if (size)
        *size = image.size();

    return image;
}

void AttachmentImageProvider::setImagePath(
    quint64 attachmentId,
    const QString &path)
{
    QMutexLocker locker(&m_mutex);

    m_imagePaths.insert(attachmentId, path);
}

bool AttachmentImageProvider::hasImage(quint64 attachmentId) const
{
    QMutexLocker locker(&m_mutex);

    return m_imagePaths.contains(attachmentId);
}

void AttachmentImageProvider::removeImage(quint64 attachmentId)
{
    QMutexLocker locker(&m_mutex);

    m_imagePaths.remove(attachmentId);
}

void AttachmentImageProvider::clear()
{
    QMutexLocker locker(&m_mutex);

    m_imagePaths.clear();
}

QString AttachmentImageProvider::imagePath(quint64 attachmentId) const
{
    QMutexLocker locker(&m_mutex);

    auto it = m_imagePaths.constFind(attachmentId);

    if (it == m_imagePaths.constEnd())
        return {};

    return *it;
}
