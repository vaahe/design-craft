#ifndef CREATEFURNITUREWIDGET_H
#define CREATEFURNITUREWIDGET_H

#include "src/widgets/create_furniture_widget/createfurnitureopenglwidget.h"
#include "src/core/furniture_generator/ClosetGenerator.h"


#include <QDialog>
#include <QColor>
#include <QFormLayout>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QSplitter>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class CreateFurnitureWidget : public QDialog {
    Q_OBJECT

public:
    explicit CreateFurnitureWidget(QWidget *parent = nullptr);

signals:
    void furnitureCreated(float width, float height, float depth, const QColor& color, int shelves, float shelfSpacing);
    void closetCreated(QString, QVector3D, QColor, std::vector<float>);

private slots:
    void updatePreview();
    void createFurniture();
    void pickColor();

private:
    void setupUI();
    void updateColorButton();

    // UI Elements
    QLineEdit *nameEdit;
    QDoubleSpinBox *widthSpin;
    QDoubleSpinBox *heightSpin;
    QDoubleSpinBox *depthSpin;
    QSpinBox *shelfCountSpin;
    QDoubleSpinBox *shelfSpacingSpin;
    QComboBox *typeCombo;
    QPushButton *colorButton;
    QPushButton *createButton;
    CreateFurnitureOpenGLWidget *previewWidget;

    // Data
    QColor currentColor;
};

#endif // CREATEFURNITUREWIDGET_H
