#ifndef CREATEFURNITUREOPENGLWIDGET_H
#define CREATEFURNITUREOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QColor>
#include <QVector>
#include <glm/glm.hpp>

class CreateFurnitureOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit CreateFurnitureOpenGLWidget(QWidget *parent = nullptr);
    void setFurnitureDimensions(float width, float height, float depth,
                                const QColor& color, int shelfCount, float shelfSpacing);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void drawMainBox();
    void drawShelves();
    void drawDoors();
    void drawGridFloor();

    // Furniture properties
    float width = 2.0f;
    float height = 2.2f;
    float depth = 0.6f;
    QColor color = QColor(139, 69, 19);
    QVector<float> shelfPositions;

    // View properties
    glm::mat4 projection;
    glm::mat4 view;
    float xRot = -30.0f;
    float yRot = -45.0f;
    QPoint lastMousePos;
};

#endif // CREATEFURNITUREOPENGLWIDGET_H
