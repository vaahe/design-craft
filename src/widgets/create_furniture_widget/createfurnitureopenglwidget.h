#ifndef CREATEFURNITUREOPENGLWIDGET_H
#define CREATEFURNITUREOPENGLWIDGET_H

#include <QColor>
#include <QString>
#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class CreateFurnitureOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit CreateFurnitureOpenGLWidget(QWidget *parent = nullptr);

    void setFurnitureAttributes(const QString &category, const QColor &color,
                                int width, int height, int depth);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    QString furnitureCategory;
    QColor furnitureColor;
    int furnitureWidth;
    int furnitureHeight;
    int furnitureDepth;
};


#endif // CREATEFURNITUREOPENGLWIDGET_H
