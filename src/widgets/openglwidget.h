#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include "src/data_structures/Furniture.h"
#include "src/data_structures/enums/WallsEnum.h"

#include <QMatrix4x4>
#include <QWheelEvent>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    void setRoomSize(int width, int height, int depth);

    void zoomIn();
    void zoomOut();
    void resetView();
    void enableGrid();

    void renderGrid();
    void renderRoom();
    void renderFurnitures();

    void renderDoorAndWindows();
    void renderWindowOrDoor(float x, float y, float width, float height, float opacity);

    // void setFloorMaterial(const QColor &color);
    // void setWallColor(const QColor &color, WallType wallType);
    void onWallColorChanged(const QColor &color, WallType wallType);
    void onWallOpacityChanged(float newOpacityValue);

signals:
    void viewUpdated(float zoomFactor, float yaw, float pitch);

protected:
    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int roomWidth;
    int roomHeight;
    int roomDepth;

    float m_zoom = 45.0f;
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    bool m_rightButtonPressed = false;
    bool m_gridEnabled;
    QPoint m_lastMousePosition;

    QColor floorColor = QColor(128, 128, 128);
    QColor wallColor = QColor(255, 255, 255);

    std::vector<Furniture> furnitureList;

    QColor leftWallColor = QColor(255, 255, 255, 255);
    QColor rightWallColor = QColor(255, 255, 255, 0);
    QColor frontWallColor = QColor(255, 255, 255, 0);
    QColor backWallColor = QColor(255, 255, 255, 255);
    float wallOpacity = 0.5f;
};

#endif // OPENGLWIDGET_H
