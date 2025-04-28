#ifndef FURNITURE_H
#define FURNITURE_H

#include <QString>

struct Furniture {
    float x, y, z;
    float width, height, depth;
    float rotation;
    QString type;
};

#endif // FURNITURE_H
