#pragma once

class Database;

class DatabaseMigrator
{
public:
    explicit DatabaseMigrator(Database *database);

    bool migrate();

private:
    bool metadataTableExists();
    bool createMetadataTable();

    int schemaVersion();

    bool setSchemaVersion(int version);

    bool migrate0to1();

private:
    Database *m_database = nullptr;
};
