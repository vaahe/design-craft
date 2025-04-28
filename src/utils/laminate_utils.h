#ifndef CREATE_LAMINATE_PATTERN_H
#define CREATE_LAMINATE_PATTERN_H

#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>

class LaminateUtils {
public:
    static QBrush createLaminatePattern(float width, float height) {
        // Create a pixmap for the pattern
        QPixmap patternPixmap(width, height);
        patternPixmap.fill(Qt::white);

        QPainter painter(&patternPixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        // Wood-like laminate pattern
        QLinearGradient gradient(0, 0, width, height);
        gradient.setColorAt(0, QColor(210, 180, 140)); // Light wood color
        gradient.setColorAt(0.5, QColor(180, 130, 90)); // Medium wood color
        gradient.setColorAt(1, QColor(150, 100, 70)); // Dark wood color

        painter.fillRect(0, 0, width, height, gradient);

        // Add wood grain details
        QPen grainPen(QColor(120, 80, 50, 100));
        grainPen.setWidth(1);
        painter.setPen(grainPen);

        // Draw wood grain lines
        for (int i = 0; i < width; i += 20) {
            QPainterPath path;
            path.moveTo(i, 0);

            // Create wavy wood grain pattern
            for (int y = 0; y < height; y += 5) {
                int offset = qSin(y * 0.05) * 5;
                path.lineTo(i + offset, y);
            }

            painter.drawPath(path);
        }

        return QBrush(patternPixmap);
    }
};

#endif // CREATE_LAMINATE_PATTERN_H
