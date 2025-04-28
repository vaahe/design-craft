// closetgenerator.h
#pragma once
#include <QVector3D>
#include <QColor>
#include <vector>

class ClosetGenerator {
public:
    struct ClosetPart {
        QString name;
        QVector3D dimensions;
        int quantity;
    };

    static std::vector<ClosetPart> generateClosetParts(float width, float height, float depth,
                                                       int shelfCount, float shelfSpacing);

    static std::vector<float> calculateShelfPositions(float height, int shelfCount, float spacing);
};
