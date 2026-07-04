#include "gameplayhelper.h"
#include "core/pokercompareutils.h"
#include "core/pokertypeutils.h"
#include "core/pokerutils.h"
#include "ui/gameuikit.h"

#include <QHBoxLayout>
#include <QPushButton>

GameplayHelper::PlayValidation GameplayHelper::validatePlay(const QList<Poker> &selected,
                                                            const QList<Poker> &hand,
                                                            const QList<Poker> &activityCards,
                                                            int activitySeat,
                                                            int mySeat,
                                                            bool landlordFirstMustPlay)
{
    PlayValidation result;
    if (selected.isEmpty()) {
        result.message = QStringLiteral("请选择要出的牌");
        return result;
    }

    if (!PokerTypeUtils::getType(selected).isValid()) {
        result.message = QStringLiteral("牌型不合法，请重新选择");
        return result;
    }

    if (landlordFirstMustPlay && hand.size() == 20) {
        result.message = QStringLiteral("地主首张必须出牌");
        return result;
    }

    if (activitySeat != mySeat && !activityCards.isEmpty()) {
        if (!PokerCompareUtils::comparePokers(selected, activityCards)) {
            result.message = QStringLiteral("没有上家的牌大");
            return result;
        }
    }

    Q_UNUSED(hand)
    result.ok = true;
    return result;
}

void GameplayHelper::hideButtons(QWidget *panel)
{
    if (!panel)
        return;
    if (auto *layout = panel->layout()) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget())
                item->widget()->deleteLater();
            delete item;
        }
    }
}

void GameplayHelper::showButtonGroup(QWidget *panel, GameConstants::ButtonGroup group,
                                     const std::function<void()> &onPass,
                                     const std::function<void()> &onConfirm,
                                     const std::function<void()> &onHint,
                                     const std::function<void()> &onPlay,
                                     const std::function<void()> &onLeave,
                                     const std::function<void()> &onRestart)
{
    hideButtons(panel);
    auto *layout = qobject_cast<QHBoxLayout *>(panel->layout());
    if (!layout)
        return;

    auto connectBtn = [](QPushButton *btn, const std::function<void()> &handler) {
        if (btn && handler)
            QObject::connect(btn, &QPushButton::clicked, btn, handler);
    };

    if (group == GameConstants::ButtonsCallLandlord || group == GameConstants::ButtonsFightLandlord) {
        GameUiKit::bindLandlordButtonsModern(panel, layout, group, onPass, onConfirm);
    } else if (group == GameConstants::ButtonsDiscard) {
        auto *passBtn = GameUiKit::createImageButton(panel, QStringLiteral("button_pass.png"),
                                                     QStringLiteral("不要"));
        auto *hintBtn = GameUiKit::createImageButton(panel, QStringLiteral("button_tip.png"),
                                                     QStringLiteral("提示"));
        auto *playBtn = GameUiKit::createImageButton(panel, QStringLiteral("button_ok.png"),
                                                     QStringLiteral("出牌"));
        layout->addWidget(passBtn);
        layout->addWidget(hintBtn);
        layout->addWidget(playBtn);
        connectBtn(passBtn, onPass);
        connectBtn(hintBtn, onHint);
        connectBtn(playBtn, onPlay);
    } else if (group == GameConstants::ButtonsGameOver) {
        auto *leaveBtn = GameUiKit::createImageButton(panel, QStringLiteral("button_leaveRoom.png"),
                                                      QStringLiteral("离开"));
        auto *restartBtn = GameUiKit::createImageButton(panel, QStringLiteral("button_newGame.png"),
                                                        QStringLiteral("再来一局"));
        layout->addWidget(leaveBtn);
        layout->addWidget(restartBtn);
        connectBtn(leaveBtn, onLeave);
        connectBtn(restartBtn, onRestart);
    }

    panel->show();
}
