#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    roomWidth(10),
    roomHeight(3),
    roomDepth(10),
    m_gridEnabled(true)
{
    furnitureList.push_back({0.0f, -1.0f, 2.0f, 2.0f, 1.0f, 2.0f, 0.0f, "table"});
}

void OpenGLWidget::setRoomSize(int width, int height, int depth)
{
    roomWidth = width;
    roomHeight = height;
    roomDepth = depth;
    update();
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void OpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 projection;
    projection.perspective(m_zoom, float(width()) / float(height()), 0.1f, 100.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(projection.constData()); // Use Qt's matrix

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -20.0f);
    glRotatef(m_pitch, 1.0f, 0.0f, 0.0f); // Rotate up/down
    glRotatef(m_yaw, 0.0f, 1.0f, 0.0f);   // Rotate left/right

    renderRoom();
    renderGrid();
    renderFurnitures();

    /// make floor gray
    glBegin(GL_QUADS);
    glColor3f(floorColor.redF(), floorColor.greenF(), floorColor.blueF());
    glVertex3f(-roomWidth / 2.0f, -roomHeight / 2.0f, -roomDepth / 2.0f);
    glVertex3f(roomWidth / 2.0f, -roomHeight / 2.0f, -roomDepth / 2.0f);
    glVertex3f(roomWidth / 2.0f, -roomHeight / 2.0f, roomDepth / 2.0f);
    glVertex3f(-roomWidth / 2.0f, -roomHeight / 2.0f, roomDepth / 2.0f);
    glEnd();
    ///
}

void OpenGLWidget::zoomIn() {
    if (m_zoom > 10.0f) {
        m_zoom -= 2.0f;
    }

    update();
}

void OpenGLWidget::zoomOut() {
    if (m_zoom < 90.0f) {
        m_zoom += 2.0f;
    }

    update();
}

void OpenGLWidget::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() > 0) {
        zoomIn();
    } else {
        zoomOut();
    }

    emit viewUpdated(m_zoom, m_yaw, m_pitch);
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        m_rightButtonPressed = true;
        m_lastMousePosition = event->pos();
    }
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_rightButtonPressed) {
        int dx = event->pos().x() - m_lastMousePosition.x();
        int dy = event->pos().y() - m_lastMousePosition.y();

        m_yaw += dx * 0.5f;   // Adjust sensitivity
        m_pitch += dy * 0.5f; // Adjust sensitivity

        m_lastMousePosition = event->pos();
        update(); // Redraw the scene

        emit viewUpdated(m_zoom, m_yaw, m_pitch);
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        m_rightButtonPressed = false;
    }
}

void OpenGLWidget::resetView() {
    m_zoom = 45.0f;
    m_yaw = 0.0f;
    m_pitch = 0.0f;

    update();
}

void OpenGLWidget::enableGrid() {
    m_gridEnabled = !m_gridEnabled;
    update();
}


void OpenGLWidget::renderGrid() {
    if (m_gridEnabled) {
        glBegin(GL_LINES);
        glColor3f(0.5f, 0.5f, 0.5f);  // Gray grid

        int gridSize = 20;   // Number of lines
        float gridSpacing = 1.0f;  // Distance between lines
        float gridY = -roomHeight / 2.0f - 0.01f;  // Slightly below the cube

        // Draw vertical lines
        for (int i = -gridSize / 2; i <= gridSize / 2; i++) {
            glVertex3f(i * gridSpacing, gridY, -gridSize * gridSpacing / 2);
            glVertex3f(i * gridSpacing, gridY, gridSize * gridSpacing / 2);
        }

        // Draw horizontal lines
        for (int i = -gridSize / 2; i <= gridSize / 2; i++) {
            glVertex3f(-gridSize * gridSpacing / 2, gridY, i * gridSpacing);
            glVertex3f(gridSize * gridSpacing / 2, gridY, i * gridSpacing);
        }

        glEnd();
    }
}

void OpenGLWidget::renderFurnitures() {
    for (const Furniture &f : furnitureList) {
        glPushMatrix();
        glTranslatef(f.x, f.y, f.z);
        glRotatef(f.rotation, 0.0f, 1.0f, 0.0f);

        if (f.type == "table") {
            glColor3f(0.6f, 0.3f, 0.1f); // Brown color
            glBegin(GL_QUADS);

            // Table Top
            glVertex3f(-f.width / 2, f.height / 2, -f.depth / 2);
            glVertex3f(f.width / 2, f.height / 2, -f.depth / 2);
            glVertex3f(f.width / 2, f.height / 2, f.depth / 2);
            glVertex3f(-f.width / 2, f.height / 2, f.depth / 2);

            glEnd();

            // Table Legs
            glColor3f(0.4f, 0.2f, 0.1f);
            glBegin(GL_QUADS);
            for (float x = -f.width / 2 + 0.1f; x <= f.width / 2 - 0.1f; x += f.width - 0.2f) {
                for (float z = -f.depth / 2 + 0.1f; z <= f.depth / 2 - 0.1f; z += f.depth - 0.2f) {
                    glVertex3f(x, -f.height / 2, z);
                    glVertex3f(x + 0.1f, -f.height / 2, z);
                    glVertex3f(x + 0.1f, f.height / 2, z);
                    glVertex3f(x, f.height / 2, z);
                }
            }
            glEnd();
        }
        else if (f.type == "chair") {
            glColor3f(0.7f, 0.4f, 0.2f); // Light brown
            glBegin(GL_QUADS);

            // Seat
            glVertex3f(-f.width / 2, f.height / 2, -f.depth / 2);
            glVertex3f(f.width / 2, f.height / 2, -f.depth / 2);
            glVertex3f(f.width / 2, f.height / 2, f.depth / 2);
            glVertex3f(-f.width / 2, f.height / 2, f.depth / 2);

            // Chair Back
            glVertex3f(-f.width / 2, f.height, -f.depth / 2);
            glVertex3f(f.width / 2, f.height, -f.depth / 2);
            glVertex3f(f.width / 2, f.height / 2, -f.depth / 2);
            glVertex3f(-f.width / 2, f.height / 2, -f.depth / 2);

            glEnd();
        }

        glPopMatrix();
    }
}

void OpenGLWidget::renderRoom() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_FALSE); // Disable depth writing for transparent objects
    glBegin(GL_QUADS);

    // Set white color for edges
     glColor4f(0.0f, 0.0f, 0.0f, 0.0f);

    // Front wall (transparent)
    glColor4f(frontWallColor.redF(), frontWallColor.greenF(), frontWallColor.blueF(), wallOpacity);
    glVertex3f(-roomWidth / 2.0, -roomHeight / 2.0, roomDepth / 2.0);
    glVertex3f(roomWidth / 2.0, -roomHeight / 2.0, roomDepth / 2.0);
    glVertex3f(roomWidth / 2.0, roomHeight / 2.0, roomDepth / 2.0);
    glVertex3f(-roomWidth / 2.0, roomHeight / 2.0, roomDepth / 2.0);

    // Back wall (transparent)
    glColor4f(backWallColor.redF(), backWallColor.greenF(), backWallColor.blueF(), wallOpacity);
    glVertex3f(-roomWidth / 2.0, -roomHeight / 2.0, -roomDepth / 2.0);
    glVertex3f(roomWidth / 2.0, -roomHeight / 2.0, -roomDepth / 2.0);
    glVertex3f(roomWidth / 2.0, roomHeight / 2.0, -roomDepth / 2.0);
    glVertex3f(-roomWidth / 2.0, roomHeight / 2.0, -roomDepth / 2.0);

    // Left wall (transparent)
    glColor4f(leftWallColor.redF(), leftWallColor.greenF(), leftWallColor.blueF(), wallOpacity);
    glVertex3f(-roomWidth / 2.0, -roomHeight / 2.0, -roomDepth / 2.0);
    glVertex3f(-roomWidth / 2.0, -roomHeight / 2.0, roomDepth / 2.0);
    glVertex3f(-roomWidth / 2.0, roomHeight / 2.0, roomDepth / 2.0);
    glVertex3f(-roomWidth / 2.0, roomHeight / 2.0, -roomDepth / 2.0);

    // Right wall (transparent)
    glColor4f(rightWallColor.redF(), rightWallColor.greenF(), rightWallColor.blueF(), wallOpacity);
    glVertex3f(roomWidth / 2.0, -roomHeight / 2.0, -roomDepth / 2.0);
    glVertex3f(roomWidth / 2.0, -roomHeight / 2.0, roomDepth / 2.0);
    glVertex3f(roomWidth / 2.0, roomHeight / 2.0, roomDepth / 2.0);
    glVertex3f(roomWidth / 2.0, roomHeight / 2.0, -roomDepth / 2.0);

    glEnd();


    // Now, render the separators
    glBegin(GL_LINES);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f); // Set color for the separators (black)

    // Front-left separator (line between front and left wall)
    glVertex3f(-roomWidth / 2.0, -roomHeight / 2.0, roomDepth / 2.0);
    glVertex3f(-roomWidth / 2.0, roomHeight / 2.0, roomDepth / 2.0);

    // Front-right separator (line between front and right wall)
    glVertex3f(roomWidth / 2.0, -roomHeight / 2.0, roomDepth / 2.0);
    glVertex3f(roomWidth / 2.0, roomHeight / 2.0, roomDepth / 2.0);

    // Back-left separator (line between back and left wall)
    glVertex3f(-roomWidth / 2.0, -roomHeight / 2.0, -roomDepth / 2.0);
    glVertex3f(-roomWidth / 2.0, roomHeight / 2.0, -roomDepth / 2.0);

    // Back-right separator (line between back and right wall)
    glVertex3f(roomWidth / 2.0, -roomHeight / 2.0, -roomDepth / 2.0);
    glVertex3f(roomWidth / 2.0, roomHeight / 2.0, -roomDepth / 2.0);

    glEnd();

    renderDoorAndWindows();

    glDepthMask(GL_TRUE); // Re-enable depth writing
    glFlush();
}

void OpenGLWidget::onWallColorChanged(const QColor &color, WallType wallType) {
    switch (wallType) {
    case LEFT:
        leftWallColor = color;
        leftWallColor.setAlpha(255);
        break;
    case RIGHT:
        rightWallColor = color;
        rightWallColor.setAlpha(255);
        break;
    case FRONT:
        frontWallColor = color;
        frontWallColor.setAlpha(255);
        break;
    case BACK:
        backWallColor = color;
        backWallColor.setAlpha(255);
        break;
    default:
        return;
    }

    update();
}

void OpenGLWidget::onWallOpacityChanged(float newOpacityValue) {
    wallOpacity = newOpacityValue;
}


void OpenGLWidget::renderDoorAndWindows() {
    // Front wall window (example)
    renderWindowOrDoor(-roomWidth / 4.0, roomHeight / 4.0, 1.0, 1.0, 0.2);  // Position, width, height, and opacity for a window on front wall
    // Front wall door (example)
    renderWindowOrDoor(0.0, -roomHeight / 4.0, 1.5, 3.0, 0.6);  // Position, width, height, and opacity for a door on front wall

    // You can repeat for other walls, adjusting position and size for doors/windows
}

void OpenGLWidget::renderWindowOrDoor(float x, float y, float width, float height, float opacity) {
    // Draw a window or door using a transparent rectangle
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 1.0f, opacity);  // Set color to blue with desired opacity (adjust as needed)

    glBegin(GL_QUADS);
    glVertex3f(x - width / 2.0, y - height / 2.0, roomDepth / 2.0);  // Bottom-left corner
    glVertex3f(x + width / 2.0, y - height / 2.0, roomDepth / 2.0);  // Bottom-right corner
    glVertex3f(x + width / 2.0, y + height / 2.0, roomDepth / 2.0);  // Top-right corner
    glVertex3f(x - width / 2.0, y + height / 2.0, roomDepth / 2.0);  // Top-left corner
    glEnd();
}
