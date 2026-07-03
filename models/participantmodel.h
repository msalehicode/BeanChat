#pragma once

#include <QAbstractListModel>
#include "../video/videosink.h"
#include "clientuser.h"

class ParticipantModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        UsernameRole = Qt::UserRole + 1,
        UserAvatarPathRole,
        VideoSinkRole,
        IsTalkingRole,
        IsCameraOpenRole,
        UserId,
        IsDeafened,
        IsMuted
    };

    explicit ParticipantModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(
        const QModelIndex &index,
        int role) const override;

    QHash<int,QByteArray>
    roleNames() const override;

    void addUser(ClientUser *user);

    void removeUser(const quint64 &userId);

    void clear();

    ClientUser* findUser(quint64 userId);

    VideoSink *videoSink(quint64 userId);
private:
    int findRow(ClientUser *user) const;
    void observeUser(ClientUser* user);
    struct ParticipantData
    {
        ClientUser *user = nullptr;
        VideoSink *videoSink = nullptr;
    };

    QList<ParticipantData> m_users;
};
