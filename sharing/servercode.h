#pragma once

#include <QObject>
#include <QString>


class ServerCode : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE static QString encode(const QString &ip,
                          quint16 port);

    static bool decode(const QString &code,
                       QString &ip,
                       quint16 &port);
    Q_INVOKABLE QVariantMap decode(const QString &code); // for qml to get values

    Q_INVOKABLE static bool isIPv4(const QString &ip);

private:
    static bool parseIPv4(const QString &ip,
                          quint32 &address);

    static QString formatIPv4(quint32 address);

    static QString normalizeCode(const QString &code);

    static QString encodeBase32(quint64 value);

    static bool decodeBase32(const QString &text,
                             quint64 &value);
};


/*
    //test sharing server code:
    QString code = ServerCode::encode("87.248.131.168", 9987);
    qDebug() << "encoded ipport= " << code;


    QString ip;
    quint16 port;
    if (ServerCode::decode(code, ip, port))
        qDebug() << "decoded servercode= " << ip << " : " << port;
    else
        qDebug() << "failed to decode.";

*/
