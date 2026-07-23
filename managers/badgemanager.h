#pragma once

#include <QObject>
#include <QHash>

class BadgeManager : public QObject
{
    Q_OBJECT

public:
    explicit BadgeManager(QObject *parent = nullptr);

    enum class Badge
    {
        None      = 0,

        Developer = 1 << 0, // 1
        Donator   = 1 << 1, // 2
        Debugger    = 1 << 2  // 4

        /*
             | User has...                  | Value |
            | ---------------------------- | ----: |
            | None                         |     0 |
            | Developer                    |     1 |
            | Donator                      |     2 |
            | Developer + Donator          |     3 |
            | Debugger                       |     4 |
            | Developer + Debugger           |     5 |
            | Donator + Debugger             |     6 |
            | Developer + Donator + Debugger |     7 |
        */
    };
    Q_ENUM(Badge)

    void clear();

    bool load(const QByteArray &json);

    void setBadges(const QString &identity, quint32 badgeMask);
    void remove(const QString &identity);

    [[nodiscard]]
    quint32 badges(const QString &identity) const;

    [[nodiscard]]
    bool hasBadge(const QString &identity, quint32 badge) const;

    [[nodiscard]]
    bool contains(const QString &identity) const;

private:
    QHash<QString, quint32> m_badges;
};
