#include "qrcodegenerator.h"


#include <QPainter>
#include <QPainterPath>


#include "third-party/nayuki-qr-code-generator/qrcodegen.hpp"
using qrcodegen::QrCode;

QImage QRCodeGenerator::generate(const QString &text,
                                 const Options &options)
{
    const QrCode qr =
        QrCode::encodeText(
            text.toUtf8().constData(),
            QrCode::Ecc::HIGH);

    const int qrSize = qr.getSize();

    const int imageSize = options.size;

    const int totalModules =
        qrSize + options.border * 2;

    const double scale =
        double(imageSize) / totalModules;

    QImage image(
        imageSize,
        imageSize,
        QImage::Format_ARGB32);

    image.fill(options.background);

    QPainter painter(&image);

    painter.setRenderHint(
        QPainter::Antialiasing,
        false);

    painter.setPen(Qt::NoPen);
    painter.setBrush(options.foreground);

    for (int y = 0; y < qrSize; ++y)
    {
        for (int x = 0; x < qrSize; ++x)
        {
            if (!qr.getModule(x, y))
                continue;

            QRectF rect(
                (x + options.border) * scale,
                (y + options.border) * scale,
                scale,
                scale);

            painter.drawRect(rect);
        }
    }

    if (!options.centerLogo.isNull())
    {
        int logoSize =
            imageSize * options.logoPercent / 100;

        QRect logoRect(
            (imageSize - logoSize) / 2,
            (imageSize - logoSize) / 2,
            logoSize,
            logoSize);

        if (options.roundedLogoBackground)
        {
            QRect bg = logoRect.adjusted(-8,-8,8,8);

            QPainterPath path;
            path.addRoundedRect(bg,16,16);

            painter.fillPath(path, Qt::white);
        }

        painter.drawImage(
            logoRect,
            options.centerLogo.scaled(
                logoRect.size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation));
    }

    return image;
}
