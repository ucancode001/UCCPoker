#ifndef GAMEPLAYHELPER_H
#define GAMEPLAYHELPER_H

#include "core/poker.h"
#include "core/gameconstants.h"

#include <QHBoxLayout>
#include <QWidget>
#include <functional>

class GameplayHelper
{
public:
    struct PlayValidation {
        bool ok = false;
        QString message;
    };

    static PlayValidation validatePlay(const QList<Poker> &selected,
                                       const QList<Poker> &hand,
                                       const QList<Poker> &activityCards,
                                       int activitySeat,
                                       int mySeat,
                                       bool landlordFirstMustPlay);

    static void showButtonGroup(QWidget *panel, GameConstants::ButtonGroup group,
                                const std::function<void()> &onPass,
                                const std::function<void()> &onConfirm,
                                const std::function<void()> &onHint,
                                const std::function<void()> &onPlay,
                                const std::function<void()> &onLeave,
                                const std::function<void()> &onRestart);

    static void hideButtons(QWidget *panel);
};

#endif // GAMEPLAYHELPER_H
