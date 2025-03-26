#ifndef CREATEFURNITUREWIDGET_H
#define CREATEFURNITUREWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColorDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>

#include "src/widgets/create_furniture_widget/createfurnitureopenglwidget.h"

class FurnitureDialog : public QDialog {
    Q_OBJECT

public:
    explicit FurnitureDialog(QWidget *parent = nullptr);

private slots:
    void pickColor();        // Open color picker
    void updatePreview();    // Update 3D preview
    void saveFurniture();    // Save furniture to DB

private:
    // Input Fields
    QLineEdit *nameEdit;
    QComboBox *categoryCombo;
    QColor selectedColor; // Store selected color

    // Sliders for dimensions
    QSlider *widthSlider;
    QSlider *heightSlider;
    QSlider *depthSlider;

    // 3D Preview
    CreateFurnitureOpenGLWidget *m_previewWidget;
};

#endif // CREATEFURNITUREWIDGET_H
