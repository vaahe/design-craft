#include "StartupWindow.h"
#include <QLabel>
#include <QFileDialog>
#include <QInputDialog>
#include <QIcon>

StartupWindow::StartupWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Select or Create a Project");
    setFixedSize(420, 350);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    QLabel *logo = new QLabel();
    QPixmap pixmap(":/icons/images/logo.png");
    logo->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio));
    logo->setAlignment(Qt::AlignCenter);
    layout->addWidget(logo);

    QLabel *title = new QLabel("Furniture Design App");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QPushButton *createBtn = new QPushButton("Create New Project");
    QPushButton *openBtn = new QPushButton("Open Existing Project");

    layout->addWidget(createBtn);
    layout->addWidget(openBtn);

    QLabel *recentLabel = new QLabel("Recently Opened Projects:");
    layout->addWidget(recentLabel);

    recentProjectsList = new QListWidget();

    recentProjectsList->addItem("Living Room Design");
    recentProjectsList->addItem("Kitchen Layout");
    recentProjectsList->addItem("Office Space");

    layout->addWidget(recentProjectsList);

    connect(createBtn, &QPushButton::clicked, this, &StartupWindow::handleCreateProject);
    connect(openBtn, &QPushButton::clicked, this, &StartupWindow::handleOpenProject);
    connect(recentProjectsList, &QListWidget::itemClicked, this, &StartupWindow::handleProjectSelection);
}

void StartupWindow::handleCreateProject() {
    emit createProjectRequested();
}

void StartupWindow::handleOpenProject() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Project", "", "Design Craft Projects (*.decr)");

    if (fileName.isEmpty()) return;

    if (!fileName.endsWith(".decr", Qt::CaseInsensitive)) {
        qDebug() << "Invalid file format! Please select a .decr file.";
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open project file!";
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    if (jsonDoc.isNull()) {
        qDebug() << "Invalid JSON format in project file!";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    qDebug() << "Loaded project:" << jsonObj["project_name"].toString();

    emit projectSelected(jsonObj["project_name"].toString());
}

void StartupWindow::handleProjectSelection() {
    if (recentProjectsList->currentItem()) {
        emit projectSelected(recentProjectsList->currentItem()->text());
    }
}
