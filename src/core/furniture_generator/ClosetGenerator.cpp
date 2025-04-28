#include "closetgenerator.h"

std::vector<ClosetGenerator::ClosetPart> ClosetGenerator::generateClosetParts(
    float width, float height, float depth, int shelfCount, float shelfSpacing)
{
    std::vector<ClosetPart> parts;

    // parts.push_back({"Back Panel", {width, height, 0.02f}, 1});
    parts.push_back({"Left Side", {0.02f, height, depth}, 1});
    parts.push_back({"Right Side", {0.02f, height, depth}, 1});
    parts.push_back({"Top", {width, 0.02f, depth}, 1});
    parts.push_back({"Bottom", {width, 0.02f, depth}, 1});
    parts.push_back({"Door", {width/2, height, 0.02f}, 2});

    for (int i = 0; i < shelfCount; ++i) {
        parts.push_back({QString("Shelf %1").arg(i+1), {width*0.98f, 0.02f, depth*0.98f}, 1});
    }

    return parts;
}

std::vector<float> ClosetGenerator::calculateShelfPositions(float height, int shelfCount, float spacing)
{
    std::vector<float> positions;
    float bottomMargin = 0.1f; // 10cm from bottom
    float topMargin = 0.4f;    // 40cm at top

    float availableHeight = height - bottomMargin - topMargin;
    float actualSpacing = (availableHeight - (shelfCount * 0.02f)) / (shelfCount + 1);

    for (int i = 0; i < shelfCount; ++i) {
        float y = bottomMargin + (i+1)*actualSpacing + i*0.02f;
        positions.push_back(y - height/2);
    }

    return positions;
}
