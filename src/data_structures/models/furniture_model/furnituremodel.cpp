#include "furnituremodel.h"

void FurnitureModel::addPart(const QVector3D& position, const QVector3D& dimensions,
                             const QColor& color, const QString& name) {
    parts.push_back({position, dimensions, color, name});
}

const std::vector<FurnitureModel::Part>& FurnitureModel::getParts() const {
    return parts;
}

void FurnitureModel::clear() {
    parts.clear();
}
