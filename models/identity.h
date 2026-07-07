#pragma once

#include <QString>
#include <QByteArray>
#include <crypto/Crypto.h>


class Identity
{
public:
    Identity();

    QString name;

    QByteArray publicKey;
    QByteArray privateKey;

    qint64 createdAt = 0;

    QString fingerprint() const;

    QString publicKeyBase64() const;
    QString privateKeyBase64() const;

    void setPublicKeyBase64(
        const QString& value);

    void setPrivateKeyBase64(
        const QString& value);
};
