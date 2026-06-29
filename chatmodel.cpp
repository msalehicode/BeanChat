#include "chatmodel.h"

ChatModel::ChatModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ChatModel::rowCount(
    const QModelIndex &) const
{
    return m_messages.size();
}

QVariant ChatModel::data(
    const QModelIndex &index,
    int role) const
{
    if(!index.isValid())
        return {};

    const auto& msg =
        m_messages[index.row()];

    switch(role)
    {
    case MessageIdRole:
        return QVariant::fromValue(msg.messageId);

    case SenderIdRole:
        return QVariant::fromValue(msg.senderId);

    case SenderNameRole:
        return msg.senderName;

    case TextRole:
        return msg.text;

    case TypeRole:
        return static_cast<int>(msg.type);

    case MediaPathRole:
        return msg.mediaPath;

    case TimestampRole:
        return msg.timestamp;
    }

    return {};
}

QHash<int,QByteArray>
ChatModel::roleNames() const
{
    return
        {
            { MessageIdRole, "messageId" },
            { SenderIdRole, "senderId" },
            { SenderNameRole, "senderName"},
            { TextRole, "textMessage" },
            { TypeRole, "messageType" },
            { MediaPathRole, "mediaPath" },
            { TimestampRole, "timestamp" }
        };
}

void ChatModel::clear()
{
    beginResetModel();

    m_messages.clear();

    endResetModel();
}

void ChatModel::addMessage(
    const ChatMessagePacket &message)
{
    beginInsertRows(
        QModelIndex(),
        rowCount(),
        rowCount());

    m_messages.push_back(message);

    endInsertRows();
}

void ChatModel::removeMessage(
    quint64 messageId)
{
    for(int i=0;i<m_messages.size();i++)
    {
        if(m_messages[i].messageId ==
            messageId)
        {
            beginRemoveRows(
                QModelIndex(),
                i,
                i);

            m_messages.removeAt(i);

            endRemoveRows();

            return;
        }
    }
}
