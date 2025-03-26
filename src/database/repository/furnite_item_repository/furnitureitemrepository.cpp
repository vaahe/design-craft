// // postgresfurniturecategoryrepository.h
// #ifndef POSTGRESFURNITURECATEGORYREPOSITORY_H
// #define POSTGRESFURNITURECATEGORYREPOSITORY_H

// #include "furnitureitemrepository.h"
// #include <QtSql/QSqlDatabase>

// class PostgresFurnitureCategoryRepository : public FurnitureCategoryRepository
// {
// public:
//     PostgresFurnitureCategoryRepository(QSqlDatabase& database);
//     ~PostgresFurnitureCategoryRepository();

//     // CRUD operations for categories
//     bool addCategory(const QString& name, const QString& description = "") override;
//     QList<QString> getAllCategories() override;
//     bool updateCategoryDescription(int categoryId, const QString& newDescription) override;
//     bool deleteCategory(int categoryId) override;

// private:
//     QSqlDatabase& m_database;
// };

// #endif // POSTGRESFURNITURECATEGORYREPOSITORY_H
