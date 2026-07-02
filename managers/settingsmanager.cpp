#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent),
    m_settings(APP_ORGANIZATION, APP_NAME)
{
}

QVariant SettingsManager::value(const QString &key,
                                const QVariant &defaultValue)
{
    return m_settings.value(key, defaultValue);
}

void SettingsManager::setValue(const QString &key,
                               const QVariant &value)
{
    m_settings.setValue(key, value);
    m_settings.sync();
}

bool SettingsManager::contains(const QString &key)
{
    return m_settings.contains(key);
}
