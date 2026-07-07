#pragma once

#include <QObject>

#include "../models/identity.h"

class IdentityManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList identityNames
                   READ identityNames
                       NOTIFY identitiesChanged)

    Q_PROPERTY(int currentIdentityIndex
                   READ currentIdentityIndex
                       WRITE setCurrentIdentityIndex
                           NOTIFY currentIdentityChanged)

public:
    explicit IdentityManager(QObject *parent = nullptr);

    bool load();
    bool save();

    Q_INVOKABLE bool createIdentity(const QString& name);

    bool removeIdentity(const QString& name);

    bool renameIdentity(const QString& oldName,const QString& newName);

    bool setCurrentIdentity(const QString& name);

    Identity* currentIdentity();

    Identity* findIdentity(const QString& name);

    const QList<Identity>& identities() const;


    QStringList identityNames() const;
    int currentIdentityIndex() const;
    Q_INVOKABLE void setCurrentIdentityIndex(int index);
    Q_INVOKABLE void removeCurrentIdentity();
    Q_INVOKABLE void renameCurrentIdentity(const QString& newName);

signals:
    void identitiesChanged();
    void currentIdentityChanged();

    void currentIdentityChangedTo(const QString& name);
private:
    QString identityFile() const;

private:
    QList<Identity> m_identities;

    QString m_currentIdentityName;
};
