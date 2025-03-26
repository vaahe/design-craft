#ifndef FURNITURE_H
#define FURNITURE_H

#include <QString>

struct Furniture {
    float x, y, z;  // Position
    float width, height, depth;  // Size
    float rotation;  // Rotation angle
    QString type;  // Type of furniture ("table", "chair")
};

#endif // FURNITURE_H
