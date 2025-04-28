#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "src/widgets/room_viewer_widget/roomviewer.h"
#include "src/widgets/create_furniture_widget/createfurniturewidget.h"
#include "src/widgets/material_optimizer_widget/materialoptimizerwidget.h"
#include "src/data_structures/structs/part_types/part_types.h"

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
    void setupFurnitureDesigner();

    void addFurnitureItem(QListWidget *listWidget, const QString &imagePath, const QString &title);

public slots:
    void setRoomSize(int width, int height, int depth);
    void loadProject(const QString &fileName);
    void setStatusLabel(float zoom, float yaw, float pitch);

    void addFurnitureToRoom(float width, float height, float depth, const QColor& color, int shelves, float spacing);

    // File operations
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void closeApp();

    // Edit operations
    void undoAction();
    void redoAction();
    void cutAction();
    void copyAction();
    void pasteAction();
    void deleteAction();
    void selectAllAction();

    // View operations
    void homeAction();
    void exportAction();
    void zoomIn();
    void zoomOut();

signals:
    void homeWindowRequested();

private:
    Ui::MainWindow *ui;
    RoomViewer *m_roomViewer;
    CreateFurnitureWidget *m_furnitureDesigner;
    QLabel *m_statusLabel;
    MaterialOptimizerWidget *m_materialOptimizer;

    QVector3D m_roomSize;
};

#endif // MAINWINDOW_H
