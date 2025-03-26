#include "WallControllerWidget.h"

WallControllerWidget::WallControllerWidget(QWidget *parent)
    : QWidget(parent),
    leftWallButton(new QToolButton(this)),
    rightWallButton(new QToolButton(this)),
    frontWallButton(new QToolButton(this)),
    backWallButton(new QToolButton(this)),
    opacitySlider(new QSlider(Qt::Horizontal, this)),
    opacityLabel(new QLabel("Opacity: 1.0", this))  // Initialize label
{
    // Set up the layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Set up tool buttons for wall color change
    leftWallButton->setText("Left Wall");
    rightWallButton->setText("Right Wall");
    frontWallButton->setText("Front Wall");
    backWallButton->setText("Back Wall");

    layout->addWidget(leftWallButton);
    layout->addWidget(rightWallButton);
    layout->addWidget(frontWallButton);
    layout->addWidget(backWallButton);

    // Set up the opacity slider
    opacitySlider->setRange(0, 100);  // 0 to 100 for opacity control
    opacitySlider->setValue(100);     // Default to full opacity
    layout->addWidget(opacityLabel);
    layout->addWidget(opacitySlider);

    // Connect signals to slots
    connect(leftWallButton, &QToolButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(Qt::white, this, "Select Left Wall Color");
        if (color.isValid()) {
            emit wallColorChanged(color, LEFT);
        }
    });
    connect(rightWallButton, &QToolButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(Qt::white, this, "Select Right Wall Color");
        if (color.isValid()) {
            emit wallColorChanged(color, RIGHT);
        }
    });
    connect(frontWallButton, &QToolButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(Qt::white, this, "Select Front Wall Color");
        if (color.isValid()) {
            emit wallColorChanged(color, FRONT);
        }
    });
    connect(backWallButton, &QToolButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(Qt::white, this, "Select Back Wall Color");
        if (color.isValid()) {
            emit wallColorChanged(color, BACK);
        }
    });

    // Connect the slider value change signal to the slot
    connect(opacitySlider, &QSlider::valueChanged, this, &WallControllerWidget::onOpacityChanged);
}

void WallControllerWidget::onOpacityChanged(int value)
{
    // Map the slider value (0-100) to a float value (0.0-1.0)
    float opacity = value / 100.0f;
    opacityLabel->setText(QString("Opacity: %1").arg(opacity, 0, 'f', 2));  // Optional: update label text
    emit wallOpacityChanged(opacity);  // Emit signal to notify that opacity has changed
}
