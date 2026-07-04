#ifndef CARDFLYANIMATOR_H
#define CARDFLYANIMATOR_H

#include "core/poker.h"

#include <QObject>
#include <QPointF>
#include <functional>

class QWidget;

class CardFlyAnimator : public QObject
{
    Q_OBJECT

public:
    explicit CardFlyAnimator(QWidget *overlayParent);

    void flyCards(const QList<Poker> &cards,
                  const QPointF &fromGlobal,
                  const QPointF &toGlobal,
                  const std::function<void()> &finished = {});

    static void fly(QWidget *overlayParent,
                    const QList<Poker> &cards,
                    const QPointF &fromGlobal,
                    const QPointF &toGlobal,
                    const std::function<void()> &finished = {});

private:
    QWidget *m_parent = nullptr;
};

#endif // CARDFLYANIMATOR_H
