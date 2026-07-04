#pragma once

#include <QAbstractListModel>

#include <QElapsedTimer>
#include <QTimer>
#include "clientuser.h"

struct UserItem
{
    ClientUser *user = nullptr;
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
    bool isLocked=false;
    bool saveChats=false;

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
        IsLcokedRole,
        SaveChatsRole,
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
        const QString& name, bool isLocked, bool saveChat);


    void updateChannel(quint64 id,
                       const QString &name,
                       bool isLocked,
                       bool saveChats);

    void removeChannel(quint64 channelId);

    ClientUser* getUser(quint64 channelId, quint64 userId);
    QString getChannelName(quint64 channelId);

    void removeUser(
        quint64 userId);

    void moveUser(
        quint64 userId,
        quint64 newChannelId);



    void setCurrentChannelId(quint64 channelId);

    void resetChannelTalkingStatus(quint64 channelId);
    bool getChannelSaveChats(quint64 channelId);
    void addUser(quint64 channelId, ClientUser *user);
    ChannelItem* findChannel(quint64 id);
    ChannelItem* findChannelOfUser(quint64 userId);
    ClientUser* findUserInChannel(ChannelItem *channel, quint64 userId);
    void restartVoiceTimer(quint64 userId);
signals:
    void userTalkingStatus(quint64 userId,bool status);

private slots:
    void updateTalkingUsers();
private:


private:
    UserItem *findUserItem(quint64 userId);
    int findRow(ChannelItem *channel) const;
    int findRow(ClientUser *user) const;
    void observeUser(ClientUser *user);

    quint64 m_currentChannelId=0;
    QTimer m_talkingTimer;

    QList<ChannelItem> m_channels;
    QSet<ClientUser*> m_observedUsers;

};
