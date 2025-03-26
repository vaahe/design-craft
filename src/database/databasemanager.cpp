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
