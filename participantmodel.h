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
        IsCameraOpenRole
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
            {VideoSinkRole,"videoSink"}
        };
    }

    Q_INVOKABLE void addUser(const QString &name, bool isTalking=false, bool isCameraOpen=false)
    {
        beginInsertRows(QModelIndex(), m_users.size(), m_users.size());

        m_users.append(new Participant(name,isTalking,isCameraOpen,this));

        endInsertRows();
    }

    Q_INVOKABLE void removeUser(const QString &name)
    {
        for(int i=0;i<m_users.size();i++)
        {
            if(m_users[i]->username()==name)
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

    Participant *findUser(const QString &name)
    {
        for(auto user : m_users)
        {
            if(user->username()==name)
                return user;
        }

        return nullptr;
    }

private:
    QList<Participant*> m_users;
};
