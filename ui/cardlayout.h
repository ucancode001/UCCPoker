#ifndef CARDLAYOUT_H
#define CARDLAYOUT_H

#include <QList>
#include <QSize>

namespace CardLayout {

// Matches Check layout_json fan centering (20-card reference width).
constexpr int ReferenceHandCount = 20;
constexpr int DealTickMs = 300;

enum class Mode {
    PlayerHand,
    PlayerPlayed,
    OpponentLeft,
    OpponentRight,
    BottomCards,
    VerticalStack
};

struct Metrics {
    int step = 0;
    int startX = 0;
    int totalWidth = 0;
    int totalHeight = 0;
};

Metrics layoutMetrics(Mode mode, int cardCount, const QSize &cardSize, bool selectableHand);

int horizontalStep(Mode mode, int cardCount, int cardWidth);

int centeredStartX(int cardCount, int step);

int compactStartX(int cardCount, int step, bool alignRight);

} // namespace CardLayout

#endif // CARDLAYOUT_H
