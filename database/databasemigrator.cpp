#include "databasemigrator.h"

#include "database.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseMigrator::DatabaseMigrator(Database *database) : m_database(database)
{

}

bool DatabaseMigrator::migrate()
{
    // Old debug databases
    if (!metadataTableExists())
    {
        qDebug() << "Legacy database detected.";

        QSqlDatabase db = m_database->database();

        if (!db.transaction())
            return false;

        QSqlQuery query(db);

        if(!query.exec("DROP TABLE IF EXISTS MyServers"))
        {
            db.rollback();
            return false;
        }

        if(!query.exec("DROP TABLE IF EXISTS UserRelations"))
        {
            db.rollback();
            return false;
        }


        if (!createMetadataTable())
        {
            db.rollback();
            return false;
        }

        if (!db.commit())
        {
            qDebug() << db.lastError();
            return false;
        }
    }


    int version = schemaVersion();

    qDebug() << "Current schema version:" << version;

    while (true)
    {
        switch (version)
        {
            case 0:
            {
                if (!migrate0to1())
                    return false;

                version = 1;
                break;
            }

            default:
            {
                qDebug() << "Database schema is up to date.";
                return true;
            }
        }
    }
}

bool DatabaseMigrator::createMetadataTable()
{
    QSqlQuery query(m_database->database());

    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS Metadata
        (
            key TEXT PRIMARY KEY,
            value TEXT NOT NULL
        )
    )"))
    {
        qDebug() << query.lastError();
        return false;
    }

    return true;
}

int DatabaseMigrator::schemaVersion()
{
    QSqlQuery query(m_database->database());

    query.prepare(
        "SELECT value "
        "FROM Metadata "
        "WHERE key='schemaVersion'");

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return 0;
    }

    if (!query.next())
        return 0;

    return query.value(0).toInt();
}

bool DatabaseMigrator::setSchemaVersion(int version)
{
    QSqlQuery query(m_database->database());

    query.prepare(R"(
        INSERT INTO Metadata
        (
            key,
            value
        )
        VALUES
        (
            'schemaVersion',
            ?
        )
        ON CONFLICT(key)
        DO UPDATE SET
            value=excluded.value
    )");

    query.addBindValue(version);

    if (!query.exec())
    {
        qDebug() << query.lastError();
        return false;
    }

    return true;
}

bool DatabaseMigrator::metadataTableExists()
{
    QSqlQuery query(m_database->database());

    query.prepare(R"(
        SELECT name
        FROM sqlite_master
        WHERE type='table'
        AND name='Metadata'
    )");

    if (!query.exec())
        return false;

    return query.next();
}

bool DatabaseMigrator::migrate0to1()
{
    qDebug() << "Migrating schema 0 -> 1";

    QSqlDatabase db = m_database->database();

    if(!db.transaction())
        return false;

    QSqlQuery query(db);

    //setup database tables
    if(!query.exec(R"(
        CREATE TABLE IF NOT EXISTS MyServers
        (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            avatarPath TEXT,
            ip TEXT,
            port TEXT
        )
    )"))
    {
        qDebug() << query.lastError();

        db.rollback();

        return false;
    }


    //create relations
    if(!query.exec(R"(
        CREATE TABLE IF NOT EXISTS UserRelations
        (
            identity TEXT PRIMARY KEY,

            nickname TEXT,
            note TEXT,

            relationship INTEGER NOT NULL DEFAULT 0,

            muted INTEGER NOT NULL DEFAULT 0,
            voiceVolume INTEGER NOT NULL DEFAULT 100,
            createdAt DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )"))
    {
        qDebug() << query.lastError();

        db.rollback();

        return false;
    }


    if (!setSchemaVersion(1))
    {
        db.rollback();

        return false;
    }

    if (!db.commit())
    {
        qDebug() << "Failed to commit transaction:" << db.lastError();
        return false;
    }

    qDebug() << "Database upgraded to schema version 1";

    return true;
}
