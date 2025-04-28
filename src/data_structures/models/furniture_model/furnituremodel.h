#ifndef FURNITUREMODEL_H
#define FURNITUREMODEL_H

#include <QVector3D>
#include <QColor>
#include <vector>

class FurnitureModel {
public:
    struct Part {
        QVector3D position;
        QVector3D dimensions;
        QColor color;
        QString name;
    };

    void addPart(const QVector3D& position, const QVector3D& dimensions,
                 const QColor& color, const QString& name = "");
    const std::vector<Part>& getParts() const;
    void clear();

private:
    std::vector<Part> parts;
};

#endif // FURNITUREMODEL_H
