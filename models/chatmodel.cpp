#include "chatmodel.h"

#include "logging/loggingcategories.h"

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
    if (!index.isValid() ||
        index.row() < 0 ||
        index.row() >= m_messages.size())
    {
        qCCritical(_models) << "invalid index to get chat data";
        return {};
    }


    const ChatItem &item = m_messages[index.row()];

    switch(role)
    {
        case MessageIdRole:
            return QVariant::fromValue(item.message.messageId);

        case SenderIdRole:
            return item.sender
                       ? item.sender->id()
                       : item.message.senderId;

        case SenderStatusRole:
            return item.sender
                        ? static_cast<int>(item.sender->status())
                        : static_cast<int>(ClientUser::Status::Offline);

        case SenderRelationRole:
            return item.sender
                        ? static_cast<int>(item.sender->relationship())
                        : static_cast<int>(Relationship::Type::None);

        case SenderAvatarPathRole:
            return item.sender
                       ? item.sender->avatarPath()
                       : QString();

        case SenderNameRole:
            return item.sender
                       ? item.sender->username()  // live username
                       : item.message.senderName; // cached username in case user is not connected. and cant access his name

        case TextRole:
            return item.message.text;

        case TypeRole:
            return static_cast<int>(item.message.type);

        case MediaPathRole:
            return item.message.mediaPath;

        case TimestampRole:
            return item.message.timestamp;
    }


    qCWarning(_models) << "chat data, return {}";
    return {};
}

QHash<int,QByteArray> ChatModel::roleNames() const
{
    return
        {
            { MessageIdRole, "messageId" },
            { SenderIdRole, "senderId" },
            { SenderNameRole, "senderName"},
            { SenderAvatarPathRole, "senderAvatarPath"},
            { SenderRelationRole, "senderRelation"},
            { SenderStatusRole, "senderStatus"},
            { TextRole, "textMessage" },
            { TypeRole, "messageType" },
            { MediaPathRole, "mediaPath" },
            { TimestampRole, "timestamp" }
        };
}

void ChatModel::updateUserMessages(ClientUser *user, const QList<int> &roles)
{
    for (int row = 0; row < m_messages.size(); ++row)
    {
        if (m_messages[row].sender == user)
            emit dataChanged(index(row), index(row), roles);
    }
}

void ChatModel::clear()
{
    qCInfo(_models) << "clear chatModel";
    beginResetModel();

    m_messages.clear();
    m_observedUsers.clear();

    endResetModel();
}

void ChatModel::addMessage(const ChatMessagePacket &message, ClientUser* sender)
{
    qCInfo(_models) << "add message to chat.";
    if(!sender)
    {
        qCCritical(_models) << "failed to add message, invalid sender object.";
        return;
    }

    ChatItem item;
    item.message = message;
    item.sender = sender;

    observeUser(sender);

    beginInsertRows(
        QModelIndex(),
        rowCount(),
        rowCount());

    m_messages.append(item);

    endInsertRows();
}

void ChatModel::observeUser(ClientUser *user)
{

    if (!user || m_observedUsers.contains(user))
    {
        // qCCritical(_models) << "failed to observe user, invalid user OR user has already exist in observedUsers.";
        return;
    }
    qCInfo(_models) << "add user to observe for chat";
    m_observedUsers.insert(user);

    connect(user,
            &ClientUser::usernameChanged,
            this,
            [this, user]()
            {
                updateUserMessages(user, { SenderNameRole });
            });

    connect(user,
            &ClientUser::relationshipChanged,
            this,
            [this, user]()
            {
                updateUserMessages(user, { SenderRelationRole });
            });


    connect(user,
            &ClientUser::statusChanged,
            this,
            [this, user]()
            {
                updateUserMessages(user, { SenderStatusRole });
            });

    connect(user,
            &ClientUser::avatarPathChanged,
            this,
            [this, user]()
            {
                updateUserMessages(user, { SenderAvatarPathRole });
            });
}

void ChatModel::removeMessage(
    quint64 messageId)
{
    qCInfo(_models) << "remove message, msg-id=" << messageId;
    for(int i=0;i<m_messages.size();i++)
    {
        if (m_messages[i].message.messageId == messageId)
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

    qCWarning(_models) << "failed to remove message from chat";
}
