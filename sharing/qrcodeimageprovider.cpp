#include "qrcodeimageprovider.h"

#include "qrcodegenerator.h"

QRCodeImageProvider::QRCodeImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage QRCodeImageProvider::requestImage(
    const QString &id,
    QSize *size,
    const QSize &requestedSize)
{
    QRCodeGenerator::Options options;
    options.centerLogo = QImage(":/icons/BeanChatQR.png");
    options.logoPercent=20;//20%

    if (requestedSize.isValid())
        options.size = qMin(requestedSize.width(),
                            requestedSize.height());

    QImage image =
        QRCodeGenerator::generate(id, options);

    if (size)
        *size = image.size();

    return image;
}
