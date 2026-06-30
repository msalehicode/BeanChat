#include "avatarmanager.h"

AvatarManager::AvatarManager(QObject *parent)
    : QObject{parent}
{}


QByteArray AvatarManager::imageFileToBytes(
    const QString& path)
{
    QString localPath =
        QUrl(path).toLocalFile();

    QFile file(localPath);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "failed to open" << localPath;

        return {};
    }

    return file.readAll();
}

bool AvatarManager::saveAvatar(const QString &serverDir, const QString &hash, const QByteArray &avatarData)
{
    QDir().mkpath(serverDir);

    QFile file(
        serverDir + "/" +
        hash + ".png");

    if (!file.open(QIODevice::WriteOnly))
        return false;

    file.write(avatarData);

    return true;
}

bool AvatarManager::deleteAvatar(const QString &serverDir, const QString &hash)
{
    return QFile::remove(
        serverDir + "/" +
        hash + ".png");
}

bool AvatarManager::avatarExists(const QString &serverDir, const QString &hash)
{
    return QFile::exists(
        serverDir + "/" +
        hash + ".png");
}
