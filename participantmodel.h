#pragma once

#include <QAbstractListModel>
#include "participant.h"

class ParticipantModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        UsernameRole = Qt::UserRole + 1,
        VideoSinkRole,
        IsTalkingRole,
        IsCameraOpenRole,
        UserId,
        IsDeafened
    };

    explicit ParticipantModel(QObject *parent = nullptr)
        : QAbstractListModel(parent)
    {
    }

    int rowCount(
        const QModelIndex &) const override
    {
        return m_users.size();
    }

    QVariant data(
        const QModelIndex &index,
        int role) const override
    {
        Participant *user =
            m_users[index.row()];

        switch(role)
        {
        case UsernameRole:
            return user->username();

        case IsTalkingRole:
            return user->isTalking();

        case IsCameraOpenRole:
            return user->isCameraOpen();

        case VideoSinkRole:
            return QVariant::fromValue(user->videoSink());

        case UserId:
            return user->id();

        case IsDeafened:
            return user->isDeafened();
        }

        return {};
    }

    QHash<int,QByteArray>
    roleNames() const override
    {
        return {
            {UsernameRole,"username"},
            {IsTalkingRole,"isTalking"},
            {IsCameraOpenRole,"isCameraOpen"},
            {VideoSinkRole,"videoSink"},
            {UserId,"userId"},
            {IsDeafened, "isDeafened"}
        };
    }

    void addUser(quint64 id, const QString &name, bool isTalking=false,bool isDeafened=false, bool isCameraOpen=false)
    {
        beginInsertRows(QModelIndex(), m_users.size(), m_users.size());

        m_users.append(new Participant(id,name,isTalking,isDeafened,isCameraOpen,this));

        endInsertRows();
    }

    void removeUser(const quint64 &userId)
    {
        for(int i=0;i<m_users.size();i++)
        {
            if(m_users[i]->id()==userId)
            {
                beginRemoveRows(
                    QModelIndex(),
                    i,
                    i);

                delete m_users.takeAt(i);

                endRemoveRows();
                return;
            }
        }
    }

    void clear()
    {
        beginResetModel();

        qDeleteAll(m_users); // deletes all Participant*
        m_users.clear();

        endResetModel();
    }

    void updateUser(
        quint64 userId,
        bool isTalking,
        bool isDeafened,
        bool isCameraOpen)
    {
        for(int i = 0; i < m_users.size(); ++i)
        {
            Participant* user = m_users[i];

            if(user->id() == userId)
            {
                user->setIsTalking(isTalking);
                user->setIsDeafened(isDeafened);
                user->setIsCameraOpen(isCameraOpen);

                QModelIndex idx = index(i);

                emit dataChanged(
                    idx,
                    idx,
                    {
                        IsTalkingRole,
                        IsDeafened,
                        IsCameraOpenRole
                    });

                return;
            }
        }
    }


    void setTalking(
        quint64 userId,
        bool talking=false)
    {
        for(int i = 0; i < m_users.size(); ++i)
        {
            if(m_users[i]->id() == userId)
            {
                m_users[i]->setIsTalking(talking);

                emit dataChanged(
                    index(i),
                    index(i),
                    { IsTalkingRole });

                return;
            }
        }
    }

    void setCameraOpen(
        quint64 userId,
        bool open)
    {
        for(int i = 0; i < m_users.size(); ++i)
        {
            if(m_users[i]->id() == userId)
            {
                m_users[i]->setIsCameraOpen(open);

                emit dataChanged(
                    index(i),
                    index(i),
                    { IsCameraOpenRole });

                return;
            }
        }
    }

    void setDeafened(
        quint64 userId,
        bool deafened)
    {
        for(int i = 0; i < m_users.size(); ++i)
        {
            if(m_users[i]->id() == userId)
            {
                m_users[i]->setIsDeafened(deafened);

                emit dataChanged(
                    index(i),
                    index(i),
                    { IsDeafened });

                return;
            }
        }
    }

    Participant *findUser(quint64 userId)
    {
        for(auto user : m_users)
        {
            if(user->id()==userId)
                return user;
        }

        return nullptr;
    }

private:
    QList<Participant*> m_users;
};
