#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>


class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent =nullptr);
    ~Database();

    bool open();

    // Execute raw SQL
    bool exec(const QString &sql);

    // Create table
    bool createTable(const QString &sql);

    // Insert one row
    bool insert(const QString &table,
                const QVariantMap &values);

    // Update row
    bool update(const QString &table,
                int id,
                const QVariantMap &values);

    // Delete row
    bool remove(const QString &table,
                int id);

    // Get one row
    QVariantMap getById(const QString &table,
                        int id);

    QVariantMap getServer(const QString &ip, const QString& port);

    qint64 getServerId(const QString &ip, int port);

    // Get every row
    QVariantList getAll(const QString &table);

    // Generic query
    QVariantList query(const QString &sql);

private:
    QSqlDatabase m_db;
};

#endif
