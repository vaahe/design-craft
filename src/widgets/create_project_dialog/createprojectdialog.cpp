#include "CreateProjectDialog.h"
#include <QLabel>

CreateProjectDialog::CreateProjectDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Create New Project");
    setFixedSize(300, 200);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Width input
    layout->addWidget(new QLabel("Room Width (m):"));
    widthInput = new QSpinBox();
    widthInput->setRange(1, 50);
    widthInput->setValue(10);
    layout->addWidget(widthInput);

    // Height input
    layout->addWidget(new QLabel("Room Height (m):"));
    heightInput = new QSpinBox();
    heightInput->setRange(1, 50);
    heightInput->setValue(3);
    layout->addWidget(heightInput);

    // Depth input
    layout->addWidget(new QLabel("Room Depth (m):"));
    depthInput = new QSpinBox();
    depthInput->setRange(1, 50);
    depthInput->setValue(10);
    layout->addWidget(depthInput);

    // Confirm Button
    QPushButton *confirmBtn = new QPushButton("Create Project");
    layout->addWidget(confirmBtn);

    connect(confirmBtn, &QPushButton::clicked, this, &QDialog::accept);
}

int CreateProjectDialog::getRoomWidth() const { return widthInput->value(); }
int CreateProjectDialog::getRoomHeight() const { return heightInput->value(); }
int CreateProjectDialog::getRoomDepth() const { return depthInput->value(); }
