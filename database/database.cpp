#include "database.h"

Database::Database(QObject *parent)
    : QObject(parent)
{
    open();
}

Database::~Database()
{
    if (m_db.isOpen())
        m_db.close();
}

bool Database::open()
{
    QString location =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation);

    QDir().mkpath(location);

    QString dbPath = location + DATABASE_FILENAME;

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open())
    {
        qDebug() << m_db.lastError();
        return false;
    }

    qDebug() << "Database:" << dbPath;

    return true;
}

bool Database::exec(const QString &sql)
{
    QSqlQuery query;

    if (!query.exec(sql))
    {
        qDebug() << query.lastError();
        return false;
    }

    return true;
}

bool Database::createTable(const QString &sql)
{
    return exec(sql);
}

bool Database::insert(const QString &table,
                      const QVariantMap &values)
{
    QStringList fields;
    QStringList binds;

    for (auto it = values.begin(); it != values.end(); ++it)
    {
        fields << it.key();
        binds << ":" + it.key();
    }

    QString sql =
        QString("INSERT INTO %1 (%2) VALUES (%3)")
            .arg(table)
            .arg(fields.join(","))
            .arg(binds.join(","));

    QSqlQuery query;

    query.prepare(sql);

    for (auto it = values.begin(); it != values.end(); ++it)
        query.bindValue(":" + it.key(), it.value());

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return false;
    }

    return true;
}

bool Database::update(const QString &table,
                      int id,
                      const QVariantMap &values)
{
    QStringList sets;

    for (auto it = values.begin(); it != values.end(); ++it)
        sets << QString("%1=:%1").arg(it.key());

    QString sql =
        QString("UPDATE %1 SET %2 WHERE id=:id")
            .arg(table)
            .arg(sets.join(","));

    QSqlQuery query;

    query.prepare(sql);

    for (auto it = values.begin(); it != values.end(); ++it)
        query.bindValue(":" + it.key(), it.value());

    query.bindValue(":id", id);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return false;
    }

    return true;
}

bool Database::remove(const QString &table,
                      int id)
{
    QSqlQuery query;

    query.prepare(
        QString("DELETE FROM %1 WHERE id=?").arg(table));

    query.addBindValue(id);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return false;
    }

    return true;
}

QVariantMap Database::getById(const QString &table,
                              int id)
{
    QVariantMap row;

    QSqlQuery query;

    query.prepare(
        QString("SELECT * FROM %1 WHERE id=?").arg(table));

    query.addBindValue(id);

    if (!query.exec())
        return row;

    if (!query.next())
        return row;

    QSqlRecord record = query.record();

    for (int i = 0; i < record.count(); ++i)
        row[record.fieldName(i)] = query.value(i);

    return row;
}

QVariantMap Database::getServer(const QString &ip, const QString &port)
{
    QVariantMap row;

    QSqlQuery query;

    query.prepare(
        "SELECT * FROM MyServers "
        "WHERE ip = ? AND port = ?");

    query.addBindValue(ip);
    query.addBindValue(port);

    if (!query.exec())
        return row;

    if (!query.next())
        return row;   // Empty map = not found

    QSqlRecord record = query.record();

    for (int i = 0; i < record.count(); ++i)
        row[record.fieldName(i)] = query.value(i);

    return row;
}

qint64 Database::getServerId(const QString &ip, int port)
{
    QSqlQuery query;

    query.prepare(
        "SELECT id FROM MyServers "
        "WHERE ip = ? AND port = ?");

    query.addBindValue(ip);
    query.addBindValue(port);

    if (!query.exec())
        return -1;

    if (!query.next())
        return -1;    // Not found

    return query.value(0).toLongLong();
}

QVariantList Database::getAll(const QString &table)
{
    return query(QString("SELECT * FROM %1").arg(table));
}

QVariantList Database::query(const QString &sql)
{
    QVariantList rows;

    QSqlQuery query;

    if (!query.exec(sql))
    {
        qDebug() << query.lastError();
        return rows;
    }

    while (query.next())
    {
        QVariantMap row;

        QSqlRecord record = query.record();

        for (int i = 0; i < record.count(); ++i)
            row[record.fieldName(i)] = query.value(i);

        rows.append(row);
    }

    return rows;
}
