#include "furniturespec.h"
#include <QJsonObject>

QJsonObject FurnitureSpec::toJson() const {
    QJsonObject json;
    json["name"] = name;
    json["type"] = type;
    json["width"] = width;
    json["height"] = height;
    json["depth"] = depth;
    json["shelves"] = shelves;
    json["material"] = material;
    json["color"] = color.name();
    json["thickness"] = thickness;
    json["hasDoors"] = hasDoors;
    json["hasDrawers"] = hasDrawers;
    return json;
}

bool FurnitureSpec::fromJson(const QJsonObject& json) {
    if (!json.contains("type") || !json.contains("width")) {
        return false;
    }

    name = json["name"].toString();
    type = json["type"].toString();
    width = json["width"].toDouble();
    height = json["height"].toDouble();
    depth = json["depth"].toDouble();
    shelves = json["shelves"].toInt();
    material = json["material"].toString();
    color = QColor(json["color"].toString());
    thickness = json["thickness"].toDouble(0.018f);
    hasDoors = json["hasDoors"].toBool(true);
    hasDrawers = json["hasDrawers"].toBool(false);

    return true;
}

bool FurnitureSpec::isValid() const {
    return width > 0 && height > 0 && depth > 0 && !type.isEmpty();
}

FurnitureSpec FurnitureSpec::defaultClosetSpec() {
    FurnitureSpec spec;
    spec.type = "Closet";
    spec.name = "Closet";
    spec.width = 2.0f;
    spec.height = 2.4f;
    spec.depth = 0.6f;
    spec.shelves = 3;
    spec.material = "Plywood";
    spec.color = QColor(139, 69, 19);
    spec.hasDoors = true;
    return spec;
}

FurnitureSpec FurnitureSpec::defaultTableSpec() {
    FurnitureSpec spec;
    spec.type = "Table";
    spec.name = "Table";
    spec.width = 1.5f;
    spec.height = 0.75f;
    spec.depth = 0.9f;
    spec.material = "Solid Wood";
    spec.color = QColor(160, 82, 45);
    return spec;
}
