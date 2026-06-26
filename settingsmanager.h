#pragma once

#include <QObject>
#include <QSettings>
#include <QVariant>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);

    Q_INVOKABLE QVariant value(const QString &key,
                               const QVariant &defaultValue);

    Q_INVOKABLE void setValue(const QString &key,
                              const QVariant &value);

    Q_INVOKABLE bool contains(const QString &key);

private:
    QSettings m_settings;
};
