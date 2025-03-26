#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "src/windows/mainwindow.h"
#include "src/windows/startup_window/startupwindow.h"

#include <QObject>
#include <QMainWindow>
#include <QStackedWidget>

class MainController : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainController(QWidget *parent = nullptr);

    void setStackedWidget();

private slots:
    void onCreateProject();
    void onOpenProject();
    void onSelectProject(const QString projectName);

signals:

private:
    QStackedWidget *m_stackedWidget;
    MainWindow *m_mainWindow;
    StartupWindow *m_startupWindow;
};

#endif // MAINCONTROLLER_H
