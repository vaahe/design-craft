#include "createfurnitureopenglwidget.h"
#include <QMouseEvent>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

CreateFurnitureOpenGLWidget::CreateFurnitureOpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setMinimumSize(300, 300);
}

void CreateFurnitureOpenGLWidget::setFurnitureDimensions(float w, float h, float d, const QColor& c, int shelfCount, float shelfSpacing)
{
    width = w;
    height = h;
    depth = d;
    color = c;

    shelfPositions.clear();
    float currentHeight = shelfSpacing;
    for (int i = 0; i < shelfCount && currentHeight < height; ++i) {
        shelfPositions.append(currentHeight - height/2);
        currentHeight += shelfSpacing;
    }

    update();
}

void CreateFurnitureOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
}

void CreateFurnitureOpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(w)/h,
        0.1f,
        100.0f
        );

    view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, height * 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
        );
}

void CreateFurnitureOpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(xRot), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(yRot), glm::vec3(0.0f, 1.0f, 0.0f));

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(view * model));

    drawGridFloor();
    drawMainBox();
    drawShelves();
    drawDoors();
}

void CreateFurnitureOpenGLWidget::drawGridFloor()
{
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_LINES);

    float gridSize = qMax(width, depth) * 1.5f;
    float step = 0.5f;

    for (float x = -gridSize; x <= gridSize; x += step) {
        glVertex3f(x, -height/2, -gridSize);
        glVertex3f(x, -height/2, gridSize);
    }

    for (float z = -gridSize; z <= gridSize; z += step) {
        glVertex3f(-gridSize, -height/2, z);
        glVertex3f(gridSize, -height/2, z);
    }

    glEnd();
}

void CreateFurnitureOpenGLWidget::drawMainBox()
{
    glPushMatrix();
    glColor3f(color.redF(), color.greenF(), color.blueF());

    // Draw main cabinet
    glBegin(GL_QUADS);
    // Front face
    glVertex3f(-width/2, -height/2, depth/2);
    glVertex3f(width/2, -height/2, depth/2);
    glVertex3f(width/2, height/2, depth/2);
    glVertex3f(-width/2, height/2, depth/2);

    // Back face
    glVertex3f(-width/2, -height/2, -depth/2);
    glVertex3f(-width/2, height/2, -depth/2);
    glVertex3f(width/2, height/2, -depth/2);
    glVertex3f(width/2, -height/2, -depth/2);

    // Left side
    glVertex3f(-width/2, -height/2, -depth/2);
    glVertex3f(-width/2, -height/2, depth/2);
    glVertex3f(-width/2, height/2, depth/2);
    glVertex3f(-width/2, height/2, -depth/2);

    // Right side
    glVertex3f(width/2, -height/2, -depth/2);
    glVertex3f(width/2, height/2, -depth/2);
    glVertex3f(width/2, height/2, depth/2);
    glVertex3f(width/2, -height/2, depth/2);

    // Top
    glVertex3f(-width/2, height/2, -depth/2);
    glVertex3f(-width/2, height/2, depth/2);
    glVertex3f(width/2, height/2, depth/2);
    glVertex3f(width/2, height/2, -depth/2);

    glEnd();
    glPopMatrix();
}

void CreateFurnitureOpenGLWidget::drawShelves()
{
    if (shelfPositions.isEmpty()) return;

    QColor shelfColor(160, 82, 45);
    glColor3f(shelfColor.redF(), shelfColor.greenF(), shelfColor.blueF());

    for (float yPos : shelfPositions) {
        glPushMatrix();
        glTranslatef(0.0f, yPos, 0.0f);
        glScalef(width*0.95f, 0.02f, depth*0.95f);

        glBegin(GL_QUADS);
        // Top face
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);

        // Front edge
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glEnd();

        glPopMatrix();
    }
}

void CreateFurnitureOpenGLWidget::drawDoors()
{
    if (depth < 0.4f) return;

    QColor doorColor(color.redF()*0.8f, color.greenF()*0.8f, color.blueF()*0.8f);
    glColor3f(doorColor.redF(), doorColor.greenF(), doorColor.blueF());

    // Left door
    glPushMatrix();
    glTranslatef(-width/4, 0.0f, depth/2 + 0.01f);
    glScalef(width/2, height, 0.02f);

    glBegin(GL_QUADS);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glEnd();

    glPopMatrix();

    // Right door
    glPushMatrix();
    glTranslatef(width/4, 0.0f, depth/2 + 0.01f);
    glScalef(width/2, height, 0.02f);

    glBegin(GL_QUADS);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glEnd();

    glPopMatrix();
}

void CreateFurnitureOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
}

void CreateFurnitureOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastMousePos.x();
    int dy = event->y() - lastMousePos.y();

    if (event->buttons() & Qt::LeftButton) {
        yRot += dx * 0.5f;
        xRot += dy * 0.5f;
        xRot = qBound(-90.0f, xRot, 90.0f);
        update();
    }

    lastMousePos = event->pos();
}
