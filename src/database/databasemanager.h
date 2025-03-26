#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QDebug>
#include <QObject>

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDatabase>

class DatabaseManager {
public:
    static DatabaseManager& getInstance();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

public:
    QString lastError() const;
    QSqlQuery createTable();
    QSqlQuery executeQuery(const QString& query);
    bool connect(const QString& host, int port, const QString& dbName, const QString& user, const QString& password);

private:
    DatabaseManager();
    ~DatabaseManager();

    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
