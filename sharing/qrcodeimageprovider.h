#pragma once

#include <QQuickImageProvider>

class QRCodeImageProvider : public QQuickImageProvider
{
public:
    QRCodeImageProvider();

    QImage requestImage(const QString &id,
                        QSize *size,
                        const QSize &requestedSize) override;
};

