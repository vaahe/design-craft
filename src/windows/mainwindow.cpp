#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_roomSize(10.0f, 3.0f, 10.0f)
{
    ui->setupUi(this);

    m_roomViewer = new RoomViewer(this);
    setCentralWidget(m_roomViewer);

    m_furnitureDesigner = new CreateFurnitureWidget(this);
    setupFurnitureDesigner();

    createMenuBar();
    createToolBar();
    createSidebar();
    createStatusBar();
    setShortcuts();
}

void MainWindow::setupFurnitureDesigner()
{
    connect(m_furnitureDesigner, &CreateFurnitureWidget::furnitureCreated,
            this, &MainWindow::addFurnitureToRoom);

    QToolButton *designButton = new QToolButton(this);
    designButton->setText("Design Furniture");
    designButton->setIcon(QIcon(":/icons/design.png"));
    // ui->mainToolBar->addWidget(designButton);
    connect(designButton, &QToolButton::clicked,
            m_furnitureDesigner, &QDialog::exec);
}


void MainWindow::addFurnitureToRoom(float width, float height, float depth, const QColor& color, int shelves, float spacing)
{
    std::vector<float> shelfPositions;
    float currentY = spacing - height/2;
    for (int i = 0; i < shelves; i++) {
        shelfPositions.push_back(currentY);
        currentY += spacing;
    }

    m_roomViewer->addFurniture(
        QVector3D(0, -m_roomSize.y()/2 + height/2, -m_roomSize.z()/2 + 0.1f),
        QVector3D(width, height, depth),
        color,
        "Custom Furniture",
        shelfPositions
        );
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::createMenuBar() {
    // connect(ui->resetViewAction, &QAction::triggered, this, [this]() {m_glWidget->resetView();});
    // connect(ui->showGridAction, &QAction::triggered, this, [this]() {m_glWidget->enableGrid();});
}

void MainWindow::createStatusBar() {
    QWidget *statusWidget = new QWidget(this);
    QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);
    statusLayout->setContentsMargins(0, 0, 0, 0);

    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();

    QToolButton *zoomInButton = new QToolButton(this);
    zoomInButton->setDefaultAction(ui->zoomInAction);

    QToolButton *zoomOutButton = new QToolButton(this);
    zoomOutButton->setDefaultAction(ui->zoomOutAction);

    statusLayout->addWidget(new QLabel("Zoom:"));
    statusLayout->addWidget(zoomInButton);
    statusLayout->addWidget(zoomOutButton);

    statusWidget->setLayout(statusLayout);
    statusBar()->addWidget(statusWidget, 1);
}

void MainWindow::createToolBar() {
    QToolBar *toolBar = addToolBar("Main Toolbar");

    toolBar->addAction("Open", this, []() { /* TODO: Open project */ });
    toolBar->addAction("Save", this, []() { /* TODO: Save project */ });
    toolBar->addSeparator();
    toolBar->addAction("Undo", this, []() { /* TODO: Undo action */ });
    toolBar->addAction("Redo", this, []() { /* TODO: Redo action */ });
    toolBar->addAction("Calculate material", this, [this]() {
        auto furnitureList = m_roomViewer->getFurniturePieces();

        QList<Part> allPartsMM;

        for (const auto& f : furnitureList) {
            auto parts = ClosetGenerator::generateClosetParts(
                f.dimensions.x(), f.dimensions.y(), f.dimensions.z(),
                static_cast<int>(f.shelfPositions.size()),
                f.shelfPositions.empty() ? 0.5f : (f.shelfPositions[1] - f.shelfPositions[0])
            );

            for (const auto& part : parts) {
                allPartsMM.append({
                    part.name,
                    part.dimensions.x() * 1000.0f,  // width
                    part.dimensions.y() * 1000.0f   // height
                });
            }
        }

        if (allPartsMM.isEmpty()) {
            QMessageBox::information(this, "No Furniture", "There is no furniture in the room.");
            return;
        }

        auto *optimizer = new MaterialOptimizerWidget();
        optimizer->setFurnitureParts(allPartsMM);
        optimizer->show();
    });

    addToolBar(Qt::TopToolBarArea, toolBar);
}

void MainWindow::createSidebar() {
    QDockWidget *sidebar = new QDockWidget("Design Elements", this);
    sidebar->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    QWidget *sidebarWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    QComboBox *categoryDropdown = new QComboBox();
    categoryDropdown->setStyleSheet("QComboBox { padding: 5px; font-size: 14px; }");
    categoryDropdown->addItem("Furniture Catalog");
    categoryDropdown->addItem("Flooring");
    categoryDropdown->addItem("Wall Treatments");
    categoryDropdown->addItem("Windows");
    categoryDropdown->addItem("Doors");
    categoryDropdown->addItem("Lighting");
    categoryDropdown->addItem("Decorations");

    QListWidget *listWidget = new QListWidget();
    listWidget->setStyleSheet(R"(
        QListWidget {
            background: white;
            border: none;
        }
        QListWidget::item {
            border: 1px solid #ccc;
            margin: 0;
            padding: 5px;
            background: #f8f8f8;
        }
        QListWidget::item:selected {
            background: #ddd;
        }
    )");

    listWidget->setViewMode(QListWidget::IconMode);
    listWidget->setIconSize(QSize(80, 80));
    listWidget->setResizeMode(QListWidget::Adjust);
    listWidget->setMovement(QListWidget::Static);
    listWidget->setSpacing(10);

    QMap<QString, QList<QPair<QString, QString>>> categoryItems = {
        {"Furniture Catalog", {
                                  {":/icons/images/armchair.png", "Armchair"},
                                  {":/icons/images/sofa.png", "Sofa"},
                                  {":/icons/images/table.png", "Dining Table"},
                                  {":/icons/images/coffee_table.png", "Coffee Table"},
                                  {":/icons/images/desk.png", "Office Desk"},
                                  {":/icons/images/bed.png", "Queen Bed"},
                                  {":/icons/images/wardrobe.png", "Wardrobe"},
                                  {":/icons/images/bookshelf.png", "Bookshelf"},
                                  {":/icons/images/tv_stand.png", "TV Stand"}
                              }},
        {"Flooring", {
                         {":/icons/images/hardwood.png", "Hardwood"},
                         {":/icons/images/tile.png", "Ceramic Tile"},
                         {":/icons/images/marble.png", "Marble"},
                         {":/icons/images/carpet.png", "Carpet"},
                         {":/icons/images/vinyl.png", "Vinyl Plank"},
                         {":/icons/images/laminate.png", "Laminate"},
                         {":/icons/images/bamboo.png", "Bamboo"},
                         {":/icons/images/concrete.png", "Polished Concrete"}
                     }},
        {"Wall Treatments", {
                                {":/icons/images/paint.png", "Paint"},
                                {":/icons/images/wallpaper.png", "Wallpaper"},
                                {":/icons/images/stone_wall.png", "Stone Veneer"},
                                {":/icons/images/tile.png", "Tile"},
                            }},
        {"Windows", {
                        {":/icons/images/casement.png", "Casement"},
                        {":/icons/images/double_hung.png", "Double Hung"},
                        {":/icons/images/panorama.png", "Panorama"}
                    }},
        {"Doors", {
                      {":/icons/images/wooden_door.png", "Wooden Door"},
                      {":/icons/images/double_hund_door.png", "Double hund Door"}
                  }},
        {"Lighting", {
                         {":/icons/images/chandelier.png", "Chandelier"},
                         {":/icons/images/pendant.png", "Pendant"},
                         {":/icons/images/sconce.png", "Wall Sconce"},
                         {":/icons/images/floor_lamp.png", "Floor Lamp"},
                         {":/icons/images/table_lamp.png", "Table Lamp"}
                     }},
        {"Decorations", {
                            {":/icons/images/rug.png", "Area Rug"},
                            {":/icons/images/curtains.png", "Curtains"},
                            {":/icons/images/mirror.png", "Mirror"},
                            {":/icons/images/vase.png", "Vase"},
                            {":/icons/images/plant.png", "Indoor Plant"}
                        }}
    };

    auto updateList = [listWidget, categoryItems](const QString &selectedCategory) {
        listWidget->clear();
        if (categoryItems.contains(selectedCategory)) {
            for (const auto &item : categoryItems[selectedCategory]) {
                QListWidgetItem *listItem = new QListWidgetItem(QIcon(item.first), item.second);
                listItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
                listItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
                listWidget->addItem(listItem);
            }
        }
    };

    updateList(categoryDropdown->currentText());

    layout->addWidget(categoryDropdown);
    layout->addWidget(listWidget);
    sidebarWidget->setLayout(layout);
    sidebar->setWidget(sidebarWidget);
    addDockWidget(Qt::LeftDockWidgetArea, sidebar);

    connect(categoryDropdown, &QComboBox::currentTextChanged, updateList);
    // connect(listWidget, &QListWidget::itemClicked, this, &MainWindow::handleItemSelection);
}


void MainWindow::addFurnitureItem(QListWidget *listWidget, const QString &imagePath, const QString &title) {
    QListWidgetItem *item = new QListWidgetItem(QIcon(imagePath), title);
    item->setSizeHint(QSize(100, 80));  // Adjust the item size
    listWidget->addItem(item);
}


void MainWindow::setRoomSize(int width, int height, int depth)
{
    // if (m_glWidget) {
    //     m_glWidget->setRoomSize(width, height, depth);
    // }
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
    emit homeWindowRequested();
    // QMessageBox::information(thsis, "Home", "Home button triggered.");
}

void MainWindow::exportAction() {
    QMessageBox::information(this, "Export", "Export action triggered.");
}

void MainWindow::zoomIn() {
    // m_glWidget->zoomIn();
}

void MainWindow::zoomOut() {
    // m_glWidget->zoomOut();
}
