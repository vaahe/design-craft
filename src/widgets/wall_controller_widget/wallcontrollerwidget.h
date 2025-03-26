#ifndef WALLCONTROLLERWIDGET_H
#define WALLCONTROLLERWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QSlider>  // Add QSlider for opacity control
#include <QLabel>   // Optional: for displaying opacity value

#include "src/data_structures/enums/WallsEnum.h"

class WallControllerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WallControllerWidget(QWidget *parent = nullptr);

signals:
    void wallColorChanged(const QColor &color, WallType wallType);
    void wallOpacityChanged(float opacity);  // Signal to notify opacity change

private slots:
    void onOpacityChanged(int value);  // Slot to handle slider changes

private:
    QToolButton *leftWallButton;
    QToolButton *rightWallButton;
    QToolButton *frontWallButton;
    QToolButton *backWallButton;

    QSlider *opacitySlider;  // The opacity slider
    QLabel *opacityLabel;    // Optional label to display current opacity value
};

#endif // WALLCONTROLLERWIDGET_H
