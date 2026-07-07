#include "identity.h"

Identity::Identity()
{

}

QString Identity::fingerprint() const
{
    return BeanChatCommon::Crypto::fingerprint(publicKey);
}

QString Identity::publicKeyBase64() const
{
    return QString::fromUtf8(publicKey.toBase64());
}

QString Identity::privateKeyBase64() const
{
    return QString::fromUtf8(privateKey.toBase64());
}

void Identity::setPublicKeyBase64(const QString& value)
{
    publicKey = QByteArray::fromBase64(value.toUtf8());
}

void Identity::setPrivateKeyBase64(const QString& value)
{
    privateKey = QByteArray::fromBase64(value.toUtf8());
}
