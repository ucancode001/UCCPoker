#ifndef CLASSICGAMEWIDGET_H
#define CLASSICGAMEWIDGET_H

#include "core/gameconstants.h"
#include "core/poker.h"

#include <QJsonObject>
#include <QWidget>

class GameTableView;
class GameWebSocketClient;
class QLabel;
class QTimer;

class ClassicGameWidget : public QWidget
{
    Q_OBJECT

public:
    enum class RoomEntry {
        Classic,
        FriendCreate,
        FriendJoin
    };

    explicit ClassicGameWidget(RoomEntry entry = RoomEntry::Classic,
                               const QString &friendRoomId = QString(),
                               QWidget *parent = nullptr);
    ~ClassicGameWidget() override;

signals:
    void backToHallRequested();
    void restartRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onConnected();
    void onDisconnected();
    void onConnectionError(const QString &message);
    void onReconnecting(int attempt, int maxAttempts);
    void onSendFailed(int code, const QString &reason);
    void onRoomMessage(int code, const QJsonObject &data);
    void onDealTick();
    void onCountdownTick();
    void onPlayCards();
    void onPass();
    void onHint();
    void onBidPass();
    void onBidConfirm();
    void onLeaveRoom();
    void onRestart();

private:
    struct RemotePlayer {
        QString id;
        QString name;
        int seat = 0;
        int cardCount = 17;
    };

    void setupUi();
    void connectServer();
    void enterRoomFlow();
    void sendDiscard(const QList<Poker> &cards);
    void sendPass();
    void sendLandlordBid(const QString &action);
    QList<Poker> parsePokerArray(const QJsonArray &array) const;
    bool isLeftSeat(int seat) const;
    void refreshPlayerHand();
    void showButtons(GameConstants::ButtonGroup group);
    void hideButtons();
    void setStatus(const QString &text);
    void updateLandlordUi();
    void updateOpponentCounts();
    void showResult(bool victory);
    void handleBidPrompt(const QJsonObject &data);
    void restoreReconnectState(const QJsonObject &data);
    void applyPlayersFromJson(const QJsonArray &players);
    void startTurnCountdown();
    void stopTurnCountdown();
    void showPlayedCardsAnimated(int seat, const QList<Poker> &cards);
    RemotePlayer *playerBySeat(int seat);

    GameWebSocketClient *m_client = nullptr;
    GameTableView *m_table = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_connectionLabel = nullptr;
    QLabel *m_timerLabel = nullptr;
    QTimer *m_dealTimer = nullptr;
    QTimer *m_countdownTimer = nullptr;

    RoomEntry m_entry = RoomEntry::Classic;
    QString m_friendRoomId;
    QString m_userId;
    QString m_roomId;
    int m_mySeat = GameConstants::SeatPlayer;
    int m_landlordSeat = 0;
    int m_activitySeat = 0;
    int m_nextSeat = 0;
    int m_countdown = 0;
    GameConstants::ButtonGroup m_bidButtonGroup = GameConstants::ButtonsCallLandlord;

    QList<Poker> m_playerCards;
    QList<Poker> m_activityCards;
    QList<Poker> m_publicCards;
    QList<Poker> m_selectedCards;
    RemotePlayer m_leftPlayer;
    RemotePlayer m_rightPlayer;

    int m_dealIndex = 0;
    bool m_dealing = false;
    bool m_gameStarted = false;
    bool m_waitingForMyTurn = false;
};

#endif // CLASSICGAMEWIDGET_H
