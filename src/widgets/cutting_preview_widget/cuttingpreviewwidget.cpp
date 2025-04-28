#include "cuttingpreviewwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QBrush>
#include <QMouseEvent>
#include <QToolTip>

CuttingPreviewWidget::CuttingPreviewWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(600, 400);
    setMouseTracking(true);
}

QSize CuttingPreviewWidget::sizeHint() const
{
    return QSize(800, 600);
}

void CuttingPreviewWidget::setSheetSize(float width, float height)
{
    sheetWidth = width;
    sheetHeight = height;
    update();
}

void CuttingPreviewWidget::setParts(const QList<QRectF>& parts)
{
    placedParts = parts;
    update();
}

void CuttingPreviewWidget::setMaterialColor(const QColor& color)
{
    materialColor = color;
    update();
}

void CuttingPreviewWidget::setShowDimensions(bool show)
{
    showDimensions = show;
    update();
}

void CuttingPreviewWidget::setShowPartNumbers(bool show)
{
    showPartNumbers = show;
    update();
}

QRectF CuttingPreviewWidget::getScaledRect(const QRectF& part, float scale, float offsetX, float offsetY) const
{
    return QRectF(
        offsetX + part.x() * scale,
        offsetY + part.y() * scale,
        part.width() * scale,
        part.height() * scale
        );
}

void CuttingPreviewWidget::drawPartInfo(QPainter& painter, const QRectF& partRect, const QRectF& originalPart)
{
    QString info;
    if (showDimensions) {
        info = QString("%1 x %2 mm").arg(originalPart.width()).arg(originalPart.height());
    }
    if (showPartNumbers && hoveredPartIndex >= 0) {
        if (!info.isEmpty()) info += "\n";
        info += QString("Part #%1").arg(hoveredPartIndex + 1);
    }

    if (!info.isEmpty()) {
        QFont font = painter.font();
        font.setPointSize(8);
        painter.setFont(font);

        QRectF textRect = partRect.adjusted(5, 5, -5, -5);
        painter.setPen(Qt::black);
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignTop, info);
    }
}

void CuttingPreviewWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    float margin = 20.0f;
    float availableWidth = width() - 2 * margin;
    float availableHeight = height() - 2 * margin;
    float scaleX = availableWidth / sheetWidth;
    float scaleY = availableHeight / sheetHeight;
    float scale = qMin(scaleX, scaleY);

    float offsetX = margin + (availableWidth - sheetWidth * scale) / 2.0f;
    float offsetY = margin + (availableHeight - sheetHeight * scale) / 2.0f;

    QRectF panelRect(offsetX, offsetY, sheetWidth * scale, sheetHeight * scale);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter.setBrush(materialColor);
    painter.drawRect(panelRect);

    painter.setPen(Qt::darkGray);
    painter.drawText(panelRect, Qt::AlignBottom | Qt::AlignHCenter,
                     QString("%1 x %2 mm").arg(sheetWidth).arg(sheetHeight));

    for (int i = 0; i < placedParts.size(); ++i) {
        const QRectF& part = placedParts[i];
        QRectF scaledPart = getScaledRect(part, scale, offsetX, offsetY);

        if (i == hoveredPartIndex) {
            painter.setPen(QPen(Qt::blue, 2));
            painter.setBrush(QColor(100, 200, 255, 200));
        } else {
            painter.setPen(QPen(Qt::darkBlue, 1));
            painter.setBrush(QColor(173, 216, 230, 150));
        }

        painter.drawRect(scaledPart);
        drawPartInfo(painter, scaledPart, part);
    }

    float totalArea = sheetWidth * sheetHeight;
    float usedArea = 0.0f;
    for (const QRectF& part : placedParts) {
        usedArea += part.width() * part.height();
    }
    float utilization = (usedArea / totalArea) * 100.0f;

    painter.setPen(Qt::black);
    painter.drawText(rect().adjusted(5, 5, -5, -5), Qt::AlignRight | Qt::AlignTop,
                     QString("Utilization: %1%").arg(utilization, 0, 'f', 1));
}

void CuttingPreviewWidget::mouseMoveEvent(QMouseEvent *event)
{
    float margin = 20.0f;
    float availableWidth = width() - 2 * margin;
    float availableHeight = height() - 2 * margin;
    float scaleX = availableWidth / sheetWidth;
    float scaleY = availableHeight / sheetHeight;
    float scale = qMin(scaleX, scaleY);
    float offsetX = margin + (availableWidth - sheetWidth * scale) / 2.0f;
    float offsetY = margin + (availableHeight - sheetHeight * scale) / 2.0f;

    int oldHovered = hoveredPartIndex;
    hoveredPartIndex = -1;

    for (int i = 0; i < placedParts.size(); ++i) {
        QRectF scaledPart = getScaledRect(placedParts[i], scale, offsetX, offsetY);
        if (scaledPart.contains(event->pos())) {
            hoveredPartIndex = i;
            break;
        }
    }

    if (hoveredPartIndex != oldHovered) {
        if (hoveredPartIndex >= 0) {
            const QRectF& part = placedParts[hoveredPartIndex];
            QToolTip::showText(event->globalPos(),
                               QString("Part %1\nSize: %2 x %3 mm\nPosition: %4, %5 mm")
                                   .arg(hoveredPartIndex + 1)
                                   .arg(part.width())
                                   .arg(part.height())
                                   .arg(part.x())
                                   .arg(part.y()),
                               this);
        } else {
            QToolTip::hideText();
        }
        update();
    }
}

void CuttingPreviewWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (hoveredPartIndex != -1) {
        hoveredPartIndex = -1;
        update();
    }
    QToolTip::hideText();
}
