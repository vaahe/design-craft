#include "databasemanager.h"

DatabaseManager::DatabaseManager() {
    m_db = QSqlDatabase::addDatabase("QPSQL");
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "DB connection is closed";
    }
}

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connect(const QString& host, int port, const QString& dbName, const QString& user, const QString& password) {
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(user);
    m_db.setPassword(password);

    if (!m_db.open()) {
        qDebug() << "Failed to connect to database:" << m_db.lastError().text();
        return false;
    }

    qDebug() << "Connected to PostgreSQL database!";

    if (!createTables()) {
        return false;
    }

    insertDefaultFurnitureCategories();

    return true;
}

QSqlQuery DatabaseManager::executeQuery(const QString& query) {
    QSqlQuery sqlQuery(m_db);
    if (!sqlQuery.exec(query)) {
        qDebug() << "Query failed:" << sqlQuery.lastError().text();
    }

    return sqlQuery;
}

QString DatabaseManager::lastError() const {
    return m_db.lastError().text();
}


bool DatabaseManager::createTables() {
    QSqlQuery query(m_db);

    const QString createCategoriesTableQuery = R"(
        CREATE TABLE IF NOT EXISTS furniture_categories (
            id SERIAL PRIMARY KEY,
            name VARCHAR(100) NOT NULL UNIQUE,
            supports_shelves BOOLEAN DEFAULT FALSE
        );
    )";

    if (!query.exec(createCategoriesTableQuery)) {
        qWarning() << "Failed to create furniture_categories table:" << query.lastError().text();
        return false;
    }

    const QString createItemsTableQuery = R"(
        CREATE TABLE IF NOT EXISTS furniture_items (
            id SERIAL PRIMARY KEY,
            name VARCHAR(150) NOT NULL,
            width FLOAT NOT NULL,
            height FLOAT NOT NULL,
            depth FLOAT NOT NULL,
            color VARCHAR(20) NOT NULL,
            material VARCHAR(100),
            category_id INTEGER NOT NULL REFERENCES furniture_categories(id) ON DELETE CASCADE,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )";

    if (!query.exec(createItemsTableQuery)) {
        qWarning() << "Failed to create furniture_items table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::createFurnitureItem(const FurnitureItem &item) {
    QSqlQuery query(m_db);

    query.prepare(R"(
        INSERT INTO furniture_items
        (name, width, height, depth, color, material, category_id)
        VALUES (:name, :width, :height, :depth, :color, :material, :category_id)
    )");

    query.bindValue(":name", item.name);
    query.bindValue(":width", item.width);
    query.bindValue(":height", item.height);
    query.bindValue(":depth", item.depth);
    query.bindValue(":color", item.color.name(QColor::HexRgb)); // Store color as string
    query.bindValue(":material", item.material);
    query.bindValue(":category_id", item.categoryId);

    if (!query.exec()) {
        qWarning() << "Failed to insert furniture item:" << query.lastError().text();
        return false;
    }

    return true;
}

void DatabaseManager::insertDefaultFurnitureCategories() {
    struct CategoryInfo {
        QString name;
        bool supportsShelves;
    };

    QList<CategoryInfo> defaultCategories = {
        { "Chair", false },
        { "Table", false },
        { "Desk", true },
        { "Wardrobe", true },
        { "Bed", false },
        { "Cabinet", true },
        { "Bookshelf", true }
    };

    QSqlQuery query(m_db);

    for (const CategoryInfo &category : defaultCategories) {
        query.prepare(R"(
            INSERT INTO furniture_categories (name, supports_shelves)
            VALUES (:name, :supports_shelves)
            ON CONFLICT (name) DO UPDATE SET supports_shelves = EXCLUDED.supports_shelves
        )");
        query.bindValue(":name", category.name);
        query.bindValue(":supports_shelves", category.supportsShelves);

        if (!query.exec()) {
            qWarning() << "Failed to insert/update category:" << category.name << query.lastError().text();
        }
    }
}

QStringList DatabaseManager::getFurnitureCategoryField(const QString &field) {
    QStringList results;

    QString queryStr = QString("SELECT %1 FROM furniture_categories").arg(field);
    QSqlQuery query(m_db);

    if (!query.exec(queryStr)) {
        qWarning() << "Failed to get category field:" << query.lastError().text();
        return results;
    }

    while (query.next()) {
        results << query.value(0).toString();
    }

    return results;
}

QList<FurnitureCategory> DatabaseManager::getFurnitureCategories(const QStringList &fields) {
    QList<FurnitureCategory> categories;

    QString selectedFields = fields.isEmpty() ? "id, name, supports_shelves" : fields.join(", ");
    QString queryStr = QString("SELECT %1 FROM furniture_categories").arg(selectedFields);

    QSqlQuery query(m_db);
    if (!query.exec(queryStr)) {
        qWarning() << "Failed to get categories:" << query.lastError().text();
        return categories;
    }

    while (query.next()) {
        FurnitureCategory category;

        if (fields.isEmpty() || fields.contains("id"))
            category.id = query.value("id").toInt();
        if (fields.isEmpty() || fields.contains("name"))
            category.name = query.value("name").toString();
        if (fields.isEmpty() || fields.contains("supports_shelves"))
            category.supportsShelves = query.value("supports_shelves").toBool();

        categories.append(category);
    }

    return categories;
}
