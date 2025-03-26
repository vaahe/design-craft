#include "maincontroller.h"

MainController::MainController(QWidget *parent) :
    QMainWindow(parent),
    m_stackedWidget(new QStackedWidget(this)),
    m_mainWindow(new MainWindow()),
    m_startupWindow(new StartupWindow())
{
    setStackedWidget();
    setCentralWidget(m_stackedWidget);

    connect(m_startupWindow, &StartupWindow::createProjectRequested, this, &MainController::onCreateProject);
    connect(m_startupWindow, &StartupWindow::projectSelected, this, &MainController::onSelectProject);
    connect(m_startupWindow, &StartupWindow::openProjectRequested, this, &MainController::onOpenProject);
}

void MainController::setStackedWidget() {
    m_stackedWidget->addWidget(m_startupWindow);
    m_stackedWidget->addWidget(m_mainWindow);
    m_stackedWidget->setCurrentWidget(m_startupWindow);
}

void MainController::onCreateProject() {
    qDebug() << "Create project request";
    m_stackedWidget->setCurrentWidget(m_mainWindow);

    int width = 10, height = 3, depth = 10;
    m_mainWindow->setRoomSize(width, height, depth);
    m_stackedWidget->setCurrentWidget(m_mainWindow);
}

void MainController::onOpenProject() {
    qDebug() << "Open project request";
}

void MainController::onSelectProject(const QString projectName) {
    qDebug() << "Project selected" << projectName;
}
