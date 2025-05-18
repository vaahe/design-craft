#include "roomviewer.h"
#include <QMouseEvent>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

RoomViewer::RoomViewer(QWidget *parent) : QOpenGLWidget(parent) {}

void RoomViewer::addFurniture(const QVector3D& position, const QVector3D& dimensions,
                              const QColor& color, const QString& name,
                              const std::vector<float>& shelves)
{
    furniturePieces.push_back({position, dimensions, color, name, shelves});
    update();
}

void RoomViewer::initializeGL()
{
    // initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.9f, 0.9f, 0.95f, 1.0f);
}


void RoomViewer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    projection = glm::perspective(glm::radians(45.0f), static_cast<float>(w)/h, 0.1f, 100.0f);

    view = glm::lookAt(glm::vec3(0.0f, 0.0f, cameraDistance), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}


void RoomViewer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(xRot), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(yRot), glm::vec3(0.0f, 1.0f, 0.0f));

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(view * modelMatrix));

    drawGrid();
    drawRoom();

    for (const auto& furniture : furniturePieces) {
        drawFurniture(furniture);
    }
}

void RoomViewer::drawGrid()
{
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);

    float size = roomSize.x();
    for (float x = -size; x <= size; x += 0.5f) {
        glVertex3f(x, -roomSize.y()/2, -size);
        glVertex3f(x, -roomSize.y()/2, size);
    }

    for (float z = -size; z <= size; z += 0.5f) {
        glVertex3f(-size, -roomSize.y()/2, z);
        glVertex3f(size, -roomSize.y()/2, z);
    }

    glEnd();
}

void RoomViewer::drawRoom()
{
    float floorLevel = -roomSize.y()/2;

    drawQuad(
        QVector3D(-roomSize.x()/2, floorLevel, -roomSize.z()/2),
        QVector3D(roomSize.x()/2, floorLevel, -roomSize.z()/2),
        QVector3D(roomSize.x()/2, floorLevel, roomSize.z()/2),
        QVector3D(-roomSize.x()/2, floorLevel, roomSize.z()/2),
        QColor(200, 200, 200)
    );

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, cameraDistance);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraForward = glm::normalize(cameraTarget - cameraPos);
    lastCameraForward = QVector3D(cameraForward.x, 0, cameraForward.z).normalized();

    walls.clear();
    addWall(QVector3D(-5, floorLevel, -5), QVector3D(5, floorLevel, -5), 3.0f, 0.2f, QColor(220, 220, 220), 1); // Front
    addWall(QVector3D(5, floorLevel, -5), QVector3D(5, floorLevel, 5), 3.0f, 0.2f, QColor(220, 220, 220), 2);    // Right
    addWall(QVector3D(5, floorLevel, 5), QVector3D(-5, floorLevel, 5), 3.0f, 0.2f, QColor(220, 220, 220), 3);    // Back
    addWall(QVector3D(-5, floorLevel, 5), QVector3D(-5, floorLevel, -5), 3.0f, 0.2f, QColor(220, 220, 220), 4); // Left

    updateWallOpacities();

    for (const auto& wall : walls) {
        drawWall(wall);
    }
}

void RoomViewer::drawFurniture(const Furniture& f)
{
    glPushMatrix();
    glTranslatef(f.position.x(), f.position.y(), f.position.z());

    glRotatef(f.rotation, 0.0f, 1.0f, 0.0f);

    if (&f == &furniturePieces[selectedFurnitureIndex]) {
        QColor highlightColor = f.color.lighter(150);
        glColor3f(highlightColor.redF(), highlightColor.greenF(), highlightColor.blueF());
        drawWireframeBox(f.dimensions);
    }

    drawCuboid(f.dimensions, f.color);

    if (!f.shelfPositions.empty()) {
        QColor shelfColor = f.color.darker(120);
        for (float y : f.shelfPositions) {
            glPushMatrix();
            glTranslatef(0, y, 0);
            drawCuboid(
                QVector3D(f.dimensions.x()*0.95f, 0.02f, f.dimensions.z()*0.95f),
                shelfColor
                );
            glPopMatrix();
        }
    }

    if (f.name.contains("closet", Qt::CaseInsensitive)) {
        drawDoors(f.dimensions, f.color.darker(150));
    }

    glPopMatrix();
}

void RoomViewer::drawDoors(const QVector3D& dim, const QColor& color)
{
    float doorWidth = dim.x() / 2 - 0.01f;
    float doorThickness = 0.02f;

    // Left door
    glPushMatrix();
    glTranslatef(-dim.x()/4, 0, dim.z()/2 + doorThickness/2);
    glRotatef(45, 0, 1, 0);
    drawCuboid(QVector3D(doorWidth, dim.y(), doorThickness), color);
    glPopMatrix();

    // Right door
    glPushMatrix();
    glTranslatef(dim.x()/4, 0, dim.z()/2 + doorThickness/2);
    glRotatef(-45, 0, 1, 0);
    drawCuboid(QVector3D(doorWidth, dim.y(), doorThickness), color);
    glPopMatrix();
}

void RoomViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        selectedFurnitureIndex = selectFurnitureAtPosition(event->pos());
        if (selectedFurnitureIndex >= 0) {
            isDragging = true;
        }
    }

    if (event->button() == Qt::RightButton) {
        selectedFurnitureIndex = selectFurnitureAtPosition(event->pos());

        if (selectedFurnitureIndex >= 0) {
            QMenu menu(this);

            QAction* optimizeAction = menu.addAction("Optimize Cutting");
            QAction* rotateLeftAction = menu.addAction("Rotate Left");
            QAction* rotateRightAction = menu.addAction("Rotate Right");
            QAction* deleteAction = menu.addAction("Delete Furniture");

            QAction* selectedAction = menu.exec(event->globalPos());

            if (selectedAction == optimizeAction) {
                if (!optimizeButton) {
                    optimizeButton = new QPushButton("Optimize Cutting", this);
                    optimizeButton->setStyleSheet(
                        "QPushButton { background-color: #4CAF50; color: white; padding: 5px; }"
                        "QPushButton:hover { background-color: #45a049; }"
                        );
                    connect(optimizeButton, &QPushButton::clicked,
                            this, &RoomViewer::showCuttingOptimizer);
                }

                QPoint buttonPos = mapFromGlobal(
                    mapToGlobal(QPoint(
                        furniturePieces[selectedFurnitureIndex].position.x() + width()/2,
                        furniturePieces[selectedFurnitureIndex].position.y() + height()/2
                        ))
                    );
                optimizeButton->move(buttonPos);
                optimizeButton->show();
            }
            else if (selectedAction == rotateLeftAction) {
                furniturePieces[selectedFurnitureIndex].rotation -= 90.0f;
                update();
            }
            else if (selectedAction == rotateRightAction) {
                furniturePieces[selectedFurnitureIndex].rotation += 90.0f;
                update();
            }
            else if (selectedAction == deleteAction) {
                if (selectedFurnitureIndex >= 0 && selectedFurnitureIndex < furniturePieces.size()) {
                    furniturePieces.erase(furniturePieces.begin() + selectedFurnitureIndex);
                    selectedFurnitureIndex = -1;
                    if (optimizeButton) optimizeButton->hide();
                    update();
                }
            }
        } else {
            if (optimizeButton) optimizeButton->hide();
        }
    }

    lastMousePos = event->pos();
}


void RoomViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && selectedFurnitureIndex >= 0) {
        QPoint delta = event->pos() - lastMousePos;

        float sensitivity = 0.02f;
        QVector3D movement(delta.x() * sensitivity, 0, -delta.y() * sensitivity);

        glm::mat4 viewRot = glm::mat4(glm::mat3(view));
        glm::vec4 movementGL(movement.x(), movement.y(), movement.z(), 0);
        movementGL = viewRot * movementGL;

        QVector3D newPos = furniturePieces[selectedFurnitureIndex].position + QVector3D(movementGL.x, movementGL.y, movementGL.z);
        targetPosition = newPos;

        // const QVector3D& dim = furniturePieces[selectedFurnitureIndex].dimensions;
        // newPos.setX(qBound(-roomSize.x()/2 + dim.x()/2, newPos.x(), roomSize.x()/2 - dim.x()/2));
        // newPos.setZ(qBound(-roomSize.z()/2 + dim.z()/2, newPos.z(), roomSize.z()/2 - dim.z()/2));
        QVector3D rotatedDim = getRotatedDimensions(furniturePieces[selectedFurnitureIndex]);

        newPos.setX(qBound(-roomSize.x()/2 + rotatedDim.x()/2, newPos.x(), roomSize.x()/2 - rotatedDim.x()/2));
        newPos.setZ(qBound(-roomSize.z()/2 + rotatedDim.z()/2, newPos.z(), roomSize.z()/2 - rotatedDim.z()/2));


        furniturePieces[selectedFurnitureIndex].position = newPos;
        update();
    }
    else if (event->buttons() & Qt::RightButton) {
        int dx = event->x() - lastMousePos.x();
        int dy = event->y() - lastMousePos.y();
        yRot += dx * 0.5f;
        xRot += dy * 0.5f;
        xRot = qBound(-90.0f, xRot, 90.0f);
        update();
    }

    lastMousePos = event->pos();
}

void RoomViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isDragging) {
        isDragging = false;
        selectedFurnitureIndex = -1;
    }
}

void RoomViewer::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y() * 0.01f;
    cameraDistance = qBound(5.0f, cameraDistance - delta, 50.0f);

    view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, cameraDistance),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
        );

    update();
}

void RoomViewer::drawCuboid(const QVector3D& dim, const QColor& color)
{
    float w = dim.x() / 2.0f;  // Half width
    float h = dim.y() / 2.0f;  // Half height
    float d = dim.z() / 2.0f;  // Half depth

    glColor3f(color.redF(), color.greenF(), color.blueF());
    glBegin(GL_QUADS);

    // Front face
    glVertex3f(-w, -h, d);
    glVertex3f(w, -h, d);
    glVertex3f(w, h, d);
    glVertex3f(-w, h, d);

    // Back face
    glVertex3f(-w, -h, -d);
    glVertex3f(-w, h, -d);
    glVertex3f(w, h, -d);
    glVertex3f(w, -h, -d);

    // Top face
    glVertex3f(-w, h, -d);
    glVertex3f(-w, h, d);
    glVertex3f(w, h, d);
    glVertex3f(w, h, -d);

    // Bottom face
    glVertex3f(-w, -h, -d);
    glVertex3f(w, -h, -d);
    glVertex3f(w, -h, d);
    glVertex3f(-w, -h, d);

    // Right face
    glVertex3f(w, -h, -d);
    glVertex3f(w, h, -d);
    glVertex3f(w, h, d);
    glVertex3f(w, -h, d);

    // Left face
    glVertex3f(-w, -h, -d);
    glVertex3f(-w, -h, d);
    glVertex3f(-w, h, d);
    glVertex3f(-w, h, -d);

    glEnd();
}

int RoomViewer::selectFurnitureAtPosition(const QPoint& pos)
{
    QVector3D nearPoint = screenToWorld(pos, 0);
    QVector3D farPoint = screenToWorld(pos, 1);

    for (int i = 0; i < furniturePieces.size(); ++i) {
        const Furniture& f = furniturePieces[i];
        if (rayIntersectsAABB(nearPoint, farPoint,
                              f.position - f.dimensions/2,
                              f.position + f.dimensions/2)) {
            return i;
        }
    }
    return -1;
}

QVector3D RoomViewer::screenToWorld(const QPoint& screenPos, float y) const
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glm::vec3 screenPos3D(screenPos.x(), viewport[3] - screenPos.y(), y);
    glm::vec4 viewportGL(viewport[0], viewport[1], viewport[2], viewport[3]);

    glm::vec3 worldPos = glm::unProject(
        screenPos3D,
        view * modelMatrix,
        projection,
        viewportGL
        );

    return QVector3D(worldPos.x, worldPos.y, worldPos.z);
}

bool RoomViewer::rayIntersectsAABB(const QVector3D& rayStart, const QVector3D& rayEnd,
                                   const QVector3D& boxMin, const QVector3D& boxMax)
{
    QVector3D dir = (rayEnd - rayStart).normalized();
    QVector3D invDir(1.0f/dir.x(), 1.0f/dir.y(), 1.0f/dir.z());

    float t1 = (boxMin.x() - rayStart.x()) * invDir.x();
    float t2 = (boxMax.x() - rayStart.x()) * invDir.x();
    float t3 = (boxMin.y() - rayStart.y()) * invDir.y();
    float t4 = (boxMax.y() - rayStart.y()) * invDir.y();
    float t5 = (boxMin.z() - rayStart.z()) * invDir.z();
    float t6 = (boxMax.z() - rayStart.z()) * invDir.z();

    float tmin = qMax(qMax(qMin(t1, t2), qMin(t3, t4)), qMin(t5, t6));
    float tmax = qMin(qMin(qMax(t1, t2), qMax(t3, t4)), qMax(t5, t6));

    return tmax >= qMax(tmin, 0.0f);
}


void RoomViewer::drawWireframeBox(const QVector3D& dim)
{
    float w = dim.x()/2, h = dim.y()/2, d = dim.z()/2;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);

    glBegin(GL_QUADS);
    // Front
    glVertex3f(-w, -h, d);
    glVertex3f(w, -h, d);
    glVertex3f(w, h, d);
    glVertex3f(-w, h, d);
    // Back
    glVertex3f(-w, -h, -d);
    glVertex3f(-w, h, -d);
    glVertex3f(w, h, -d);
    glVertex3f(w, -h, -d);
    // ... other faces ...
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RoomViewer::updatePosition()
{
    if (selectedFurnitureIndex >= 0) {
        QVector3D& current = furniturePieces[selectedFurnitureIndex].position;
        velocity = (targetPosition - current) * 0.2f; // Smoothing factor
        current += velocity;
        update();
    }
}

void RoomViewer::drawCube(const QVector3D& center, const QVector3D& size, const QColor& color)
{
    glPushMatrix();
    glTranslatef(center.x(), center.y(), center.z());

    float w = size.x() / 2.0f;
    float h = size.y() / 2.0f;
    float d = size.z() / 2.0f;

    glColor3f(color.redF(), color.greenF(), color.blueF());
    glBegin(GL_QUADS);

    // Front
    glVertex3f(-w, -h, d);
    glVertex3f(w, -h, d);
    glVertex3f(w, h, d);
    glVertex3f(-w, h, d);

    // Back
    glVertex3f(-w, -h, -d);
    glVertex3f(-w, h, -d);
    glVertex3f(w, h, -d);
    glVertex3f(w, -h, -d);

    // Top
    glVertex3f(-w, h, -d);
    glVertex3f(-w, h, d);
    glVertex3f(w, h, d);
    glVertex3f(w, h, -d);

    // Bottom
    glVertex3f(-w, -h, -d);
    glVertex3f(w, -h, -d);
    glVertex3f(w, -h, d);
    glVertex3f(-w, -h, d);

    // Right
    glVertex3f(w, -h, -d);
    glVertex3f(w, h, -d);
    glVertex3f(w, h, d);
    glVertex3f(w, -h, d);

    // Left
    glVertex3f(-w, -h, -d);
    glVertex3f(-w, -h, d);
    glVertex3f(-w, h, d);
    glVertex3f(-w, h, -d);

    glEnd();
    glPopMatrix();
}

void RoomViewer::drawQuad(const QVector3D& p1, const QVector3D& p2, const QVector3D& p3, const QVector3D& p4, const QColor& color)
{
    glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    glBegin(GL_QUADS);
    glVertex3f(p1.x(), p1.y(), p1.z());
    glVertex3f(p2.x(), p2.y(), p2.z());
    glVertex3f(p3.x(), p3.y(), p3.z());
    glVertex3f(p4.x(), p4.y(), p4.z());
    glEnd();
}

void RoomViewer::addWall(const QVector3D& start, const QVector3D& end, float height, float thickness, const QColor& color, int id)
{
    walls.push_back({start, end, height, thickness, color, id, 1.0f});
    update();
}

void RoomViewer::drawWall(const Wall& wall)
{
    QVector3D direction = wall.end - wall.start;
    QVector3D perpendicular = QVector3D::crossProduct(direction, QVector3D(0, 1, 0)).normalized();
    perpendicular *= wall.thickness / 2.0f;

    QVector3D bottom1 = wall.start - perpendicular;
    QVector3D bottom2 = wall.end - perpendicular;
    QVector3D bottom3 = wall.end + perpendicular;
    QVector3D bottom4 = wall.start + perpendicular;

    QVector3D top1 = bottom1 + QVector3D(0, wall.height, 0);
    QVector3D top2 = bottom2 + QVector3D(0, wall.height, 0);
    QVector3D top3 = bottom3 + QVector3D(0, wall.height, 0);
    QVector3D top4 = bottom4 + QVector3D(0, wall.height, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QColor wallColor = wall.color;
    wallColor.setAlphaF(wall.opacity);

    drawQuad(bottom1, bottom2, top2, top1, wallColor); // Front
    drawQuad(bottom3, bottom4, top4, top3, wallColor); // Back
    drawQuad(bottom2, bottom3, top3, top2, wallColor); // Right
    drawQuad(bottom4, bottom1, top1, top4, wallColor); // Left

    drawQuad(top1, top2, top3, top4, wall.color.darker(110));

    glDisable(GL_BLEND);

}

const std::vector<RoomViewer::Furniture>& RoomViewer::getFurniturePieces() const {
    return  furniturePieces;
}

void RoomViewer::showCuttingOptimizer()
{
    if (selectedFurnitureIndex < 0 || selectedFurnitureIndex >= furniturePieces.size())
        return;

    if (!optimizer) {
        optimizer = new MaterialOptimizerWidget(this);
        optimizer->setWindowFlags(Qt::Dialog);
        optimizer->setWindowModality(Qt::ApplicationModal);
    }

    const auto& furniture = furniturePieces[selectedFurnitureIndex];
    float width = furniture.dimensions.x();
    float height = furniture.dimensions.y();
    float depth = furniture.dimensions.z();

    QList<Part> parts;

    parts.append(Part("Left Side Panel", width*1000, depth*1000));
    parts.append(Part("Right Side Panel", width*1000, depth*1000));

    parts.append(Part("Top Panel", width*1000, depth*1000));
    parts.append(Part("Bottom Panel", width*1000, depth*1000));

    for (int i = 0; i < furniture.shelfPositions.size(); i++) {
        parts.append(Part(
            QString("Shelf %1").arg(i+1),
            (width-0.1f)*1000,
            (depth-0.1f)*1000
            ));
    }

    if (depth >= 0.5f) { // Only add doors for deeper cabinets
        parts.append(Part("Left Door", width*500, height*1000)); // Half width
        parts.append(Part("Right Door", width*500, height*1000));
    }

    optimizer->setFurnitureParts(parts);
    optimizer->show();
}

void RoomViewer::updateWallOpacities()
{
    if (walls.empty()) return;

    for (auto& wall : walls) {
        wall.opacity = 1.0f;
    }

    int wallToFade = -1;
    float maxDot = -1;

    for (auto& wall : walls) {
        QVector3D wallDir = (wall.end - wall.start).normalized();
        QVector3D wallNormal(-wallDir.z(), 0, wallDir.x()); // Perpendicular in XZ plane

        float dot = QVector3D::dotProduct(lastCameraForward, wallNormal);

        if (dot > maxDot) {
            maxDot = dot;
            wallToFade = wall.id;
        }
    }

    if (wallToFade != -1 && maxDot > fadeStartAngle) {
        for (auto& wall : walls) {
            if (wall.id == wallToFade) {
                float fadeAmount = (maxDot - fadeStartAngle) / (fadeEndAngle - fadeStartAngle);
                fadeAmount = qBound(0.0f, fadeAmount, 1.0f);
                wall.opacity = 1.0f - fadeAmount;
                break;
            }
        }
    }
}

QVector3D RoomViewer::getRotatedDimensions(const Furniture& furniture) const
{
    float angleRad = glm::radians(furniture.rotation);

    float cosA = std::abs(std::cos(angleRad));
    float sinA = std::abs(std::sin(angleRad));

    float rotatedWidth = furniture.dimensions.x() * cosA + furniture.dimensions.z() * sinA;
    float rotatedDepth = furniture.dimensions.x() * sinA + furniture.dimensions.z() * cosA;

    return QVector3D(rotatedWidth, furniture.dimensions.y(), rotatedDepth);
}
