#ifndef CUTTINGPREVIEWWIDGET_H
#define CUTTINGPREVIEWWIDGET_H

#include <QWidget>
#include <QRectF>
#include <QColor>

class CuttingPreviewWidget : public QWidget {
    Q_OBJECT

public:
    explicit CuttingPreviewWidget(QWidget *parent = nullptr);

    void setSheetSize(float width, float height); // mm
    void setParts(const QList<QRectF>& parts);
    void setMaterialColor(const QColor& color);
    void setShowDimensions(bool show);
    void setShowPartNumbers(bool show);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    float sheetWidth = 2800.0f;  // Standard sheet size (2800x2070mm)
    float sheetHeight = 2070.0f;
    QList<QRectF> placedParts;
    QColor materialColor = QColor(245, 245, 245);
    bool showDimensions = true;
    bool showPartNumbers = true;
    int hoveredPartIndex = -1;

    QRectF getScaledRect(const QRectF& part, float scale, float offsetX, float offsetY) const;
    void drawPartInfo(QPainter& painter, const QRectF& partRect, const QRectF& originalPart);
};

#endif // CUTTINGPREVIEWWIDGET_H
