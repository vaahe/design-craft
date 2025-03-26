#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_glWidget(new OpenGLWidget(this)),
    m_statusLabel(new QLabel("Zoom: 1.0 | Yaw: 0° | Pitch: 0°", this))
{
    ui->setupUi(this);

    createMenuBar();
    createToolBar();
    createSidebar();
    createStatusBar();

    setShortcuts();
    setCentralWidget(m_glWidget);


    connect(m_glWidget, &OpenGLWidget::viewUpdated, this, &MainWindow::setStatusLabel);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::createMenuBar() {
    connect(ui->resetViewAction, &QAction::triggered, this, [this]() {m_glWidget->resetView();});
    connect(ui->showGridAction, &QAction::triggered, this, [this]() {m_glWidget->enableGrid();});
}

void MainWindow::createStatusBar() {
    QWidget *statusWidget = new QWidget(this);
    QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);
    statusLayout->setContentsMargins(0, 0, 0, 0);

    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();

    ////
    QToolButton *wallControllerButton = new QToolButton(this);
    wallControllerButton->setText("Show walls");

    WallControllerWidget *widget = new WallControllerWidget();
    connect(wallControllerButton, &QToolButton::clicked, this, [widget]() { widget->show();} );
    connect(widget, &WallControllerWidget::wallColorChanged, m_glWidget, &OpenGLWidget::onWallColorChanged);
    connect(widget, &WallControllerWidget::wallOpacityChanged, m_glWidget, &OpenGLWidget::onWallOpacityChanged);
    ////

    QToolButton *zoomInButton = new QToolButton(this);
    zoomInButton->setDefaultAction(ui->zoomInAction);

    QToolButton *zoomOutButton = new QToolButton(this);
    zoomOutButton->setDefaultAction(ui->zoomOutAction);

    statusLayout->addWidget(wallControllerButton);
    statusLayout->addWidget(new QLabel("Zoom:"));
    statusLayout->addWidget(zoomInButton);
    statusLayout->addWidget(zoomOutButton);

    statusWidget->setLayout(statusLayout);
    statusBar()->addWidget(statusWidget, 1);
}

void MainWindow::createToolBar() {
    QToolBar *toolBar = addToolBar("Main Toolbar");

    toolBar->addAction("Create furniture", this, &MainWindow::createFurniture);
    toolBar->addAction("Open", this, []() { /* TODO: Open project */ });
    toolBar->addAction("Save", this, []() { /* TODO: Save project */ });
    toolBar->addSeparator();
    toolBar->addAction("Undo", this, []() { /* TODO: Undo action */ });
    toolBar->addAction("Redo", this, []() { /* TODO: Redo action */ });

    addToolBar(Qt::TopToolBarArea, toolBar);
}

void MainWindow::createSidebar() {
    QDockWidget *sidebar = new QDockWidget("Sidebar", this);

    // Main sidebar widget
    QWidget *sidebarWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();

    // Dropdown menu
    QComboBox *categoryDropdown = new QComboBox();
    categoryDropdown->addItem("Furniture Catalog");
    categoryDropdown->addItem("Floor");
    categoryDropdown->addItem("Wall");
    categoryDropdown->addItem("Windows");
    categoryDropdown->addItem("Doors");

    // List widget for displaying items
    QListWidget *listWidget = new QListWidget();
    listWidget->setIconSize(QSize(64, 64));

    // Define section items (category -> list of items)
    QMap<QString, QList<QPair<QString, QString>>> categoryItems = {
                                                                   {"Furniture Catalog", {
                                                                                             {":/images/chair.png", "Chairs"},
                                                                                             {":/images/table.png", "Tables"},
                                                                                             {":/images/sofa.png", "Sofas"},
                                                                                             {":/images/bed.png", "Beds"}
                                                                                         }},
                                                                   {"Floor", {
                                                                                 {":/images/tile.png", "Tiles"},
                                                                                 {":/images/wood.png", "Wooden Floor"}
                                                                             }},
                                                                   {"Wall", {
                                                                                {":/images/wallpaper.png", "Wallpaper"},
                                                                                {":/images/paint.png", "Paint"}
                                                                            }},
                                                                   };

    // Ensure categoryItems is captured safely
    auto updateList = [&, listWidget, categoryItems](const QString &selectedCategory) {
        listWidget->clear();

        // Check if category exists in map
        if (categoryItems.contains(selectedCategory)) {
            for (const auto &item : categoryItems[selectedCategory]) {
                addFurnitureItem(listWidget, item.first, item.second);
            }
        }
    };

    // Initialize with the default category
    updateList("Furniture Catalog");

    // Add widgets to layout
    layout->addWidget(categoryDropdown);
    layout->addWidget(listWidget);
    sidebarWidget->setLayout(layout);
    sidebar->setWidget(sidebarWidget);
    addDockWidget(Qt::LeftDockWidgetArea, sidebar);

    // Connect dropdown selection change event
    connect(categoryDropdown, &QComboBox::currentTextChanged, updateList);
}



// Helper function to add an item with an image
void MainWindow::addFurnitureItem(QListWidget *listWidget, const QString &imagePath, const QString &title) {
    QListWidgetItem *item = new QListWidgetItem(QIcon(imagePath), title);
    item->setSizeHint(QSize(100, 80));  // Adjust the item size
    listWidget->addItem(item);
}


void MainWindow::setRoomSize(int width, int height, int depth)
{
    if (m_glWidget) {
        m_glWidget->setRoomSize(width, height, depth);
    }
}

void MainWindow::loadProject(const QString &fileName)
{
    // TODO: Load project data from file
    // setRoomSize(10, 3, 10); // Example: Default room size
    qDebug() << "loaded project" << fileName;
}

void MainWindow::setStatusLabel(float zoom, float yaw, float pitch) {
    m_statusLabel->setText(QString("Zoom: %1 | Yaw: %2° | Pitch: %3°").arg(zoom).arg(yaw).arg(pitch));
}

void MainWindow::setShortcuts() {
    // file menu
    ui->newAction->setShortcut(QKeySequence::New);
    ui->openAction->setShortcut(QKeySequence::Open);
    ui->saveAction->setShortcut(QKeySequence::Save);
    ui->quitAction->setShortcut(QKeySequence::Quit);
    ui->saveAsAction->setShortcut(QKeySequence::SaveAs);
    ui->homeAction->setShortcut(QKeySequence("Ctrl+H"));
    ui->exportAction->setShortcut(QKeySequence("Ctrl+E"));

    // edit menu
    ui->undoAction->setShortcut(QKeySequence::Undo);
    ui->redoAction->setShortcut(QKeySequence::Redo);
    ui->cutAction->setShortcut(QKeySequence::Cut);
    ui->copyAction->setShortcut(QKeySequence::Copy);
    ui->pasteAction->setShortcut(QKeySequence::Paste);
    ui->deleteAction->setShortcut(QKeySequence::Delete);
    ui->selectAllAction->setShortcut(QKeySequence::SelectAll);

    // view menu
    ui->zoomInAction->setShortcut(QKeySequence::ZoomIn);
    ui->zoomOutAction->setShortcut(QKeySequence::ZoomOut);

    connect(ui->quitAction, &QAction::triggered, this, &MainWindow::close);
    connect(ui->newAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->homeAction, &QAction::triggered, this, &MainWindow::homeAction);
    connect(ui->saveAsAction, &QAction::triggered, this, &MainWindow::saveAsFile);
    connect(ui->exportAction, &QAction::triggered, this, &MainWindow::exportAction);

    connect(ui->cutAction, &QAction::triggered, this, &MainWindow::cutAction);
    connect(ui->undoAction, &QAction::triggered, this, &MainWindow::undoAction);
    connect(ui->redoAction, &QAction::triggered, this, &MainWindow::redoAction);
    connect(ui->copyAction, &QAction::triggered, this, &MainWindow::copyAction);
    connect(ui->pasteAction, &QAction::triggered, this, &MainWindow::pasteAction);
    connect(ui->deleteAction, &QAction::triggered, this, &MainWindow::deleteAction);

    connect(ui->zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);
    connect(ui->zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);
}

void MainWindow::newFile() {
    QMessageBox::information(this, "New File", "Create a new file.");
}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "All Files (*.*)");
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "File Opened", "Opened file: " + fileName);
    }
}

void MainWindow::saveFile() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "All Files (*.*)");
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "File Saved", "Saved file: " + fileName);
    }
}

void MainWindow::saveAsFile() {
    saveFile();
}

void MainWindow::closeApp() {
    QApplication::quit();
}

void MainWindow::undoAction() {
    QMessageBox::information(this, "Undo", "Undo action triggered.");
}

void MainWindow::redoAction() {
    QMessageBox::information(this, "Redo", "Redo action triggered.");
}

void MainWindow::cutAction() {
    QMessageBox::information(this, "Cut", "Cut action triggered.");
}

void MainWindow::copyAction() {
    QMessageBox::information(this, "Copy", "Copy action triggered.");
}

void MainWindow::pasteAction() {
    QMessageBox::information(this, "Paste", "Paste action triggered.");
}

void MainWindow::deleteAction() {
    QMessageBox::information(this, "Delete", "Delete action triggered.");
}

void MainWindow::selectAllAction() {
    QMessageBox::information(this, "Select All", "Select all action triggered.");
}

void MainWindow::homeAction() {
    QMessageBox::information(this, "Home", "Home button triggered.");
}

void MainWindow::exportAction() {
    QMessageBox::information(this, "Export", "Export action triggered.");
}

void MainWindow::zoomIn() {
    m_glWidget->zoomIn();
}

void MainWindow::zoomOut() {
    m_glWidget->zoomOut();
}

void MainWindow::createFurniture() {
    FurnitureDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // Refresh UI or fetch updated furniture list
    }
}
