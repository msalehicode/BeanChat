#include "servercode.h"

#include <QStringList>
#include <QVariantMap>

namespace
{
    const QString Alphabet = "23456789ABCDEFGHJKLMNPQRSTUVWXYZ"; //0,1,o,O,i,I,l removed to avoid type or read problems
}

QString ServerCode::encode(const QString &ip,
                           quint16 port)
{
    quint32 address;

    if (!parseIPv4(ip.trimmed(), address))
        return QString();

    quint64 value =
        (static_cast<quint64>(address) << 16) |
        port;

    QString code = encodeBase32(value);

    // XXX-XXX-XXXX
    return code.left(3) + "-" +
           code.mid(3, 3) + "-" +
           code.mid(6);
}

bool ServerCode::decode(const QString &code,
                        QString &ip,
                        quint16 &port)
{
    QString normalized = normalizeCode(code);

    if (normalized.isEmpty())
        return false;

    quint64 value;

    if (!decodeBase32(normalized, value))
        return false;

    quint32 address =
        static_cast<quint32>(value >> 16);

    port =
        static_cast<quint16>(value & 0xFFFF);

    ip = formatIPv4(address);

    return true;
}

QVariantMap ServerCode::decode(const QString &code)
{
    QString ip;
    quint16 port;

    if (!ServerCode::decode(code, ip, port))
        return { { "ok", false } };

    return {
        { "ok", true },
        { "ip", ip },
        { "port", port }
    };
}

bool ServerCode::isIPv4(const QString &ip)
{
    quint32 address;
    return parseIPv4(ip, address);
}

bool ServerCode::parseIPv4(const QString &ip,
                           quint32 &address)
{
    QString text = ip.trimmed();

    if (text.isEmpty())
        return false;

    for (QChar c : text)
    {
        if (!(c.isDigit() || c == '.'))
            return false;
    }

    QStringList parts = text.split('.');

    if (parts.size() != 4)
        return false;

    address = 0;

    for (const QString &part : parts)
    {
        if (part.isEmpty())
            return false;

        bool ok;

        int value = part.toInt(&ok);

        if (!ok)
            return false;

        if(value < 0 || value > 255)
            return false;

        address <<= 8;
        address |= value;
    }

    return true;
}

QString ServerCode::formatIPv4(quint32 address)
{
    return QString("%1.%2.%3.%4")
    .arg((address >> 24) & 0xFF)
        .arg((address >> 16) & 0xFF)
        .arg((address >> 8) & 0xFF)
        .arg(address & 0xFF);
}

QString ServerCode::normalizeCode(const QString &code)
{
    QString out;

    for (QChar c : code)
    {
        if (c == '-' || c.isSpace())
            continue;

        c = c.toUpper();

        if (!Alphabet.contains(c))
            return QString();

        out += c;
    }

    if (out.length() != 10)
        return QString();

    return out;
}

QString ServerCode::encodeBase32(quint64 value)
{
    QString out;

    for (int i = 0; i < 10; ++i)
    {
        int index = value & 31;

        out.prepend(Alphabet[index]);

        value >>= 5;
    }

    return out;
}

bool ServerCode::decodeBase32(const QString &text,
                              quint64 &value)
{
    if (text.length() != 10)
        return false;

    value = 0;

    for (QChar c : text)
    {
        int index = Alphabet.indexOf(c);

        if (index < 0)
            return false;

        value <<= 5;
        value |= index;
    }

    // Remove the 2 padding bits (50 encoded bits -> 48 actual bits)
    value &= 0x0000FFFFFFFFFFFFULL;

    return true;
}
