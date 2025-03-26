#include "StartupWindow.h"
#include <QLabel>
#include <QFileDialog>
#include <QInputDialog>

StartupWindow::StartupWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Select or Create a Project");
    setFixedSize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Title
    QLabel *title = new QLabel("Furniture Design App");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    // Create & Open Buttons
    QPushButton *createBtn = new QPushButton("Create New Project");
    QPushButton *openBtn = new QPushButton("Open Existing Project");

    layout->addWidget(createBtn);
    layout->addWidget(openBtn);

    // Recent Projects List
    recentProjectsList = new QListWidget();
    layout->addWidget(new QLabel("Recently Opened Projects:"));
    layout->addWidget(recentProjectsList);

    // Add dummy recent projects (later load from file)
    recentProjectsList->addItem("Living Room Design");
    recentProjectsList->addItem("Kitchen Layout");
    recentProjectsList->addItem("Office Space");

    // Connect buttons
    connect(createBtn, &QPushButton::clicked, this, &StartupWindow::handleCreateProject);
    connect(openBtn, &QPushButton::clicked, this, &StartupWindow::handleOpenProject);
    connect(recentProjectsList, &QListWidget::itemClicked, this, &StartupWindow::handleProjectSelection);
}

// 📌 Handle Create New Project
void StartupWindow::handleCreateProject() {
    emit createProjectRequested();
}

// 📌 Handle Open Existing Project
void StartupWindow::handleOpenProject() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Project", "", "Furniture Projects (*.furn)");
    if (!fileName.isEmpty()) {
        emit projectSelected(fileName);
    }
}

// 📌 Handle Recent Project Selection
void StartupWindow::handleProjectSelection() {
    if (recentProjectsList->currentItem()) {
        emit projectSelected(recentProjectsList->currentItem()->text());
    }
}
