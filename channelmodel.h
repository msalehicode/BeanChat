#pragma once

#include <QAbstractListModel>

struct UserItem
{
    quint64 id;
    QString username;

    bool muted = false;
    bool deafened = false;
};

struct ChannelItem
{
    quint64 id;
    QString name;

    QList<UserItem> users;
};

class ChannelModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        NameRole,
        UsersRole
    };

    explicit ChannelModel(QObject* parent = nullptr);

    int rowCount(
        const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(
        const QModelIndex& index,
        int role) const override;

    QHash<int,QByteArray> roleNames() const override;

public:

    Q_INVOKABLE void clear();

    void addChannel(
        quint64 id,
        const QString& name);

    void addUser(
        quint64 channelId,
        quint64 userId,
        const QString& username,
        bool muted = false,
        bool deafened = false);

    void removeUser(
        quint64 userId);

    void moveUser(
        quint64 userId,
        quint64 newChannelId);

private:

    ChannelItem* findChannel(
        quint64 id);

    ChannelItem* findChannelOfUser(
        quint64 userId);

private:

    QList<ChannelItem> m_channels;
};
