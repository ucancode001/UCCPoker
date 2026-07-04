#include "cardhandwidget.h"
#include "cardwidget.h"

#include <QMouseEvent>
#include <QPainter>

#include <algorithm>

CardHandWidget::CardHandWidget(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAutoFillBackground(false);
    setStyleSheet(QStringLiteral("background: transparent; border: none;"));
}

QList<Poker> CardHandWidget::uniqueCards(const QList<Poker> &cards, int limit)
{
    QList<Poker> result;
    QSet<int> seen;
    result.reserve(limit >= 0 ? qMin(limit, cards.size()) : cards.size());
    for (const Poker &card : cards) {
        if (seen.contains(card.id()))
            continue;
        seen.insert(card.id());
        result.append(card);
        if (limit >= 0 && result.size() >= limit)
            break;
    }
    return result;
}

void CardHandWidget::applyCards(const QList<Poker> &cards, bool clearSelection)
{
    m_cards = uniqueCards(cards);
    if (clearSelection)
        m_selectedIndices.clear();
    else
        pruneSelection();
}

void CardHandWidget::setCards(const QList<Poker> &cards, LayoutMode mode, bool faceDown)
{
    m_mode = mode;
    m_faceDown = faceDown;
    applyCards(cards, true);
    recomputeLayout();
    updateGeometry();
    update();
}

void CardHandWidget::syncHand(const QList<Poker> &sortedHand, LayoutMode mode, bool faceDown)
{
    m_mode = mode;
    m_faceDown = faceDown;

    const QList<Poker> normalized = uniqueCards(sortedHand);
    const bool sameHand = (normalized.size() == m_cards.size());
    if (sameHand) {
        bool identical = true;
        for (int i = 0; i < normalized.size(); ++i) {
            if (normalized.at(i).id() != m_cards.at(i).id()) {
                identical = false;
                break;
            }
        }
        if (identical) {
            recomputeLayout();
            update();
            return;
        }
    }

    applyCards(sortedHand, !sameHand);
    recomputeLayout();
    updateGeometry();
    update();
}

void CardHandWidget::showDealingHand(const QList<Poker> &sortedHand, int visibleCount, bool faceDown)
{
    m_mode = LayoutMode::PlayerHand;
    m_faceDown = faceDown;

    const int count = qBound(0, visibleCount, sortedHand.size());
    applyCards(sortedHand.mid(0, count), true);
    recomputeLayout();
    updateGeometry();
    update();
}

void CardHandWidget::setSelectable(bool selectable)
{
    if (m_selectable == selectable)
        return;
    m_selectable = selectable;
    if (!m_selectable)
        m_selectedIndices.clear();
    recomputeLayout();
    update();
}

QList<Poker> CardHandWidget::selectedCards() const
{
    QList<Poker> selected;
    QList<int> indices = m_selectedIndices.values();
    std::sort(indices.begin(), indices.end());
    for (int index : indices) {
        if (index >= 0 && index < m_cards.size())
            selected.append(m_cards.at(index));
    }
    return selected;
}

void CardHandWidget::setSelectedCards(const QList<Poker> &cards)
{
    m_selectedIndices.clear();
    for (const Poker &card : cards) {
        for (int i = 0; i < m_cards.size(); ++i) {
            if (m_cards.at(i).id() == card.id()) {
                m_selectedIndices.insert(i);
                break;
            }
        }
    }
    recomputeLayout();
    update();
    emit selectionChanged();
}

void CardHandWidget::clearSelection()
{
    if (m_selectedIndices.isEmpty())
        return;
    m_selectedIndices.clear();
    recomputeLayout();
    update();
    emit selectionChanged();
}

void CardHandWidget::pruneSelection()
{
    QSet<int> valid;
    for (int index : m_selectedIndices) {
        if (index >= 0 && index < m_cards.size())
            valid.insert(index);
    }
    m_selectedIndices = valid;
}

bool CardHandWidget::isSelectedIndex(int index) const
{
    return m_selectedIndices.contains(index);
}

void CardHandWidget::setSelectedIndex(int index, bool selected)
{
    if (selected)
        m_selectedIndices.insert(index);
    else
        m_selectedIndices.remove(index);
}

void CardHandWidget::recomputeLayout()
{
    m_drawRects.clear();

    if (m_cards.isEmpty()) {
        setFixedSize(0, 0);
        return;
    }

    const QSize cardSize = CardWidget::cardSize();
    const bool selectableHand = m_selectable && m_mode == LayoutMode::PlayerHand;
    const CardLayout::Metrics metrics = CardLayout::layoutMetrics(
        m_mode, m_cards.size(), cardSize, selectableHand);

    setFixedSize(metrics.totalWidth, metrics.totalHeight);

    m_drawRects.reserve(m_cards.size());

    if (m_mode == LayoutMode::VerticalStack) {
        for (int i = 0; i < m_cards.size(); ++i)
            m_drawRects.append(QRect(metrics.startX, i * metrics.step, cardSize.width(), cardSize.height()));
        return;
    }

    const int restY = selectableHand ? CardWidget::selectionLift() : 0;
    int x = metrics.startX;
    for (int i = 0; i < m_cards.size(); ++i) {
        const int y = (selectableHand && isSelectedIndex(i)) ? 0 : restY;
        m_drawRects.append(QRect(x, y, cardSize.width(), cardSize.height()));
        x += metrics.step;
    }
}

int CardHandWidget::hitTestCardIndex(const QPoint &pos) const
{
    for (int i = m_drawRects.size() - 1; i >= 0; --i) {
        if (m_drawRects.at(i).contains(pos))
            return i;
    }
    return -1;
}

void CardHandWidget::paintEvent(QPaintEvent *)
{
    if (m_cards.isEmpty() || m_drawRects.size() != m_cards.size())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Strict paint order: index 0 (left) drawn first, index n-1 (right) drawn last on top.
    for (int i = 0; i < m_cards.size(); ++i)
        CardWidget::paintCard(painter, m_drawRects.at(i), m_cards.at(i), m_faceDown, isSelectedIndex(i));
}

void CardHandWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_selectable || m_faceDown || event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    const int index = hitTestCardIndex(event->pos());
    if (index < 0) {
        QWidget::mousePressEvent(event);
        return;
    }

    setSelectedIndex(index, !isSelectedIndex(index));
    recomputeLayout();
    update();
    emit selectionChanged();
    event->accept();
}
