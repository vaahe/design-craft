#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "src/widgets/openglwidget.h"
#include "src/widgets/wall_controller_widget/wallcontrollerwidget.h"
#include "src/widgets/create_furniture_widget/createfurniturewidget.h"

#include <QMap>
#include <QLabel>
#include <QToolBar>
#include <QMainWindow>
#include <QListWidget>
#include <QFileDialog>
#include <QDockWidget>
#include <QComboBox>
#include <QVBoxLayout>
#include <QToolButton>
#include <QMessageBox>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createMenuBar();
    void createToolBar();
    void createSidebar();
    void createStatusBar();
    void setShortcuts();

    void addFurnitureItem(QListWidget *listWidget, const QString &imagePath, const QString &title);
    void createFurniture();

public slots:
    void setRoomSize(int width, int height, int depth);
    void loadProject(const QString &fileName);
    void setStatusLabel(float zoom, float yaw, float pitch);

    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void closeApp();

    void undoAction();
    void redoAction();
    void cutAction();
    void copyAction();
    void pasteAction();
    void deleteAction();
    void selectAllAction();

    void homeAction();
    void exportAction();

    void zoomIn();
    void zoomOut();

private:
    Ui::MainWindow *ui;
    OpenGLWidget *m_glWidget;
    QLabel *m_statusLabel;
};
#endif // MAINWINDOW_H
