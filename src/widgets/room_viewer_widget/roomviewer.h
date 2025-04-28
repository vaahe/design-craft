#ifndef ROOMVIEWER_H
#define ROOMVIEWER_H

#include "src/data_structures/door/door.h"
#include "src/widgets/material_optimizer_widget/materialoptimizerwidget.h"

#include <glm/glm.hpp>
#include <QOpenGLWidget>

#include <QVector3D>
#include <QColor>
#include <vector>

class RoomViewer : public QOpenGLWidget {
    Q_OBJECT
public:
    struct Wall {
        QVector3D start;
        QVector3D end;
        float height;
        float thickness;
        QColor color;
        int id;
        float opacity = 1.0f;
    };

    struct Furniture {
        QVector3D position;
        QVector3D dimensions;
        QColor color;
        QString name;
        std::vector<float> shelfPositions;
    };

    explicit RoomViewer(QWidget *parent = nullptr);
    void addFurniture(const QVector3D& position, const QVector3D& dimensions, const QColor& color, const QString& name, const std::vector<float>& shelves = {});
    const std::vector<Furniture>& getFurniturePieces() const;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void drawRoom();
    void drawFurniture(const Furniture& furniture);
    void drawGrid();
    void drawDoors(const QVector3D& dim, const QColor& color);
    void drawCuboid(const QVector3D& dimensions, const QColor& color);

    // drag and drop <<start>>
    std::vector<Furniture> furniturePieces;
    QVector3D roomSize = QVector3D(10, 3, 10); // W,H,D in meters
    int selectedFurnitureIndex = -1;
    QVector3D dragOffset;
    bool isDragging = false;
    int selectFurnitureAtPosition(const QPoint& pos);
    QVector3D screenToWorld(const QPoint& screenPos, float y = 0) const;
    bool rayIntersectsAABB(const QVector3D& rayStart, const QVector3D& rayEnd, const QVector3D& boxMin, const QVector3D& boxMax);
    void drawWireframeBox(const QVector3D& dim);
    // drag and drop <<end>>

    // Modern GL matrices
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 modelMatrix;

    QVector3D velocity;
    QVector3D targetPosition;
    void updatePosition();

    // View controls
    float cameraDistance = 15.0f;
    float xRot = -30.0f;
    float yRot = -45.0f;
    QPoint lastMousePos;

    void drawQuad(const QVector3D& p1, const QVector3D& p2, const QVector3D& p3, const QVector3D& p4, const QColor& color);
    void drawCube(const QVector3D& center, const QVector3D& size, const QColor& color);
    std::vector<Wall> walls;
    void drawWall(const Wall& wall);
    void addWall(const QVector3D& start, const QVector3D& end, float height, float thickness, const QColor& color, int id);

    void updateWallOpacities();

public:
    void showCuttingOptimizer();

signals:
    void optimizeFurniture(const QList<QPair<float, float>>& parts);

private:
    MaterialOptimizerWidget *optimizer = nullptr;
    QPushButton *optimizeButton = nullptr;

private:
    float fadeStartAngle = 0.7f;
    float fadeEndAngle = 0.95f;
    QVector3D lastCameraForward;
};

#endif // ROOMVIEWER_H
