#ifndef DOOR_H
#define DOOR_H

#include <QColor>
#include <QVector3D>

class Door {
public:
    QVector3D position;
    float width = 1.0f;
    float height = 2.2f;
    float thickness = 0.1f;
    QColor color = QColor(160, 82, 45);
    bool isSelected = false;

    void draw();
    bool containsPoint(const QVector3D& point) const;
};

#endif // DOOR_H
