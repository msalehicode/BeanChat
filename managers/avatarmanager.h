#ifndef AVATARMANAGER_H
#define AVATARMANAGER_H

#include <QObject>

#include <QFile>
#include <QDir>
#include <QUrl>


class AvatarManager : public QObject
{
    Q_OBJECT
public:
    explicit AvatarManager(QObject *parent = nullptr);


    QByteArray imageFileToBytes(const QString& path);

    bool saveAvatar(
        const QString& serverDir,
        const QString& hash,
        const QByteArray& avatarData);

    bool deleteAvatar(
        const QString& serverDir,
        const QString& hash);

    bool avatarExists(
        const QString& serverDir,
        const QString& hash);

signals:
};

#endif // AVATARMANAGER_H
