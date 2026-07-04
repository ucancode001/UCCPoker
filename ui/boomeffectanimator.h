#ifndef BOOMEFFECTANIMATOR_H
#define BOOMEFFECTANIMATOR_H

#include "core/poker.h"

#include <QObject>
#include <QPointF>
#include <functional>

class QWidget;

class BoomEffectAnimator : public QObject
{
    Q_OBJECT

public:
    enum class Kind {
        Bomb,
        KingBomb
    };

    explicit BoomEffectAnimator(QWidget *overlayParent);

    void play(const QPointF &centerGlobal, Kind kind, const std::function<void()> &finished = {});

    static void play(QWidget *overlayParent,
                     const QPointF &centerGlobal,
                     Kind kind,
                     const std::function<void()> &finished = {});

    static void playIfNeeded(QWidget *overlayParent,
                             const QPointF &centerGlobal,
                             const QList<Poker> &cards,
                             const std::function<void()> &finished = {});

private:
    QWidget *m_parent = nullptr;
};

#endif // BOOMEFFECTANIMATOR_H
