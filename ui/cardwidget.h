#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include "core/poker.h"

#include <QRect>
#include <QSize>
#include <QWidget>

class QPainter;

class CardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CardWidget(const Poker &poker, QWidget *parent = nullptr);

    Poker poker() const { return m_poker; }
    bool isSelected() const { return m_selected; }
    void setSelected(bool selected);
    void setFaceDown(bool faceDown);
    bool isFaceDown() const { return m_faceDown; }

    static QSize cardSize();
    static int selectionLift();
    static void paintCard(QPainter &painter, const QRect &rect, const Poker &poker, bool faceDown,
                          bool selected);

signals:
    void clicked(CardWidget *card);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Poker m_poker;
    bool m_selected = false;
    bool m_faceDown = false;
};

#endif // CARDWIDGET_H
