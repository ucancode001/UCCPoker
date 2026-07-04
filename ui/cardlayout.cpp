#include "cardlayout.h"

#include <QtGlobal>

namespace CardLayout {

int horizontalStep(Mode mode, int cardCount, int cardWidth)
{
    Q_UNUSED(cardWidth)
    switch (mode) {
    case Mode::PlayerHand:
        if (cardCount <= 1)
            return 88;
        if (cardCount <= 10)
            return 52;
        if (cardCount <= 15)
            return 44;
        return 36;
    case Mode::PlayerPlayed:
        if (cardCount <= 1)
            return 88;
        return 40;
    case Mode::OpponentLeft:
    case Mode::OpponentRight:
    case Mode::BottomCards:
        if (cardCount <= 1)
            return 88;
        return 28;
    case Mode::VerticalStack:
        return 18;
    }
    return 36;
}

int centeredStartX(int cardCount, int step)
{
    if (cardCount <= 0)
        return 0;
    return qRound((ReferenceHandCount - cardCount) * 0.5 * step);
}

int compactStartX(int cardCount, int step, bool alignRight)
{
    if (!alignRight || cardCount <= 0)
        return 0;
    return (ReferenceHandCount - cardCount) * step;
}

Metrics layoutMetrics(Mode mode, int cardCount, const QSize &cardSize, bool selectableHand)
{
    Metrics metrics;
    if (cardCount <= 0)
        return metrics;

    metrics.step = horizontalStep(mode, cardCount, cardSize.width());

    switch (mode) {
    case Mode::PlayerHand:
    case Mode::PlayerPlayed:
        metrics.startX = centeredStartX(cardCount, metrics.step);
        metrics.totalWidth = metrics.startX + cardSize.width() + metrics.step * (cardCount - 1);
        metrics.totalHeight = cardSize.height() + (selectableHand ? 16 : 0);
        break;
    case Mode::OpponentLeft:
    case Mode::BottomCards:
        metrics.startX = compactStartX(cardCount, metrics.step, false);
        metrics.totalWidth = metrics.startX + cardSize.width() + metrics.step * (cardCount - 1);
        metrics.totalHeight = cardSize.height();
        break;
    case Mode::OpponentRight:
        metrics.startX = compactStartX(cardCount, metrics.step, true);
        metrics.totalWidth = metrics.startX + cardSize.width() + metrics.step * (cardCount - 1);
        metrics.totalHeight = cardSize.height();
        break;
    case Mode::VerticalStack:
        metrics.startX = 4;
        metrics.totalWidth = metrics.startX + cardSize.width() + 4;
        metrics.totalHeight = cardSize.height() + (cardCount - 1) * metrics.step + 8;
        break;
    }

    return metrics;
}

} // namespace CardLayout
