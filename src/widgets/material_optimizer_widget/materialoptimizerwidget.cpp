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
    setMinimumSize(1200, 800);
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

void MaterialOptimizerWidget::calculateOptimization() {
    materialWidth = materialWidthSpin->value();
    materialHeight = materialHeightSpin->value();

    QList<QPair<float, float>> parts;
    for (int i = 0; i < partsTable->rowCount(); ++i) {
        float width = partsTable->item(i, 1)->text().toFloat();
        float height = partsTable->item(i, 2)->text().toFloat();
        parts.append({width, height});
    }

    // Try all algorithms
    auto algorithms = {
        MaterialOptimizerWidget::FirstFitDecreasing,
        MaterialOptimizerWidget::Guillotine,
        MaterialOptimizerWidget::Skyline,
        MaterialOptimizerWidget::BestFitDecreasing,
        MaterialOptimizerWidget::BottomLeft,
    };

    auto results = MaterialOptimizerWidget::optimizeCutting(parts, materialWidth, materialHeight, algorithms);
    if (results.isEmpty()) {
        QMessageBox::warning(this, "Optimization", "No valid cutting plan could be generated.");
        return;
    }

    // Display all results in tabs
    QTabWidget *tabWidget = findChild<QTabWidget*>();
    if (!tabWidget) return;

    while (tabWidget->count() > 0) {
        QWidget* widget = tabWidget->widget(0);
        tabWidget->removeTab(0);
        delete widget;
    }

    for (const auto& result : results) {
        // Create a container widget for this algorithm's results
        QWidget *algorithmTab = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(algorithmTab);

        // Add label showing algorithm name and summary
        QString algorithmName;
        switch (result.algorithm) {
        case MaterialOptimizerWidget::FirstFitDecreasing: algorithmName = "First-Fit Decreasing"; break;
        case MaterialOptimizerWidget::Guillotine: algorithmName = "Guillotine"; break;
        case MaterialOptimizerWidget::Skyline: algorithmName = "Skyline"; break;
        case MaterialOptimizerWidget::BestFitDecreasing: algorithmName = "Best-Fit Decreasing"; break;
        case MaterialOptimizerWidget::BottomLeft: algorithmName = "Bottom-left"; break;
        default: algorithmName = "Unknown";
        }

        QLabel *summaryLabel = new QLabel(
            QString("%1 - %2 sheets used (%3% utilization)")
                .arg(algorithmName)
                .arg(result.sheetsUsed)
                .arg(result.utilization, 0, 'f', 1));
        summaryLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");
        layout->addWidget(summaryLabel);

        // Create a scroll area for the sheets
        QScrollArea *scrollArea = new QScrollArea();
        QWidget *sheetsContainer = new QWidget();
        QHBoxLayout *sheetsLayout = new QHBoxLayout(sheetsContainer);
        sheetsLayout->setContentsMargins(10, 10, 10, 10);
        sheetsLayout->setSpacing(20);

        // Add preview for each sheet
        for (int sheetIndex = 0; sheetIndex < result.placements.size(); ++sheetIndex) {
            QGroupBox *sheetGroup = new QGroupBox(QString("Sheet %1").arg(sheetIndex + 1));
            QVBoxLayout *sheetLayout = new QVBoxLayout(sheetGroup);

            CuttingPreviewWidget* preview = new CuttingPreviewWidget();
            preview->setSheetSize(materialWidth, materialHeight);
            preview->setParts(result.placements[sheetIndex]);

            // Add part count label
            QLabel *partCountLabel = new QLabel(
                QString("%1 parts").arg(result.placements[sheetIndex].size()));
            partCountLabel->setAlignment(Qt::AlignCenter);

            sheetLayout->addWidget(preview);
            sheetLayout->addWidget(partCountLabel);
            sheetsLayout->addWidget(sheetGroup);
        }

        sheetsContainer->setLayout(sheetsLayout);
        scrollArea->setWidget(sheetsContainer);
        scrollArea->setWidgetResizable(true);
        layout->addWidget(scrollArea);

        tabWidget->addTab(algorithmTab, algorithmName);
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

// first-fit decreasing
MaterialOptimizerWidget::Result MaterialOptimizerWidget::firstFitDecreasing(const QList<QPair<float, float>>& parts, float sheetWidth, float sheetHeight) {
    Result result;
    result.algorithm = FirstFitDecreasing;

    // Sort parts by area descending
    auto sortedParts = parts;
    std::sort(sortedParts.begin(), sortedParts.end(), [](const auto& a, const auto& b) {
        return (a.first * a.second) > (b.first * b.second);
    });

    QList<QList<QRectF>> sheets;
    QList<QList<QRectF>> freeAreas;

    for (const auto& part : sortedParts) {
        bool placed = false;

        // Try all sheets
        for (int i = 0; i < sheets.size() && !placed; ++i) {
            // Try all free areas in sheet
            for (int j = 0; j < freeAreas[i].size() && !placed; ++j) {
                if (part.first <= freeAreas[i][j].width() &&
                    part.second <= freeAreas[i][j].height()) {
                    const QRectF freeArea = freeAreas[i][j];  // Copy before removal
                    freeAreas[i].removeAt(j);

                    QRectF placement(freeArea.x(), freeArea.y(), part.first, part.second);
                    sheets[i].append(placement);

                    if (freeArea.width() > part.first) {
                        freeAreas[i].append(QRectF(
                            placement.right(), freeArea.y(),
                            freeArea.width() - part.first, part.second
                            ));
                    }
                    if (freeArea.height() > part.second) {
                        freeAreas[i].append(QRectF(
                            freeArea.x(), placement.bottom(),
                            part.first, freeArea.height() - part.second
                            ));
                    }
                    placed = true;
                }
            }
        }

        // If not placed, create new sheet
        if (!placed && part.first <= sheetWidth && part.second <= sheetHeight) {
            sheets.append(QList<QRectF>());
            freeAreas.append(QList<QRectF>());

            QRectF placement(0, 0, part.first, part.second);
            sheets.last().append(placement);

            if (sheetWidth > part.first) {
                freeAreas.last().append(QRectF(
                    part.first, 0, sheetWidth - part.first, part.second
                    ));
            }
            if (sheetHeight > part.second) {
                freeAreas.last().append(QRectF(
                    0, part.second, part.first, sheetHeight - part.second
                    ));
            }
        }
    }

    result.placements = sheets;
    result.sheetsUsed = sheets.size();
    calculateMetrics(result, sheetWidth, sheetHeight);
    return result;
}

// guillotine algorithm
MaterialOptimizerWidget::Result MaterialOptimizerWidget::guillotineCut(
    const QList<QPair<float, float>>& parts,
    float sheetWidth,
    float sheetHeight)
{
    Result result;
    result.algorithm = Guillotine;

    // 1) sort parts by their longest side (descending)
    auto sortedParts = parts;
    std::sort(sortedParts.begin(), sortedParts.end(),
              [](const auto& a, const auto& b) {
                  return std::max(a.first, a.second)
                  > std::max(b.first, b.second);
              });

    // 2) prepare storage: one list of placed rects per sheet,
    //    and one list of free rects per sheet
    QList<QList<QRectF>> sheets;
    QList<QList<QRectF>> freeRects;

    // 3) start with a single new sheet whose only free rect is the entire sheet
    sheets.append(QList<QRectF>());
    freeRects.append(QList<QRectF>{ QRectF(0, 0, sheetWidth, sheetHeight) });

    // 4) for each part, try to place it
    for (const auto& part : sortedParts) {
        bool placed = false;

        // try every existing sheet
        for (int i = 0; i < sheets.size() && !placed; ++i) {
            int bestIdx = -1;
            float bestFit = std::numeric_limits<float>::max();

            // find the free rect that leaves the least leftover area
            for (int j = 0; j < freeRects[i].size(); ++j) {
                const QRectF& r = freeRects[i][j];
                if (part.first <= r.width() && part.second <= r.height()) {
                    float leftover = r.width()*r.height()
                    - part.first*part.second;
                    if (leftover < bestFit) {
                        bestFit = leftover;
                        bestIdx = j;
                    }
                }
            }

            // if we found a spot, place & split
            if (bestIdx != -1) {
                const QRectF chosen = freeRects[i][bestIdx];
                QRectF placement(chosen.x(), chosen.y(),
                                 part.first, part.second);
                sheets[i].append(placement);
                freeRects[i].removeAt(bestIdx);

                float w = chosen.width()  - part.first;
                float h = chosen.height() - part.second;

                // guillotine split: prefer the larger leftover
                if (w >= h) {
                    if (w > 0)
                        freeRects[i].append(
                            QRectF(placement.right(), chosen.y(),
                                   w, chosen.height()));
                    if (h > 0)
                        freeRects[i].append(
                            QRectF(chosen.x(), placement.bottom(),
                                   part.first, h));
                } else {
                    if (h > 0)
                        freeRects[i].append(
                            QRectF(chosen.x(), placement.bottom(),
                                   chosen.width(), h));
                    if (w > 0)
                        freeRects[i].append(
                            QRectF(placement.right(), chosen.y(),
                                   w, part.second));
                }
                placed = true;
            }
        }

        // if it didn't fit anywhere, create a new sheet (seeded with full-sheet rect)
        if (!placed && part.first <= sheetWidth && part.second <= sheetHeight) {
            sheets.append(QList<QRectF>());
            freeRects.append(QList<QRectF>{ QRectF(0, 0, sheetWidth, sheetHeight) });
            int i = sheets.size() - 1;

            // now place into that new sheet exactly as above:
            const QRectF chosen = freeRects[i][0];
            QRectF placement(chosen.x(), chosen.y(),
                             part.first, part.second);
            sheets[i].append(placement);
            freeRects[i].removeAt(0);

            float w = chosen.width()  - part.first;
            float h = chosen.height() - part.second;

            if (w >= h) {
                if (w > 0)
                    freeRects[i].append(
                        QRectF(placement.right(), chosen.y(),
                               w, chosen.height()));
                if (h > 0)
                    freeRects[i].append(
                        QRectF(chosen.x(), placement.bottom(),
                               part.first, h));
            } else {
                if (h > 0)
                    freeRects[i].append(
                        QRectF(chosen.x(), placement.bottom(),
                               chosen.width(), h));
                if (w > 0)
                    freeRects[i].append(
                        QRectF(placement.right(), chosen.y(),
                               w, part.second));
            }
        }
    }

    // 5) finalize
    result.placements = sheets;
    result.sheetsUsed  = sheets.size();
    calculateMetrics(result, sheetWidth, sheetHeight);
    return result;
}

// bottom-left algorithm
MaterialOptimizerWidget::Result MaterialOptimizerWidget::bottomLeftAlgorithm(
    const QList<QPair<float, float>>& parts,
    float sheetWidth, float sheetHeight)
{
    Result result;
    result.algorithm = BottomLeft;

    // sort parts largest-first
    auto sortedParts = parts;
    std::sort(sortedParts.begin(), sortedParts.end(),
              [](auto &a, auto &b) {
                  return (a.first * a.second) > (b.first * b.second);
              });

    QList<QList<QRectF>> sheets;
    QList<QList<QPointF>> usedPoints;

    for (auto &part : sortedParts) {
        bool placed = false;

        // try to fit into an existing sheet
        for (int i = 0; i < sheets.size() && !placed; ++i) {
            float bestX = 0, bestY = 0;
            bool foundPosition = false;

            // scan each candidate “corner” (usedPoints) for a legal, non-colliding placement
            for (auto &pt : usedPoints[i]) {
                float x = pt.x(), y = pt.y();

                // must fit within sheet bounds
                if (x + part.first <= sheetWidth &&
                    y + part.second <= sheetHeight)
                {
                    QRectF candidate(x, y, part.first, part.second);
                    // check collision against already-placed rectangles
                    bool collision = false;
                    for (auto &r : sheets[i]) {
                        if (r.intersects(candidate)) {
                            collision = true;
                            break;
                        }
                    }
                    if (!collision) {
                        // pick the lowest y (then lowest x) to implement BL heuristic
                        if (!foundPosition ||
                            y < bestY ||
                            (y == bestY && x < bestX))
                        {
                            bestX = x;
                            bestY = y;
                            foundPosition = true;
                        }
                    }
                }
            }

            // if we found at least one valid spot, place it
            if (foundPosition) {
                QRectF placement(bestX, bestY, part.first, part.second);
                sheets[i].append(placement);
                usedPoints[i].append({ bestX + part.first, bestY });
                usedPoints[i].append({ bestX, bestY + part.second });
                placed = true;
            }
        }

        // if not placed anywhere, start a new sheet at (0,0)
        if (!placed && part.first <= sheetWidth && part.second <= sheetHeight) {
            sheets.append(QList<QRectF>());
            usedPoints.append(QList<QPointF>());

            QRectF placement(0, 0, part.first, part.second);
            sheets.last().append(placement);
            usedPoints.last().append({ part.first, 0 });
            usedPoints.last().append({ 0, part.second });
        }
    }

    result.placements = sheets;
    result.sheetsUsed  = sheets.size();
    calculateMetrics(result, sheetWidth, sheetHeight);
    return result;
}

// skyline algorithm
MaterialOptimizerWidget::Result MaterialOptimizerWidget::skylineAlgorithm(const QList<QPair<float, float>>& parts, float sheetWidth, float sheetHeight) {
    Result result;
    result.algorithm = Skyline;

    auto sortedParts = parts;
    std::sort(sortedParts.begin(), sortedParts.end(), [](const auto& a, const auto& b) {
        return a.second > b.second; // Sort by height descending
    });

    QList<QList<QRectF>> sheets;
    QList<QList<QPair<float, float>>> skylines; // (x_start, height)

    for (const auto& part : sortedParts) {
        bool placed = false;

        for (int i = 0; i < sheets.size() && !placed; ++i) {
            // Find best position in skyline
            float bestX = 0;
            float bestY = 0;
            float bestWaste = std::numeric_limits<float>::max();

            for (int j = 0; j < skylines[i].size(); ++j) {
                float x = skylines[i][j].first;
                float y = skylines[i][j].second;

                // Check if part fits here
                if (x + part.first <= sheetWidth) {
                    // Find minimum height in this segment
                    float minHeight = y;
                    for (int k = j + 1; k < skylines[i].size() &&
                                        skylines[i][k].first < x + part.first; k++) {
                        minHeight = std::max(minHeight, skylines[i][k].second);
                    }

                    if (minHeight + part.second <= sheetHeight) {
                        float waste = (x + part.first) * (minHeight + part.second);
                        if (waste < bestWaste) {
                            bestWaste = waste;
                            bestX = x;
                            bestY = minHeight;
                        }
                    }
                }
            }

            if (bestWaste < std::numeric_limits<float>::max()) {
                // Place the part
                QRectF placement(bestX, bestY, part.first, part.second);
                sheets[i].append(placement);

                // Update skyline
                QList<QPair<float, float>> newSkyline;
                bool partAdded = false;

                for (int j = 0; j < skylines[i].size(); j++) {
                    float x = skylines[i][j].first;
                    float h = skylines[i][j].second;

                    if (x < bestX) {
                        newSkyline.append({x, h});
                    } else if (!partAdded) {
                        newSkyline.append({bestX, bestY + part.second});
                        if (bestX + part.first < x) {
                            newSkyline.append({bestX + part.first, bestY});
                        }
                        partAdded = true;
                    } else if (x > bestX + part.first) {
                        newSkyline.append({x, h});
                    }
                }

                if (!partAdded) {
                    newSkyline.append({bestX, bestY + part.second});
                    newSkyline.append({bestX + part.first, bestY});
                }

                skylines[i] = newSkyline;
                placed = true;
            }
        }

        if (!placed && part.first <= sheetWidth && part.second <= sheetHeight) {
            sheets.append(QList<QRectF>());
            skylines.append(QList<QPair<float, float>>());

            QRectF placement(0, 0, part.first, part.second);
            sheets.last().append(placement);

            skylines.last().append({0, part.second});
            skylines.last().append({part.first, 0});
        }
    }

    result.placements = sheets;
    result.sheetsUsed = sheets.size();
    calculateMetrics(result, sheetWidth, sheetHeight);
    return result;
}

MaterialOptimizerWidget::Result MaterialOptimizerWidget::bestFitDecreasing(
    const QList<QPair<float, float>>& parts,
    float sheetWidth,
    float sheetHeight)
{
    Result result;
    result.algorithm = BestFitDecreasing;

    // 1) Sort by area (desc), tiebreak on longer side
    auto sortedParts = parts;
    std::sort(sortedParts.begin(), sortedParts.end(),
              [](auto &a, auto &b) {
                  float areaA = a.first * a.second;
                  float areaB = b.first * b.second;
                  if (qFuzzyCompare(areaA, areaB)) {
                      return std::max(a.first, a.second) > std::max(b.first, b.second);
                  }
                  return areaA > areaB;
              });

    // 2) Prepare storage & seed the first sheet
    QList<QList<QRectF>> sheets;
    QList<QList<QRectF>> freeRects;
    sheets.append(QList<QRectF>());
    freeRects.append({ QRectF(0, 0, sheetWidth, sheetHeight) });

    // 3) Place each part
    for (auto &part : sortedParts) {
        bool placed = false;
        int bestSheet = -1, bestRect = -1;
        float bestWaste = std::numeric_limits<float>::max();
        bool bestRot = false;
        float bestPW=0, bestPH=0;

        // 3a) scan both orientations for best waste across all free rects
        for (int ori = 0; ori < 2; ++ori) {
            float pw = ori ? part.second : part.first;
            float ph = ori ? part.first  : part.second;
            if (pw > sheetWidth || ph > sheetHeight) continue;

            for (int s = 0; s < sheets.size(); ++s) {
                for (int r = 0; r < freeRects[s].size(); ++r) {
                    const QRectF &cell = freeRects[s][r];
                    if (pw <= cell.width() && ph <= cell.height()) {
                        float waste = cell.width()*cell.height() - pw*ph;
                        if (waste < bestWaste) {
                            bestWaste = waste;
                            bestSheet = s;
                            bestRect  = r;
                            bestRot   = (ori == 1);
                            bestPW    = pw;
                            bestPH    = ph;
                        }
                    }
                }
            }
        }

        // 3b) if we found a free cell, place + split it
        if (bestSheet != -1) {
            QRectF cell = freeRects[bestSheet][bestRect];
            freeRects[bestSheet].removeAt(bestRect);

            // record placement
            QRectF placement(cell.x(), cell.y(), bestPW, bestPH);
            sheets[bestSheet].append(placement);

            // split into RIGHT (width leftover × partHeight) …
            float remW = cell.width()  - bestPW;
            if (remW > 0) {
                freeRects[bestSheet].append(
                    QRectF(cell.x() + bestPW,
                           cell.y(),
                           remW,
                           bestPH)
                    );
            }
            // … and BELOW (full cell width × height leftover)
            float remH = cell.height() - bestPH;
            if (remH > 0) {
                freeRects[bestSheet].append(
                    QRectF(cell.x(),
                           cell.y() + bestPH,
                           cell.width(),
                           remH)
                    );
            }

            placed = true;
        }

        // 3c) otherwise spin up a brand-new sheet (seeded the same way)
        if (!placed) {
            sheets.append(QList<QRectF>());
            freeRects.append({ QRectF(0, 0, sheetWidth, sheetHeight) });
            int s = sheets.size() - 1;

            // place into that fresh sheet
            QRectF cell = freeRects[s][0];
            freeRects[s].removeAt(0);

            // reuse the same orientation logic from above—
            // find which orientation actually fits here
            bool placedHere = false;
            for (int ori = 0; ori < 2 && !placedHere; ++ori) {
                float pw = ori ? part.second : part.first;
                float ph = ori ? part.first  : part.second;
                if (pw <= sheetWidth && ph <= sheetHeight) {
                    QRectF placement(cell.x(), cell.y(), pw, ph);
                    sheets[s].append(placement);

                    float remW = cell.width()  - pw;
                    if (remW > 0) {
                        freeRects[s].append(
                            QRectF(cell.x() + pw, cell.y(), remW, ph)
                            );
                    }
                    float remH = cell.height() - ph;
                    if (remH > 0) {
                        freeRects[s].append(
                            QRectF(cell.x(), cell.y() + ph, cell.width(), remH)
                            );
                    }
                    placedHere = true;
                }
            }
        }
    }

    // 4) done
    result.placements = sheets;
    result.sheetsUsed  = sheets.size();
    calculateMetrics(result, sheetWidth, sheetHeight);
    return result;
}

QList<MaterialOptimizerWidget::Result> MaterialOptimizerWidget::optimizeCutting(const QList<QPair<float, float>>& parts, float sheetWidth, float sheetHeight, const QList<Algorithm>& algorithms) {
    QList<Result> results;

    for (auto algorithm : algorithms) {
        Result result;
        switch (algorithm) {
        case FirstFitDecreasing:
            result = firstFitDecreasing(parts, sheetWidth, sheetHeight);
            break;
        case BestFitDecreasing:
            result = bestFitDecreasing(parts, sheetWidth, sheetHeight);
            break;
        case BottomLeft:
            result = bottomLeftAlgorithm(parts, sheetWidth, sheetHeight);
            break;
        case Guillotine:
            result = guillotineCut(parts, sheetWidth, sheetHeight);
            break;
        case Skyline:
            result = skylineAlgorithm(parts, sheetWidth, sheetHeight);
            break;
        }
        results.append(result);
    }

    // Sort by best utilization
    std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
        return a.utilization > b.utilization;
    });

    return results;
}

void MaterialOptimizerWidget::calculateMetrics(Result &result, float sheetWidth, float sheetHeight) {
    float totalArea = sheetWidth * sheetHeight;
    float usedArea = 0.0f;

    for (const auto& sheet : result.placements) {
        for (const auto& placement : sheet) {
            usedArea += placement.width() * placement.height();
        }
    }

    result.utilization = (usedArea / (totalArea * result.sheetsUsed)) * 100.0f;
    result.waste = 100.0f - result.utilization;
}


// void MaterialOptimizerWidget::calculateOptimization()
// {
//     materialWidth = materialWidthSpin->value();
//     materialHeight = materialHeightSpin->value();

//     QList<Part> parts;
//     for (int i = 0; i < partsTable->rowCount(); ++i) {
//         Part part;

//         part.name = partsTable->item(i, 0)->text();
//         part.width = partsTable->item(i, 1)->text().toFloat();
//         part.height = partsTable->item(i, 2)->text().toFloat();

//         parts.append(part);
//     }

//     std::sort(parts.begin(), parts.end(), [](const Part& a, const Part& b) {
//         return (a.width * a.height) > (b.width * b.height);
//     });

//     QList<QList<Part>> sheets;
//     QList<QList<QRectF>> sheetPlacements;
//     QList<QList<QRectF>> sheetSurplusAreas;

//     sheets.append(QList<Part>());
//     sheetPlacements.append(QList<QRectF>());
//     sheetSurplusAreas.append(QList<QRectF>());
//     sheetSurplusAreas.last().append(QRectF(0, 0, materialWidth, materialHeight));

//     for (const auto& part : parts) {
//         bool placed = false;

//         for (int rotation = 0; rotation < 2 && !placed; rotation++) {
//             float w = rotation ? part.height : part.width;
//             float h = rotation ? part.width : part.height;

//             for (int sheetIdx = 0; sheetIdx < sheets.size() && !placed; ++sheetIdx) {
//                 for (int areaIdx = 0; areaIdx < sheetSurplusAreas[sheetIdx].size() && !placed; ++areaIdx) {
//                     QRectF area = sheetSurplusAreas[sheetIdx][areaIdx];
//                     if (w <= area.width() && h <= area.height()) {
//                         QRectF placement(area.x(), area.y(), w, h);
//                         sheetPlacements[sheetIdx].append(placement);

//                         Part rotatedPart = part;
//                         if (rotation) {
//                             rotatedPart.width = part.height;
//                             rotatedPart.height = part.width;
//                         }
//                         sheets[sheetIdx].append(rotatedPart);

//                         sheetSurplusAreas[sheetIdx].removeAt(areaIdx);

//                         if (area.width() > w) {
//                             sheetSurplusAreas[sheetIdx].append(
//                                 QRectF(area.x() + w, area.y(), area.width() - w, h));
//                         }

//                         if (area.height() > h) {
//                             sheetSurplusAreas[sheetIdx].append(
//                                 QRectF(area.x(), area.y() + h, area.width(), area.height() - h));
//                         }

//                         placed = true;
//                     }
//                 }
//             }

//             if (!placed && w <= materialWidth && h <= materialHeight) {
//                 sheets.append(QList<Part>());
//                 sheetPlacements.append(QList<QRectF>());
//                 sheetSurplusAreas.append(QList<QRectF>());

//                 QRectF placement(0, 0, w, h);
//                 sheetPlacements.last().append(placement);

//                 Part rotatedPart = part;
//                 if (rotation) {
//                     rotatedPart.width = part.height;
//                     rotatedPart.height = part.width;
//                 }
//                 sheets.last().append(rotatedPart);

//                 if (materialWidth > w) {
//                     sheetSurplusAreas.last().append(
//                         QRectF(w, 0, materialWidth - w, h));
//                 }

//                 if (materialHeight > h) {
//                     sheetSurplusAreas.last().append(QRectF(0, h, materialWidth, materialHeight - h));
//                 }

//                 placed = true;
//             }
//         }
//     }

//     QTabWidget *tabWidget = findChild<QTabWidget*>();
//     if (!tabWidget) return;

//     while (tabWidget->count() > 0) {
//         QWidget* widget = tabWidget->widget(0);
//         tabWidget->removeTab(0);
//         delete widget;
//     }


//     for (int sheetIdx = 0; sheetIdx < sheets.size(); sheetIdx++) {
//         QGraphicsScene *scene = new QGraphicsScene;
//         QGraphicsView *view = new QGraphicsView(scene);
//         view->setRenderHint(QPainter::Antialiasing);

//         QGraphicsRectItem *sheet = new QGraphicsRectItem(0, 0, materialWidth, materialHeight);
//         sheet->setBrush(LaminateUtils::createLaminatePattern(materialWidth, materialHeight));
//         sheet->setPen(QPen(Qt::black, 2));
//         scene->addItem(sheet);

//         QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
//         shadow->setBlurRadius(15);
//         shadow->setOffset(5, 5);
//         shadow->setColor(QColor(0, 0, 0, 100));
//         sheet->setGraphicsEffect(shadow);

//         for (int partIdx = 0; partIdx < sheets[sheetIdx].size(); partIdx++) {
//             const QRectF &placement = sheetPlacements[sheetIdx][partIdx];
//             const Part &part = sheets[sheetIdx][partIdx];

//             QGraphicsRectItem *partRect = new QGraphicsRectItem(placement);
//             partRect->setBrush(QBrush(QColor(173, 216, 230, 150)));
//             partRect->setPen(QPen(Qt::blue, 1));
//             scene->addItem(partRect);

//             QGraphicsTextItem *label = new QGraphicsTextItem(part.name);
//             label->setPos(placement.x() + 5, placement.y() + 5);
//             label->setDefaultTextColor(Qt::black);
//             QFont font = label->font();
//             font.setPointSize(80);
//             label->setFont(font);
//             scene->addItem(label);

//             QPen cutPen(Qt::red, 1, Qt::DashLine);
//             float cutMargin = 10.0f;

//             scene->addLine(placement.x() - cutMargin, placement.y() - cutMargin,
//                            placement.x() + placement.width() + cutMargin, placement.y() - cutMargin, cutPen);
//             scene->addLine(placement.x() - cutMargin, placement.y() + placement.height() + cutMargin,
//                            placement.x() + placement.width() + cutMargin, placement.y() + placement.height() + cutMargin, cutPen);
//             scene->addLine(placement.x() - cutMargin, placement.y() - cutMargin,
//                            placement.x() - cutMargin, placement.y() + placement.height() + cutMargin, cutPen);
//             scene->addLine(placement.x() + placement.width() + cutMargin, placement.y() - cutMargin,
//                            placement.x() + placement.width() + cutMargin, placement.y() + placement.height() + cutMargin, cutPen);
//         }

//         if (auto splitter = findChild<QSplitter*>()) {
//             splitter->setSizes({inputPanel->sizeHint().width(), width() - inputPanel->sizeHint().width()});
//         }

//         scene->setSceneRect(0, 0, materialWidth, materialHeight);
//         view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

//         tabWidget->addTab(view, QString("Sheet %1 (%2 parts)").arg(sheetIdx + 1).arg(sheets[sheetIdx].size()));
//     }

//     if (auto splitter = findChild<QSplitter*>()) {
//         splitter->setSizes({inputPanel->sizeHint().width(), width() - inputPanel->sizeHint().width()});
//     }
// }
