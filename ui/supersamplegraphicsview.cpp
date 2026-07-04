#include "supersamplegraphicsview.h"

#include <QImage>
#include <QPainter>
#include <QPaintEvent>

SupersampleGraphicsView::SupersampleGraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
}

void SupersampleGraphicsView::setSupersampleFactor(int factor)
{
    const int clamped = qBound(1, factor, 3);
    if (m_supersample == clamped)
        return;
    m_supersample = clamped;
    viewport()->update();
}

void SupersampleGraphicsView::paintEvent(QPaintEvent *event)
{
    if (m_supersample <= 1 || !viewport() || viewport()->size().isEmpty()) {
        QGraphicsView::paintEvent(event);
        return;
    }

    const QSize targetSize = viewport()->size();
    const QSize renderSize(targetSize.width() * m_supersample, targetSize.height() * m_supersample);

    QImage image(renderSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter imagePainter(&image);
    imagePainter.setRenderHint(QPainter::Antialiasing);
    imagePainter.setRenderHint(QPainter::SmoothPixmapTransform);
    imagePainter.setRenderHint(QPainter::TextAntialiasing);

    const QRectF sceneRect = scene() ? scene()->sceneRect() : QRectF();
    if (scene())
        scene()->render(&imagePainter, QRectF(QPointF(0, 0), renderSize), sceneRect, Qt::KeepAspectRatio);

    imagePainter.end();

    QPainter viewportPainter(viewport());
    viewportPainter.setRenderHint(QPainter::SmoothPixmapTransform);
    viewportPainter.fillRect(viewport()->rect(), backgroundBrush());
    viewportPainter.drawImage(viewport()->rect(), image);
}
