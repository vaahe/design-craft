#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "src/database/interfaces/furniture_item/furniture_item.h"
#include "src/database/interfaces/furniture_category/furniture_category.h"

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
    bool createTables();
    bool createFurnitureItem();
    bool createFurnitureItem(const FurnitureItem &item);
    void insertDefaultFurnitureCategories();
    QStringList getFurnitureCategoryField(const QString &fields);
    QList<FurnitureCategory> getFurnitureCategories(const QStringList &fields);

private:
    DatabaseManager();
    ~DatabaseManager();

    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
