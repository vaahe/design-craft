#include "createfurniturewidget.h"

CreateFurnitureWidget::CreateFurnitureWidget(QWidget *parent)
    : QDialog(parent),
    currentColor(QColor(139, 69, 19))
{
    setupUI();
    updatePreview();
    updateColorButton();

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
void CreateFurnitureWidget::setupUI()
{
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("My Closet");

    widthSpin = new QDoubleSpinBox(this);
    widthSpin->setRange(0.5, 5.0);
    widthSpin->setValue(2.0);
    widthSpin->setSuffix(" m");

    heightSpin = new QDoubleSpinBox(this);
    heightSpin->setRange(1.0, 3.0);
    heightSpin->setValue(2.2);
    heightSpin->setSuffix(" m");

    depthSpin = new QDoubleSpinBox(this);
    depthSpin->setRange(0.3, 1.5);
    depthSpin->setValue(0.6);
    depthSpin->setSuffix(" m");

    shelfCountSpin = new QSpinBox(this);
    shelfCountSpin->setRange(0, 10);
    shelfCountSpin->setValue(3);

    shelfSpacingSpin = new QDoubleSpinBox(this);
    shelfSpacingSpin->setRange(0.2, 1.0);
    shelfSpacingSpin->setValue(0.5);
    shelfSpacingSpin->setSuffix(" m");

    typeCombo = new QComboBox(this);
    typeCombo->addItems({"Closet", "Wardrobe", "Bookshelf", "Cabinet"});

    colorButton = new QPushButton("Select Color", this);
    createButton = new QPushButton("Create Furniture", this);
    createButton->setStyleSheet("background-color: #4CAF50; color: white;");

    previewWidget = new CreateFurnitureOpenGLWidget(this);
    previewWidget->setMinimumSize(600, 500);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Type:", typeCombo);
    formLayout->addRow("Width:", widthSpin);
    formLayout->addRow("Height:", heightSpin);
    formLayout->addRow("Depth:", depthSpin);
    formLayout->addRow("Shelves:", shelfCountSpin);
    formLayout->addRow("Shelf Spacing:", shelfSpacingSpin);
    formLayout->addRow("Color:", colorButton);
    formLayout->addWidget(createButton);

    // Left panel (inputs) - fixed width
    QWidget *inputPanel = new QWidget;
    inputPanel->setLayout(formLayout);
    inputPanel->setFixedWidth(350);

    QVBoxLayout *previewLayout = new QVBoxLayout;
    previewLayout->addWidget(previewWidget);
    previewLayout->setContentsMargins(10, 0, 10, 10);

    QWidget *previewPanel = new QWidget;
    previewPanel->setLayout(previewLayout);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(inputPanel);
    topLayout->addWidget(previewPanel, 1);

    QWidget *topWidget = new QWidget;
    topWidget->setLayout(topLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(topWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    auto updateSignal = QOverload<double>::of(&QDoubleSpinBox::valueChanged);
    connect(widthSpin, updateSignal, this, &CreateFurnitureWidget::updatePreview);
    connect(heightSpin, updateSignal, this, &CreateFurnitureWidget::updatePreview);
    connect(depthSpin, updateSignal, this, &CreateFurnitureWidget::updatePreview);
    connect(shelfSpacingSpin, updateSignal, this, &CreateFurnitureWidget::updatePreview);
    connect(shelfCountSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &CreateFurnitureWidget::updatePreview);
    connect(colorButton, &QPushButton::clicked, this, &CreateFurnitureWidget::pickColor);
    connect(createButton, &QPushButton::clicked, this, &CreateFurnitureWidget::createFurniture);
}

void CreateFurnitureWidget::updatePreview()
{
    previewWidget->setFurnitureDimensions(
        widthSpin->value(),
        heightSpin->value(),
        depthSpin->value(),
        currentColor,
        shelfCountSpin->value(),
        shelfSpacingSpin->value()
    );
}

void CreateFurnitureWidget::pickColor()
{
    QColor color = QColorDialog::getColor(currentColor, this, "Select Furniture Color");
    if (color.isValid()) {
        currentColor = color;
        updatePreview();
        updateColorButton();
    }
}

void CreateFurnitureWidget::updateColorButton()
{
    QString textColor = currentColor.lightness() > 150 ? "black" : "white";
    colorButton->setStyleSheet(QString("background-color: %1; color: %2; padding: 5px;").arg(currentColor.name()).arg(textColor));
}

void CreateFurnitureWidget::createFurniture()
{
    float width = widthSpin->value();
    float height = heightSpin->value();
    float depth = depthSpin->value();
    int shelves = shelfCountSpin->value();
    float spacing = shelfSpacingSpin->value();
    qDebug() << "shelves count:" << shelves;

    emit furnitureCreated(width, height, depth, currentColor, shelves, spacing);

    std::vector<float> shelfPositions = ClosetGenerator::calculateShelfPositions(height, shelves, spacing);

    emit closetCreated(nameEdit->text(), QVector3D(width, height, depth), currentColor, shelfPositions);

    accept();
}
