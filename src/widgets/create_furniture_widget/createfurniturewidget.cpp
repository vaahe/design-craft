#include "createfurniturewidget.h"

FurnitureDialog::FurnitureDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Create Furniture");
    resize(800, 500); // Set dialog size

    // ** Left Panel: Input Fields **
    QLabel *nameLabel = new QLabel("Name:");
    nameEdit = new QLineEdit(this);

    QLabel *categoryLabel = new QLabel("Category:");
    categoryCombo = new QComboBox(this);
    categoryCombo->addItems({"Chair", "Table", "Sofa", "Bed"}); // More categories

    QPushButton *colorButton = new QPushButton("Pick Color");
    connect(colorButton, &QPushButton::clicked, this, &FurnitureDialog::pickColor);

    QLabel *dimensionsLabel = new QLabel("Adjust Dimensions:");
    widthSlider = new QSlider(Qt::Horizontal);
    heightSlider = new QSlider(Qt::Horizontal);
    depthSlider = new QSlider(Qt::Horizontal);

    widthSlider->setRange(10, 200);
    heightSlider->setRange(10, 200);
    depthSlider->setRange(10, 200);

    connect(widthSlider, &QSlider::valueChanged, this, &FurnitureDialog::updatePreview);
    connect(heightSlider, &QSlider::valueChanged, this, &FurnitureDialog::updatePreview);
    connect(depthSlider, &QSlider::valueChanged, this, &FurnitureDialog::updatePreview);

    QPushButton *saveButton = new QPushButton("Save");
    connect(saveButton, &QPushButton::clicked, this, &FurnitureDialog::saveFurniture);

    QVBoxLayout *inputLayout = new QVBoxLayout();
    inputLayout->addWidget(nameLabel);
    inputLayout->addWidget(nameEdit);
    inputLayout->addWidget(categoryLabel);
    inputLayout->addWidget(categoryCombo);
    inputLayout->addWidget(colorButton);
    inputLayout->addWidget(dimensionsLabel);
    inputLayout->addWidget(widthSlider);
    inputLayout->addWidget(heightSlider);
    inputLayout->addWidget(depthSlider);
    inputLayout->addWidget(saveButton);

    // ** Right Panel: OpenGL Preview **
    m_previewWidget = new CreateFurnitureOpenGLWidget(this);
    m_previewWidget->setMinimumSize(400, 400); // Set fixed preview size

    QVBoxLayout *previewLayout = new QVBoxLayout();
    previewLayout->addWidget(m_previewWidget);

    // ** Combine Both Panels into One Layout **
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(inputLayout, 1);  // Left Panel (Inputs)
    mainLayout->addLayout(previewLayout, 2); // Right Panel (Preview)

    setLayout(mainLayout);
}

// Pick color function
void FurnitureDialog::pickColor() {
    QColor color = QColorDialog::getColor(Qt::white, this, "Select Color");
    if (color.isValid()) {
        selectedColor = color;
        updatePreview();
    }
}

// Update 3D model preview
void FurnitureDialog::updatePreview() {
    m_previewWidget->setFurnitureAttributes(
        categoryCombo->currentText(), selectedColor,
        widthSlider->value(), heightSlider->value(), depthSlider->value()
        );
}

// Save furniture to database
void FurnitureDialog::saveFurniture() {
    QString name = nameEdit->text();
    QString category = categoryCombo->currentText();
    QColor color = selectedColor;

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a name for the furniture.");
        return;
    }
}
