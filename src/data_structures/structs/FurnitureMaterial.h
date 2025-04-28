#ifndef FURNITUREMATERIAL_H
#define FURNITUREMATERIAL_H

#include <QString>

struct FurnitureMaterial {
    QString name;
    float width;
    float height;
    float thickness;
    QString grainDirection;
    float wasteFactor;

    FurnitureMaterial() = default;
    FurnitureMaterial(const QString &n, float w, float h, float t, const QString &g, float waste)
        : name(n), width(w), height(h), thickness(t), grainDirection(g), wasteFactor(waste) {}
};


#endif // FURNITUREMATERIAL_H
