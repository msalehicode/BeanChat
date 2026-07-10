#pragma once

#include <QImage>
#include <QString>
#include <QColor>

class QRCodeGenerator
{
public:
    struct Options
    {
        int size = 512;

        int border = 3;

        QColor foreground = Qt::black;
        QColor background = Qt::white;

        QImage centerLogo;

        int logoPercent = 18; // % of QR width

        bool roundedLogoBackground = true;

        Options()
            : size(512),
            border(3),
            foreground(Qt::black),
            background(Qt::white),
            logoPercent(18),
            roundedLogoBackground(true)
        {
        }
    };

public:
    static QImage generate(
        const QString &text,
        const Options &options = Options());
};




/*
    QImage image = QRCodeGenerator::generate("Hello BeanChat!");
    if(image.isNull())
        qDebug() << "Failed to generate QR code.";
    else
        image.save("test.png");

    QRCodeGenerator::Options options;
    options.centerLogo = QImage(":/icons/BeanChat.png");
    image = QRCodeGenerator::generate("Hello BeanChat!", options);
    if(image.isNull())
        qDebug() << "Failed to generate QR code.";
    else
        image.save("test2.png");
*/
