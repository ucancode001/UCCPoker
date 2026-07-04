#include "cardwidget.h"
#include "core/pokerutils.h"
#include "core/resourcemanager.h"

#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

CardWidget::CardWidget(const Poker &poker, QWidget *parent)
    : QWidget(parent)
    , m_poker(poker)
{
    setFixedSize(cardSize());
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

QSize CardWidget::cardSize()
{
    return QSize(88, 122);
}

int CardWidget::selectionLift()
{
    return 16;
}

void CardWidget::paintCard(QPainter &painter, const QRect &rect, const Poker &poker, bool faceDown,
                           bool selected)
{
    painter.setRenderHint(QPainter::Antialiasing);

    QPixmap source = faceDown ? ResourceManager::instance().cardBackPixmap()
                              : ResourceManager::instance().cardPixmap(poker.id());
    if (!source.isNull()) {
        painter.drawPixmap(rect, source.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        if (selected) {
            painter.setPen(QPen(QColor(255, 193, 7), 3));
            painter.drawRoundedRect(rect.adjusted(1, 1, -1, -1), 8, 8);
        }
        return;
    }

    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0, QColor(255, 255, 255));
    gradient.setColorAt(1, QColor(240, 240, 245));
    QPainterPath path;
    path.addRoundedRect(rect, 8, 8);
    painter.fillPath(path, gradient);
    painter.setPen(QPen(QColor(40, 40, 40), 2));
    painter.drawPath(path);

    const bool red = PokerUtils::isRedSuit(poker.id());
    painter.setPen(red ? QColor(200, 30, 30) : QColor(20, 20, 20));
    painter.setFont(QFont(QStringLiteral("Microsoft YaHei"), 16, QFont::Bold));
    painter.drawText(rect.adjusted(8, 6, 0, 0), Qt::AlignLeft | Qt::AlignTop,
                     PokerUtils::rankLabel(poker.orderValue()));

    if (selected) {
        painter.setPen(QPen(QColor(255, 193, 7), 3));
        painter.drawRoundedRect(rect.adjusted(1, 1, -1, -1), 8, 8);
    }
}

void CardWidget::setSelected(bool selected)
{
    if (m_selected == selected)
        return;
    m_selected = selected;
    update();
}

void CardWidget::setFaceDown(bool faceDown)
{
    if (m_faceDown == faceDown)
        return;
    m_faceDown = faceDown;
    update();
}

void CardWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    paintCard(painter, rect(), m_poker, m_faceDown, m_selected);
}

void CardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_faceDown)
        emit clicked(this);
    QWidget::mousePressEvent(event);
}
