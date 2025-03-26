#ifndef CREATEPROJECTDIALOG_H
#define CREATEPROJECTDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>

class CreateProjectDialog : public QDialog {
    Q_OBJECT

public:
    explicit CreateProjectDialog(QWidget *parent = nullptr);
    int getRoomWidth() const;
    int getRoomHeight() const;
    int getRoomDepth() const;

private:
    QSpinBox *widthInput;
    QSpinBox *heightInput;
    QSpinBox *depthInput;
};

#endif // CREATEPROJECTDIALOG_H
