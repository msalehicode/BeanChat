#pragma once

#include <QAbstractListModel>
#include "clientuser.h"
#include <QPointer>

// include from BeanChatCommon (to server and client speak one language)
#include <protocol/Packets.h>
using namespace BeanChatCommon;

class ChatModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        MessageIdRole = Qt::UserRole + 1,
        SenderIdRole,
        SenderAvatarPathRole,
        AttachIdRole,
        SenderStatusRole,
        SenderNameRole,
        SenderRelationRole,
        TextRole,
        TypeRole,
        TimestampRole
    };

    explicit ChatModel(QObject *parent = nullptr);

    int rowCount(
        const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(
        const QModelIndex &index,
        int role) const override;

    QHash<int,QByteArray>
    roleNames() const override;

    Q_INVOKABLE void clear();

    void addMessage(const ChatMessagePacket& message, ClientUser *sender);

    void removeMessage(
        quint64 messageId);

    struct ChatItem
    {
        ChatMessagePacket message;
        QPointer<ClientUser> sender;
    };
private:
    void updateUserMessages(ClientUser *user, const QList<int> &roles);

    QList<ChatItem> m_messages;
    QSet<ClientUser*> m_observedUsers;
    void observeUser(ClientUser *user);
};
