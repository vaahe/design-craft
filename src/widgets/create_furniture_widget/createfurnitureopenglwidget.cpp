#include "createfurnitureopenglwidget.h"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>


CreateFurnitureOpenGLWidget::CreateFurnitureOpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent), furnitureCategory("Chair"),
    furnitureColor(Qt::white), furnitureWidth(50),
    furnitureHeight(50), furnitureDepth(50) {}

void CreateFurnitureOpenGLWidget::setFurnitureAttributes(const QString &category, const QColor &color,
                                                         int width, int height, int depth) {
    furnitureCategory = category;
    furnitureColor = color;
    furnitureWidth = width;
    furnitureHeight = height;
    furnitureDepth = depth;

    update(); // Request a redraw
}

void CreateFurnitureOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
}

void CreateFurnitureOpenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    QMatrix4x4 projection;
    projection.perspective(45.0, static_cast<double>(w) / h, 0.1, 100.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(projection.constData());  // Load Qt's matrix into OpenGL

    glMatrixMode(GL_MODELVIEW);
}

void CreateFurnitureOpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -5.0f); // Move the object back

    // Set color
    glColor3f(furnitureColor.redF(), furnitureColor.greenF(), furnitureColor.blueF());

    glPushMatrix();
    glScalef(furnitureWidth / 50.0f, furnitureHeight / 50.0f, furnitureDepth / 50.0f);

    // Render a basic cube as a placeholder
    glBegin(GL_QUADS);

    // Front Face
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);

    // Back Face
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    // Left Face
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);

    // Right Face
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);

    // Top Face
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);

    // Bottom Face
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glEnd();
    glPopMatrix();
}
