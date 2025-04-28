#include "materialoptimizerwidget.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

MaterialOptimizerWidget::MaterialOptimizerWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Material Cutting Optimization");

    setupUI();
}

void MaterialOptimizerWidget::setupUI()
{
    tabWidget = new QTabWidget(this);
    tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    inputPanel = new QWidget;
    QVBoxLayout *inputLayout = new QVBoxLayout(inputPanel);
    inputLayout->setContentsMargins(5, 5, 5, 5);
    inputLayout->setSpacing(10);

    QGroupBox *partsGroup = new QGroupBox("Furniture Parts", this);
    QVBoxLayout *partsLayout = new QVBoxLayout(partsGroup);

    partsTable = new QTableWidget(this);
    partsTable->setColumnCount(3);
    partsTable->setHorizontalHeaderLabels({"Part name", "Width (mm)", "Height (mm)"});
    partsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    partsTable->verticalHeader()->setVisible(false);
    partsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    partsTable->setEditTriggers(QAbstractItemView::DoubleClicked);

    QScrollArea *partsScroll = new QScrollArea;
    partsScroll->setWidget(partsTable);
    partsScroll->setWidgetResizable(true);
    partsLayout->addWidget(partsScroll);

    QGroupBox *materialGroup = new QGroupBox("Material Sheet Size", this);
    QHBoxLayout *materialLayout = new QHBoxLayout(materialGroup);

    materialWidthSpin = new QDoubleSpinBox;
    materialWidthSpin->setRange(100, 5000);
    materialWidthSpin->setValue(materialWidth);
    materialWidthSpin->setSuffix(" mm");
    materialWidthSpin->setSingleStep(10);

    materialHeightSpin = new QDoubleSpinBox;
    materialHeightSpin->setRange(100, 5000);
    materialHeightSpin->setValue(materialHeight);
    materialHeightSpin->setSuffix(" mm");
    materialHeightSpin->setSingleStep(10);

    materialLayout->addWidget(new QLabel("Width:"));
    materialLayout->addWidget(materialWidthSpin);
    materialLayout->addSpacing(10);
    materialLayout->addWidget(new QLabel("Height:"));
    materialLayout->addWidget(materialHeightSpin);

    calculateButton = new QPushButton("Calculate Optimal Cuts", this);
    calculateButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    inputLayout->addWidget(partsGroup);
    inputLayout->addWidget(materialGroup);
    inputLayout->addWidget(calculateButton);
    inputLayout->addStretch();

    connect(calculateButton, &QPushButton::clicked, this, &MaterialOptimizerWidget::calculateOptimization);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(inputPanel);
    splitter->addWidget(tabWidget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setHandleWidth(5);
    splitter->setChildrenCollapsible(false);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(splitter);

    inputPanel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    partsTable->setMinimumWidth(350);
    inputPanel->setMinimumWidth(partsTable->minimumWidth() + 40);
}

void MaterialOptimizerWidget::setFurnitureParts(const QList<Part>& parts)
{
    partsTable->setRowCount(parts.size());

    for (int i = 0; i < parts.size(); ++i) {
        partsTable->setItem(i, 0, new QTableWidgetItem(parts[i].name));
        partsTable->setItem(i, 1, new QTableWidgetItem(QString::number(parts[i].width)));
        partsTable->setItem(i, 2, new QTableWidgetItem(QString::number(parts[i].height)));
    }
}

void MaterialOptimizerWidget::setMaterialSize(float width, float height)
{
    materialWidth = width;
    materialHeight = height;
    materialWidthSpin->setValue(width);
    materialHeightSpin->setValue(height);
}

void MaterialOptimizerWidget::calculateOptimization()
{
    materialWidth = materialWidthSpin->value();
    materialHeight = materialHeightSpin->value();

    QList<Part> parts;
    for (int i = 0; i < partsTable->rowCount(); ++i) {
        Part part;

        part.name = partsTable->item(i, 0)->text();
        part.width = partsTable->item(i, 1)->text().toFloat();
        part.height = partsTable->item(i, 2)->text().toFloat();

        parts.append(part);
    }

    std::sort(parts.begin(), parts.end(), [](const Part& a, const Part& b) {
        return (a.width * a.height) > (b.width * b.height);
    });

    QList<QList<Part>> sheets;
    QList<QList<QRectF>> sheetPlacements;
    QList<QList<QRectF>> sheetSurplusAreas;

    sheets.append(QList<Part>());
    sheetPlacements.append(QList<QRectF>());
    sheetSurplusAreas.append(QList<QRectF>());
    sheetSurplusAreas.last().append(QRectF(0, 0, materialWidth, materialHeight));

    for (const auto& part : parts) {
        bool placed = false;

        for (int rotation = 0; rotation < 2 && !placed; rotation++) {
            float w = rotation ? part.height : part.width;
            float h = rotation ? part.width : part.height;

            for (int sheetIdx = 0; sheetIdx < sheets.size() && !placed; ++sheetIdx) {
                for (int areaIdx = 0; areaIdx < sheetSurplusAreas[sheetIdx].size() && !placed; ++areaIdx) {
                    QRectF area = sheetSurplusAreas[sheetIdx][areaIdx];
                    if (w <= area.width() && h <= area.height()) {
                        QRectF placement(area.x(), area.y(), w, h);
                        sheetPlacements[sheetIdx].append(placement);

                        Part rotatedPart = part;
                        if (rotation) {
                            rotatedPart.width = part.height;
                            rotatedPart.height = part.width;
                        }
                        sheets[sheetIdx].append(rotatedPart);

                        sheetSurplusAreas[sheetIdx].removeAt(areaIdx);

                        if (area.width() > w) {
                            sheetSurplusAreas[sheetIdx].append(
                                QRectF(area.x() + w, area.y(), area.width() - w, h));
                        }

                        if (area.height() > h) {
                            sheetSurplusAreas[sheetIdx].append(
                                QRectF(area.x(), area.y() + h, area.width(), area.height() - h));
                        }

                        placed = true;
                    }
                }
            }

            if (!placed && w <= materialWidth && h <= materialHeight) {
                sheets.append(QList<Part>());
                sheetPlacements.append(QList<QRectF>());
                sheetSurplusAreas.append(QList<QRectF>());

                QRectF placement(0, 0, w, h);
                sheetPlacements.last().append(placement);

                Part rotatedPart = part;
                if (rotation) {
                    rotatedPart.width = part.height;
                    rotatedPart.height = part.width;
                }
                sheets.last().append(rotatedPart);

                if (materialWidth > w) {
                    sheetSurplusAreas.last().append(
                        QRectF(w, 0, materialWidth - w, h));
                }

                if (materialHeight > h) {
                    sheetSurplusAreas.last().append(QRectF(0, h, materialWidth, materialHeight - h));
                }

                placed = true;
            }
        }
    }

    QTabWidget *tabWidget = findChild<QTabWidget*>();
    if (!tabWidget) return;

    while (tabWidget->count() > 0) {
        QWidget* widget = tabWidget->widget(0);
        tabWidget->removeTab(0);
        delete widget;
    }


    for (int sheetIdx = 0; sheetIdx < sheets.size(); sheetIdx++) {
        QGraphicsScene *scene = new QGraphicsScene;
        QGraphicsView *view = new QGraphicsView(scene);
        view->setRenderHint(QPainter::Antialiasing);

        QGraphicsRectItem *sheet = new QGraphicsRectItem(0, 0, materialWidth, materialHeight);
        sheet->setBrush(LaminateUtils::createLaminatePattern(materialWidth, materialHeight));
        sheet->setPen(QPen(Qt::black, 2));
        scene->addItem(sheet);

        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
        shadow->setBlurRadius(15);
        shadow->setOffset(5, 5);
        shadow->setColor(QColor(0, 0, 0, 100));
        sheet->setGraphicsEffect(shadow);

        for (int partIdx = 0; partIdx < sheets[sheetIdx].size(); partIdx++) {
            const QRectF &placement = sheetPlacements[sheetIdx][partIdx];
            const Part &part = sheets[sheetIdx][partIdx];

            QGraphicsRectItem *partRect = new QGraphicsRectItem(placement);
            partRect->setBrush(QBrush(QColor(173, 216, 230, 150)));
            partRect->setPen(QPen(Qt::blue, 1));
            scene->addItem(partRect);

            QGraphicsTextItem *label = new QGraphicsTextItem(part.name);
            label->setPos(placement.x() + 5, placement.y() + 5);
            label->setDefaultTextColor(Qt::black);
            QFont font = label->font();
            font.setPointSize(80);
            label->setFont(font);
            scene->addItem(label);

            QPen cutPen(Qt::red, 1, Qt::DashLine);
            float cutMargin = 10.0f;

            scene->addLine(placement.x() - cutMargin, placement.y() - cutMargin,
                           placement.x() + placement.width() + cutMargin, placement.y() - cutMargin, cutPen);
            scene->addLine(placement.x() - cutMargin, placement.y() + placement.height() + cutMargin,
                           placement.x() + placement.width() + cutMargin, placement.y() + placement.height() + cutMargin, cutPen);
            scene->addLine(placement.x() - cutMargin, placement.y() - cutMargin,
                           placement.x() - cutMargin, placement.y() + placement.height() + cutMargin, cutPen);
            scene->addLine(placement.x() + placement.width() + cutMargin, placement.y() - cutMargin,
                           placement.x() + placement.width() + cutMargin, placement.y() + placement.height() + cutMargin, cutPen);
        }

        if (auto splitter = findChild<QSplitter*>()) {
            splitter->setSizes({inputPanel->sizeHint().width(), width() - inputPanel->sizeHint().width()});
        }

        scene->setSceneRect(0, 0, materialWidth, materialHeight);
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

        tabWidget->addTab(view, QString("Sheet %1 (%2 parts)").arg(sheetIdx + 1).arg(sheets[sheetIdx].size()));
    }

    if (auto splitter = findChild<QSplitter*>()) {
        splitter->setSizes({inputPanel->sizeHint().width(), width() - inputPanel->sizeHint().width()});
    }
}

bool MaterialOptimizerWidget::tryPlacePart(QList<QPair<float, float>>& sheet, QList<QRectF>& usedAreas, const QPair<float, float>& part, float sheetWidth, float sheetHeight)
{
    float x = 0;
    float y = 0;

    while (y + part.second <= sheetHeight) {
        while (x + part.first <= sheetWidth) {
            QRectF candidateRect(x, y, part.first, part.second);

            bool overlaps = false;
            for (const QRectF& used : usedAreas) {
                if (used.intersects(candidateRect)) {
                    overlaps = true;
                    break;
                }
            }

            if (!overlaps) {
                usedAreas.append(candidateRect);
                sheet.append(part);
                return true;
            }

            x += 10;
        }
        x = 0;
        y += 10;
    }

    return false;
}

QRectF MaterialOptimizerWidget::findBestPlacement(const QList<QRectF>& usedAreas, float partWidth, float partHeight) const
{
    QRectF bestPlacement;
    float bestWaste = std::numeric_limits<float>::max();

    for (float y = 0; y <= materialHeight - partHeight; y += 1.0f) {
        for (float x = 0; x <= materialWidth - partWidth; x += 1.0f) {
            QRectF candidate(x, y, partWidth, partHeight);

            bool overlaps = false;
            for (const QRectF& used : usedAreas) {
                if (used.intersects(candidate)) {
                    overlaps = true;
                    break;
                }
            }

            if (!overlaps) {
                float waste = calculateWaste(candidate, partWidth, partHeight);
                if (waste < bestWaste) {
                    bestWaste = waste;
                    bestPlacement = candidate;
                }
            }
        }
    }

    return bestPlacement;
}

float MaterialOptimizerWidget::calculateWaste(const QRectF& freeRect, float partWidth, float partHeight) const
{
    float remainingWidth = materialWidth - freeRect.right();
    float remainingHeight = materialHeight - freeRect.top();
    return remainingWidth * remainingHeight;
}
