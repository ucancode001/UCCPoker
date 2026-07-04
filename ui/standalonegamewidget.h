#ifndef STANDALONEGAMEWIDGET_H
#define STANDALONEGAMEWIDGET_H

#include "core/gameconstants.h"
#include "core/landlordbidengine.h"
#include "core/poker.h"
#include "core/pokerutils.h"

#include <QWidget>

class GameTableView;
class QLabel;
class QTimer;

class StandaloneGameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StandaloneGameWidget(QWidget *parent = nullptr);

signals:
    void backToHallRequested();
    void restartRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onDealTick();
    void onAutoPlayTick();
    void onBidPass();
    void onBidConfirm();
    void onBidAiTick();
    void onPlayCards();
    void onPass();
    void onHint();
    void onLeaveRoom();
    void onRestart();

private:
    enum GamePhase {
        PhaseDealing,
        PhaseBidLandlord,
        PhasePlaying,
        PhaseFinished
    };

    void resetGame();
    void startDealing();
    void finishDealing();
    void startBidding();
    void processBidTurn();
    void finishBidding(int landlordSeat);
    void refreshPlayerHand();
    void refreshBottomCards();
    void updateLandlordBadges();
    void showButtonGroup(GameConstants::ButtonGroup group);
    void hideButtons();
    void clearOtherPlayAreas();
    void showPassTip(int seat);
    void scheduleAutoPlay();
    void executeAutoPlay();
    bool checkGameOver();
    void showGameResult(bool victory);

    GameTableView *m_table = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_timerLabel = nullptr;

    QTimer *m_dealTimer = nullptr;
    QTimer *m_autoTimer = nullptr;
    QTimer *m_bidTimer = nullptr;
    QTimer *m_countdownTimer = nullptr;
    LandlordBidEngine m_bidEngine;

    GamePhase m_phase = PhaseDealing;
    int m_dealIndex = 0;

    int m_mySeat = GameConstants::SeatPlayer;
    int m_landlordSeat = 0;
    int m_playingSeat = 0;
    int m_playedSeat = 0;
    int m_countdown = 0;

    QList<Poker> m_deck;
    QList<Poker> m_playerCards;
    QList<Poker> m_leftCards;
    QList<Poker> m_rightCards;
    QList<Poker> m_bottomCardsData;
    QList<Poker> m_playedCards;
    QList<Poker> m_selectedCards;

    RandomUserInfo m_leftPlayer;
    RandomUserInfo m_rightPlayer;
    QString m_playerName;
    bool m_gameFinished = false;
    bool m_playerVictory = false;
};

#endif // STANDALONEGAMEWIDGET_H
