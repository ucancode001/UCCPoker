#ifndef CARDHANDWIDGET_H
#define CARDHANDWIDGET_H

#include "core/poker.h"
#include "ui/cardlayout.h"

#include <QList>
#include <QRect>
#include <QSet>
#include <QWidget>

class QPaintEvent;
class QMouseEvent;

class CardHandWidget : public QWidget
{
    Q_OBJECT

public:
    using LayoutMode = CardLayout::Mode;

    explicit CardHandWidget(QWidget *parent = nullptr);

    void setCards(const QList<Poker> &cards, LayoutMode mode = LayoutMode::PlayerHand, bool faceDown = false);
    void showDealingHand(const QList<Poker> &sortedHand, int visibleCount, bool faceDown = false);
    void syncHand(const QList<Poker> &sortedHand, LayoutMode mode = LayoutMode::PlayerHand, bool faceDown = false);

    void setSelectable(bool selectable);
    QList<Poker> selectedCards() const;
    void setSelectedCards(const QList<Poker> &cards);
    void clearSelection();

signals:
    void selectionChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    static QList<Poker> uniqueCards(const QList<Poker> &cards, int limit = -1);
    void applyCards(const QList<Poker> &cards, bool clearSelection);
    void recomputeLayout();
    int hitTestCardIndex(const QPoint &pos) const;
    bool isSelectedIndex(int index) const;
    void setSelectedIndex(int index, bool selected);
    void pruneSelection();

    QList<Poker> m_cards;
    QList<QRect> m_drawRects;
    QSet<int> m_selectedIndices;
    LayoutMode m_mode = LayoutMode::PlayerHand;
    bool m_selectable = false;
    bool m_faceDown = false;
};

#endif // CARDHANDWIDGET_H
