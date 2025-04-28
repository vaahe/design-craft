#ifndef FURNITURE_ITEM_H
#define FURNITURE_ITEM_H

#include <QString>
#include <QColor>
#include <QDateTime>

struct FurnitureItem {
    int id = -1;
    QString name;
    float width = 0.0f;
    float height = 0.0f;
    float depth = 0.0f;
    QColor color = Qt::white;
    QString material;
    int categoryId = -1;
    QDateTime createdAt;
};

#endif // FURNITURE_ITEM_H
