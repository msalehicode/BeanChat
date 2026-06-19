#pragma once

#include <QAbstractListModel>

#include <QElapsedTimer>
#include <QTimer>

struct UserItem
{
    quint64 id;
    QString username;

    bool isTalking = false;
    bool muted = false;
    bool deafened = false;

    QElapsedTimer lastVoicePacket;

    UserItem()
    {
        lastVoicePacket.start();
    }
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

    void updateUserStatus(quint64 userId, bool isTalking, bool isMuted, bool isDefened);

    UserItem* getUser(quint64 channelId, quint64 userId);

    void removeUser(
        quint64 userId);

    void moveUser(
        quint64 userId,
        quint64 newChannelId);



    void setCurrentChannelId(quint64 channelId);

private slots:
    void updateTalkingUsers();
private:

    ChannelItem* findChannel(
        quint64 id);

    ChannelItem* findChannelOfUser(
        quint64 userId);

    UserItem* findUserInChannel(ChannelItem *channel, quint64 userId);

private:
    quint64 m_currentChannelId=0;

    QTimer m_talkingTimer;

    QList<ChannelItem> m_channels;
    friend class User;
};
