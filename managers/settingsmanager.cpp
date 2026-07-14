#include "settingsmanager.h"

#include "logging/loggingcategories.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent),
    m_settings(APP_ORGANIZATION, APP_NAME)
{
}

QVariant SettingsManager::value(const QString &key,
                                const QVariant &defaultValue)
{
    // qCDebug(_settings) << "get " << key << " defaultVal=" << defaultValue;
    return m_settings.value(key, defaultValue);
}

void SettingsManager::setValue(const QString &key,
                               const QVariant &value)
{
    // qCDebug(_settings) << "set " << key << " to " << value;
    m_settings.setValue(key, value);
    m_settings.sync();
}

bool SettingsManager::contains(const QString &key)
{
    return m_settings.contains(key);
}
