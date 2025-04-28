#ifndef FURNITURE_SPEC_H
#define FURNITURE_SPEC_H

#include <QString>
#include <QColor>
#include <QJsonObject>

struct FurnitureSpec {
    QString name;
    QString type;
    float width = 0.0f;
    float height = 0.0f;
    float depth = 0.0f;
    int shelves = 0;
    QString material;
    QColor color = Qt::white;

    float thickness = 0.018f;
    bool hasDoors = true;
    bool hasDrawers = false;

    QJsonObject toJson() const;
    bool fromJson(const QJsonObject& json);

    bool isValid() const;

    static FurnitureSpec defaultClosetSpec();
    static FurnitureSpec defaultTableSpec();
};

#endif // FURNITURE_SPEC_H
