#ifndef MATERIALOPTIMIZERWIDGET_H
#define MATERIALOPTIMIZERWIDGET_H

#include "src/utils/laminate_utils.h"
#include "src/data_structures/structs/part_types/part_types.h"
#include "src/widgets/cutting_preview_widget/cuttingpreviewwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QTabWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QGraphicsDropShadowEffect>
#include <QSplitter>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>


class MaterialOptimizerWidget : public QWidget {
    Q_OBJECT

public:
    enum Algorithm {
        FirstFitDecreasing,
        BestFitDecreasing,
        BottomLeft,
        Guillotine,
        MaximalRectangles,
        Skyline
    };

    struct Result {
        QList<QList<QRectF>> placements;
        float utilization;
        float waste;
        Algorithm algorithm;
        int sheetsUsed;
    };

    QList<Result> optimizeCutting(const QList<QPair<float, float>>& parts, float sheetWidth, float sheetHeight, const QList<Algorithm>& algorithms);
    void calculateMetrics(Result &result, float sheetWidth, float sheetHeight);

    Result guillotineCut(const QList<QPair<float, float>>& parts, float sheetWidth, float sheetHeight);
    Result skylineAlgorithm(const QList<QPair<float, float>>& parts, float sheetWidth, float sheetHeight);
    Result bestFitDecreasing(const QList<QPair<float, float>>& parts, float sheetWidth, float sheetHeight);
    Result firstFitDecreasing(const QList<QPair<float, float>>& parts, float sheetWidth, float sheetHeight);
    Result bottomLeftAlgorithm(const QList<QPair<float, float>>& parts, float sheetWidth, float sheetHeight);

public:
    explicit MaterialOptimizerWidget(QWidget *parent = nullptr);

    void setFurnitureParts(const QList<Part>& parts);
    void setMaterialSize(float width, float height);
    bool tryPlacePart(QList<QPair<float, float>>& sheet, QList<QRectF>& usedAreas, const QPair<float, float>& part, float sheetWidth, float sheetHeight);

public slots:
    void calculateOptimization();

private:
    QRectF findBestPlacement(const QList<QRectF>& usedAreas, float partWidth, float partHeight) const;
    float calculateWaste(const QRectF& freeRect, float partWidth, float partHeight) const;

private:
    void setupUI();

    QWidget *inputPanel;
    QTableWidget *partsTable;
    QTableWidget *cutsTable;
    QDoubleSpinBox *materialWidthSpin;
    QDoubleSpinBox *materialHeightSpin;
    QPushButton *calculateButton;

    QList<Part> parts;
    float materialWidth = 2440.0f; // Standard 8x6ft sheet in mm
    float materialHeight = 1830.0f;
    QTabWidget *tabWidget;
    QList<QGraphicsView*> sheetViews;
    QList<QGraphicsScene*> sheetScenes;

    CuttingPreviewWidget *previewWidget;
};

#endif // MATERIALOPTIMIZERWIDGET_H
