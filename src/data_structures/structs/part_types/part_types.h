#ifndef PART_TYPES_H
#define PART_TYPES_H

#include <QString>

struct Part {
    QString name;
    float width;
    float height;

    Part(QString n = "", float w = 0, float h = 0) : name(n), width(w), height(h) {}
};

#endif // PART_TYPES_H
