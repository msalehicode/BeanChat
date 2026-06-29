#pragma once

#include <QAbstractListModel>

// include from BeanChatServer to server and client speak one language and undrestand packets
#include "network/packets.h"

class ChatModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Roles
    {
        MessageIdRole = Qt::UserRole + 1,
        SenderIdRole,
        SenderNameRole,
        TextRole,
        TypeRole,
        MediaPathRole,
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

public:

    Q_INVOKABLE void clear();

    void addMessage(
        const ChatMessagePacket& message);

    void removeMessage(
        quint64 messageId);

private:

    QList<ChatMessagePacket> m_messages;
};
