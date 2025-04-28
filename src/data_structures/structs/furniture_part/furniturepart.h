#ifndef FURNITUREPART_H
#define FURNITUREPART_H

#include <QString>

struct FurniturePart {
    QString name;
    float width;   // meters
    float height;  // meters
    float thickness;
    float depth;
    float x;
    float y;
    float z;
};

#endif // FURNITUREPART_H
