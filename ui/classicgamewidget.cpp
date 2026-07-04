#include "classicgamewidget.h"
#include "cardhandwidget.h"
#include "core/appsettings.h"
#include "core/pokerseekutils.h"
#include "core/pokerutils.h"
#include "core/resourcemanager.h"
#include "core/roomprotocol.h"
#include "core/soundmanager.h"
#include "network/gamewebsocketclient.h"
#include "ui/gameplayhelper.h"
#include "ui/gameresultdialog.h"
#include "ui/gametableview.h"
#include "ui/gameuikit.h"
#include "ui/cardflyanimator.h"
#include "ui/boomeffectanimator.h"
#include "ui/cardlayout.h"

#include <QHBoxLayout>
#include <QJsonArray>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

ClassicGameWidget::ClassicGameWidget(RoomEntry entry, const QString &friendRoomId, QWidget *parent)
    : QWidget(parent)
    , m_entry(entry)
    , m_friendRoomId(friendRoomId)
{
    m_client = new GameWebSocketClient(this);
    m_client->setAutoReconnect(AppSettings::instance().autoReconnectEnabled());
    m_client->setMaxReconnectAttempts(AppSettings::instance().reconnectMaxAttempts());

    connect(m_client, &GameWebSocketClient::connected, this, &ClassicGameWidget::onConnected);
    connect(m_client, &GameWebSocketClient::disconnected, this, &ClassicGameWidget::onDisconnected);
    connect(m_client, &GameWebSocketClient::connectionError, this, &ClassicGameWidget::onConnectionError);
    connect(m_client, &GameWebSocketClient::reconnecting, this, &ClassicGameWidget::onReconnecting);
    connect(m_client, &GameWebSocketClient::sendFailed, this, &ClassicGameWidget::onSendFailed);
    connect(m_client, &GameWebSocketClient::roomMessageReceived, this, &ClassicGameWidget::onRoomMessage);

    m_dealTimer = new QTimer(this);
    m_dealTimer->setInterval(CardLayout::DealTickMs);
    connect(m_dealTimer, &QTimer::timeout, this, &ClassicGameWidget::onDealTick);

    m_countdownTimer = new QTimer(this);
    m_countdownTimer->setInterval(1000);
    connect(m_countdownTimer, &QTimer::timeout, this, &ClassicGameWidget::onCountdownTick);

    setupUi();
    SoundManager::instance().startBackgroundMusic();
    connectServer();
}

ClassicGameWidget::~ClassicGameWidget()
{
    m_client->disconnectFromServer();
}

void ClassicGameWidget::setupUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 12, 16, 12);

    auto *topBar = new QHBoxLayout;
    auto *backButton = GameUiKit::createFlatButton(this, QStringLiteral("返回大厅"), "#546E7A");
    connect(backButton, &QPushButton::clicked, this, &ClassicGameWidget::backToHallRequested);

    m_statusLabel = new QLabel(QStringLiteral("正在连接服务器..."), this);
    m_statusLabel->setStyleSheet("color: #FFE082; font-size: 16px; font-weight: bold;");

    m_timerLabel = GameUiKit::createTimerBadge(this);
    m_timerLabel->hide();

    m_connectionLabel = new QLabel(this);
    m_connectionLabel->setStyleSheet("color: #B2DFDB; font-size: 13px;");

    topBar->addWidget(backButton);
    topBar->addStretch();
    topBar->addWidget(m_statusLabel);
    topBar->addStretch();
    topBar->addWidget(m_timerLabel);
    topBar->addWidget(m_connectionLabel);
    root->addLayout(topBar);

    m_table = new GameTableView(this);
    m_table->setOpponentCountsVisible(true);
    m_table->setSelfName(AppSettings::instance().playerName());
    m_table->playerHand()->setSelectable(true);
    connect(m_table->playerHand(), &CardHandWidget::selectionChanged, this, [this]() {
        m_selectedCards = m_table->playerHand()->selectedCards();
    });
    root->addWidget(m_table, 1);
}

void ClassicGameWidget::connectServer()
{
    m_connectionLabel->setText(AppSettings::instance().serverUrl());
    m_client->connectToServer(AppSettings::instance().buildWebSocketUrl());
}

void ClassicGameWidget::onConnected()
{
    if (m_client->reconnectAttempts() > 0 && !m_userId.isEmpty() && !m_roomId.isEmpty()) {
        setStatus(QStringLiteral("重连成功，正在恢复房间..."));
        m_client->sendMessage(RoomProtocol::RequestReconnect,
                              {{QStringLiteral("userId"), m_userId},
                               {QStringLiteral("roomId"), m_roomId}});
        return;
    }
    setStatus(QStringLiteral("已连接，等待服务器响应..."));
}

void ClassicGameWidget::onDisconnected()
{
    stopTurnCountdown();
    if (AppSettings::instance().autoReconnectEnabled() && m_gameStarted)
        setStatus(QStringLiteral("连接断开，正在尝试重连..."));
}

void ClassicGameWidget::onConnectionError(const QString &message)
{
    setStatus(QStringLiteral("连接失败：%1").arg(message));
}

void ClassicGameWidget::onReconnecting(int attempt, int maxAttempts)
{
    m_connectionLabel->setText(QStringLiteral("重连 %1/%2").arg(attempt).arg(maxAttempts));
    setStatus(QStringLiteral("正在重连服务器..."));
}

void ClassicGameWidget::onSendFailed(int code, const QString &reason)
{
    Q_UNUSED(code)
    setStatus(QStringLiteral("发送失败：%1").arg(reason));
}

void ClassicGameWidget::enterRoomFlow()
{
    if (m_entry == RoomEntry::FriendCreate) {
        setStatus(QStringLiteral("正在创建好友房间..."));
        m_client->sendMessage(RoomProtocol::RequestCreateRoom,
                              {{QStringLiteral("userId"), m_userId},
                               {QStringLiteral("userName"), AppSettings::instance().playerName()},
                               {QStringLiteral("roomId"), m_friendRoomId}});
        return;
    }

    if (m_entry == RoomEntry::FriendJoin) {
        setStatus(QStringLiteral("正在加入好友房间 %1...").arg(m_friendRoomId));
        m_client->sendMessage(RoomProtocol::RequestJoinRoom,
                              {{QStringLiteral("userId"), m_userId},
                               {QStringLiteral("userName"), AppSettings::instance().playerName()},
                               {QStringLiteral("roomId"), m_friendRoomId}});
        return;
    }

    setStatus(QStringLiteral("正在匹配经典房间..."));
    m_client->sendMessage(RoomProtocol::RequestOnRoom,
                          {{QStringLiteral("userId"), m_userId},
                           {QStringLiteral("userName"), AppSettings::instance().playerName()}});
}

bool ClassicGameWidget::isLeftSeat(int seat) const
{
    if (seat == m_mySeat + 1 || seat == m_mySeat % 3 + 1)
        return false;
    return true;
}

ClassicGameWidget::RemotePlayer *ClassicGameWidget::playerBySeat(int seat)
{
    if (m_leftPlayer.seat == seat)
        return &m_leftPlayer;
    if (m_rightPlayer.seat == seat)
        return &m_rightPlayer;
    return nullptr;
}

QList<Poker> ClassicGameWidget::parsePokerArray(const QJsonArray &array) const
{
    QList<Poker> cards;
    for (const QJsonValue &value : array) {
        const QJsonObject obj = value.toObject();
        cards.append(Poker(obj.value(QStringLiteral("id")).toInt(),
                           obj.value(QStringLiteral("sort")).toInt()));
    }
    return cards;
}

void ClassicGameWidget::applyPlayersFromJson(const QJsonArray &players)
{
    for (const QJsonValue &value : players) {
        const QJsonObject player = value.toObject();
        if (player.value(QStringLiteral("id")).toString() == m_userId) {
            m_mySeat = player.value(QStringLiteral("seat")).toInt(m_mySeat);
            if (player.contains(QStringLiteral("cardCount")))
                Q_UNUSED(player.value(QStringLiteral("cardCount")).toInt());
        }
    }

    for (const QJsonValue &value : players) {
        const QJsonObject player = value.toObject();
        if (player.value(QStringLiteral("id")).toString() == m_userId)
            continue;

        RemotePlayer remote;
        remote.id = player.value(QStringLiteral("id")).toString();
        remote.name = player.value(QStringLiteral("name")).toString();
        remote.seat = player.value(QStringLiteral("seat")).toInt();
        remote.cardCount = player.value(QStringLiteral("cardCount")).toInt(17);
        if (isLeftSeat(remote.seat))
            m_leftPlayer = remote;
        else
            m_rightPlayer = remote;
    }

    m_table->setSelfName(AppSettings::instance().playerName());
    m_table->setLeftName(m_leftPlayer.name);
    m_table->setRightName(m_rightPlayer.name);
    updateOpponentCounts();
}

void ClassicGameWidget::restoreReconnectState(const QJsonObject &data)
{
    m_roomId = data.value(QStringLiteral("roomId")).toString(m_roomId);
    AppSettings::instance().setLastRoomId(m_roomId);
    m_gameStarted = true;

    if (data.contains(QStringLiteral("players")))
        applyPlayersFromJson(data.value(QStringLiteral("players")).toArray());

    if (data.contains(QStringLiteral("pokers")))
        m_playerCards = PokerUtils::sortDesc(parsePokerArray(data.value(QStringLiteral("pokers")).toArray()));
    else if (data.contains(QStringLiteral("playerCards")))
        m_playerCards = PokerUtils::sortDesc(parsePokerArray(data.value(QStringLiteral("playerCards")).toArray()));

    if (data.contains(QStringLiteral("landlordSeat")))
        m_landlordSeat = data.value(QStringLiteral("landlordSeat")).toInt();

    if (data.contains(QStringLiteral("publicPokers")))
        m_publicCards = parsePokerArray(data.value(QStringLiteral("publicPokers")).toArray());
    else if (data.contains(QStringLiteral("publicCards")))
        m_publicCards = parsePokerArray(data.value(QStringLiteral("publicCards")).toArray());

    if (data.contains(QStringLiteral("activityPokers")))
        m_activityCards = parsePokerArray(data.value(QStringLiteral("activityPokers")).toArray());
    else if (data.contains(QStringLiteral("lastDiscard")))
        m_activityCards = parsePokerArray(data.value(QStringLiteral("lastDiscard")).toArray());

    if (data.contains(QStringLiteral("activitySeat")))
        m_activitySeat = data.value(QStringLiteral("activitySeat")).toInt();
    else if (data.contains(QStringLiteral("lastDiscardSeat")))
        m_activitySeat = data.value(QStringLiteral("lastDiscardSeat")).toInt();

    if (data.contains(QStringLiteral("nextSeat")))
        m_nextSeat = data.value(QStringLiteral("nextSeat")).toInt();

    if (data.contains(QStringLiteral("leftCount")))
        m_leftPlayer.cardCount = data.value(QStringLiteral("leftCount")).toInt();
    if (data.contains(QStringLiteral("rightCount")))
        m_rightPlayer.cardCount = data.value(QStringLiteral("rightCount")).toInt();

    refreshPlayerHand();
    m_table->bottomCards()->setCards(m_publicCards, CardHandWidget::LayoutMode::BottomCards, m_publicCards.isEmpty());
    m_table->clearPlayAreas();

    if (!m_activityCards.isEmpty() && m_activitySeat == m_mySeat)
        m_table->playerPlayed()->setCards(PokerUtils::sortDesc(m_activityCards), CardHandWidget::LayoutMode::PlayerPlayed);
    else if (!m_activityCards.isEmpty()) {
        if (isLeftSeat(m_activitySeat))
            m_table->leftPlayed()->setCards(PokerUtils::sortDesc(m_activityCards), CardHandWidget::LayoutMode::OpponentLeft);
        else
            m_table->rightPlayed()->setCards(PokerUtils::sortDesc(m_activityCards), CardHandWidget::LayoutMode::OpponentRight);
    }

    updateLandlordUi();
    updateOpponentCounts();
    hideButtons();

    const int turnSeat = m_nextSeat > 0 ? m_nextSeat : m_activitySeat;
    if (turnSeat == m_mySeat) {
        showButtons(GameConstants::ButtonsDiscard);
        setStatus(QStringLiteral("重连成功，轮到您出牌"));
        startTurnCountdown();
    } else {
        setStatus(QStringLiteral("重连成功，等待其他玩家出牌"));
        stopTurnCountdown();
    }
}

void ClassicGameWidget::handleBidPrompt(const QJsonObject &data)
{
    const int seat = data.value(QStringLiteral("seat")).toInt(m_mySeat);
    const QString phase = data.value(QStringLiteral("phase")).toString();
    m_bidButtonGroup = phase.contains(QStringLiteral("grab"), Qt::CaseInsensitive)
        || phase.contains(QStringLiteral("fight"), Qt::CaseInsensitive)
        ? GameConstants::ButtonsFightLandlord
        : GameConstants::ButtonsCallLandlord;

    if (seat == m_mySeat) {
        showButtons(m_bidButtonGroup);
        setStatus(m_bidButtonGroup == GameConstants::ButtonsFightLandlord
                      ? QStringLiteral("是否抢地主？")
                      : QStringLiteral("是否叫地主？"));
    } else {
        hideButtons();
        setStatus(QStringLiteral("等待座位 %1 叫地主...").arg(seat));
    }
}

void ClassicGameWidget::showPlayedCardsAnimated(int seat, const QList<Poker> &cards)
{
    const QList<Poker> sorted = PokerUtils::sortDesc(cards);
    const QPointF from = m_table->seatCenterGlobal(seat, m_mySeat);
    const QPointF to = seat == m_mySeat ? m_table->playerPlayedCenterGlobal()
                                        : (isLeftSeat(seat) ? m_table->leftPlayedCenterGlobal()
                                                            : m_table->rightPlayedCenterGlobal());
    CardFlyAnimator::fly(this, sorted, from, to, [this, seat, sorted, to]() {
        if (seat == m_mySeat)
            m_table->playerPlayed()->setCards(sorted, CardHandWidget::LayoutMode::PlayerPlayed);
        else if (isLeftSeat(seat))
            m_table->leftPlayed()->setCards(sorted, CardHandWidget::LayoutMode::OpponentLeft);
        else
            m_table->rightPlayed()->setCards(sorted, CardHandWidget::LayoutMode::OpponentRight);
        BoomEffectAnimator::playIfNeeded(this, to, sorted, [] {});
    });
}

void ClassicGameWidget::onRoomMessage(int code, const QJsonObject &data)
{
    switch (code) {
    case RoomProtocol::ResponseInitUserOk:
        m_userId = data.value(QStringLiteral("userId")).toString();
        enterRoomFlow();
        break;
    case RoomProtocol::ResponseCreateRoomOk:
        m_roomId = data.value(QStringLiteral("roomId")).toString(m_friendRoomId);
        AppSettings::instance().setLastRoomId(m_roomId);
        setStatus(QStringLiteral("好友房间 %1 已创建，等待玩家加入").arg(m_roomId));
        break;
    case RoomProtocol::ResponseReconnectOk:
        restoreReconnectState(data);
        break;
    case RoomProtocol::ResponseRoomInfo: {
        m_roomId = data.value(QStringLiteral("roomId")).toString();
        AppSettings::instance().setLastRoomId(m_roomId);
        m_gameStarted = true;
        applyPlayersFromJson(data.value(QStringLiteral("players")).toArray());
        setStatus(QStringLiteral("房间 %1，等待其他玩家...").arg(m_roomId));
        break;
    }
    case RoomProtocol::ResponseNewPlayerJoin: {
        const QJsonObject player = data.value(QStringLiteral("player")).toObject();
        RemotePlayer remote;
        remote.id = player.value(QStringLiteral("id")).toString();
        remote.name = player.value(QStringLiteral("name")).toString();
        remote.seat = player.value(QStringLiteral("seat")).toInt();
        remote.cardCount = 17;
        if (isLeftSeat(remote.seat)) {
            m_leftPlayer = remote;
            m_table->setLeftName(remote.name);
        } else {
            m_rightPlayer = remote;
            m_table->setRightName(remote.name);
        }
        updateOpponentCounts();
        setStatus(QStringLiteral("玩家 %1 加入房间").arg(remote.name));
        break;
    }
    case RoomProtocol::ResponseReady:
        setStatus(QStringLiteral("房间已满，准备开始..."));
        m_client->sendMessage(RoomProtocol::RequestBeReady,
                              {{QStringLiteral("roomId"), m_roomId},
                               {QStringLiteral("userId"), m_userId}});
        break;
    case RoomProtocol::ResponseDealPoker:
        m_playerCards = parsePokerArray(data.value(QStringLiteral("pokers")).toArray());
        m_playerCards = PokerUtils::sortDesc(m_playerCards);
        m_leftPlayer.cardCount = 17;
        m_rightPlayer.cardCount = 17;
        updateOpponentCounts();
        m_table->bottomCards()->setCards(QList<Poker>{}, CardHandWidget::LayoutMode::BottomCards, true);
        m_table->clearPlayAreas();
        m_table->playerHand()->setCards({}, CardHandWidget::LayoutMode::PlayerHand);
        m_dealing = true;
        m_dealIndex = 0;
        m_table->playerHand()->setSelectable(false);
        m_dealTimer->start();
        setStatus(QStringLiteral("发牌中..."));
        break;
    case RoomProtocol::ResponseToCallLandlord:
        handleBidPrompt(data);
        break;
    case RoomProtocol::ResponseLandlordAndLastCard:
        m_landlordSeat = data.value(QStringLiteral("landlordSeat")).toInt();
        m_activitySeat = m_landlordSeat;
        m_publicCards = parsePokerArray(data.value(QStringLiteral("publicPokers")).toArray());
        m_table->bottomCards()->setCards(m_publicCards, CardHandWidget::LayoutMode::BottomCards, false);
        hideButtons();
        if (RemotePlayer *landlord = playerBySeat(m_landlordSeat)) {
            landlord->cardCount = 20;
            if (landlord->seat != m_mySeat) {
                if (&m_leftPlayer == landlord)
                    m_rightPlayer.cardCount = 17;
                else
                    m_leftPlayer.cardCount = 17;
            }
        }
        if (m_mySeat == m_landlordSeat) {
            m_playerCards = PokerUtils::sortDesc(m_playerCards + m_publicCards);
            refreshPlayerHand();
            showButtons(GameConstants::ButtonsDiscard);
            setStatus(QStringLiteral("你是地主，请出牌"));
            startTurnCountdown();
        } else {
            setStatus(QStringLiteral("地主已确定，请等待出牌"));
            stopTurnCountdown();
        }
        updateLandlordUi();
        updateOpponentCounts();
        SoundManager::instance().playDeal();
        break;
    case RoomProtocol::ResponseDiscard: {
        const int seat = data.value(QStringLiteral("seat")).toInt();
        const int nextSeat = data.value(QStringLiteral("nextSeat")).toInt();
        const QJsonArray pokers = data.value(QStringLiteral("pokers")).toArray();
        m_nextSeat = nextSeat;

        if (seat == m_mySeat) {
            if (pokers.isEmpty()) {
                setStatus(QStringLiteral("您选择不出"));
            } else {
                m_activityCards = parsePokerArray(pokers);
                m_activitySeat = seat;
                m_playerCards = PokerUtils::removePokers(m_playerCards, m_activityCards);
                refreshPlayerHand();
                showPlayedCardsAnimated(seat, m_activityCards);
                SoundManager::instance().playDiscard();
            }
            m_selectedCards.clear();
            hideButtons();
            stopTurnCountdown();
        } else {
            if (RemotePlayer *player = playerBySeat(seat)) {
                if (pokers.isEmpty())
                    player->cardCount = qMax(0, player->cardCount);
                else
                    player->cardCount = qMax(0, player->cardCount - pokers.size());
            }

            const bool left = isLeftSeat(seat);
            if (pokers.isEmpty()) {
                if (left)
                    m_table->leftPassLabel()->setText(PokerUtils::randomPassText());
                else
                    m_table->rightPassLabel()->setText(PokerUtils::randomPassText());
            } else {
                m_activityCards = parsePokerArray(pokers);
                m_activitySeat = seat;
                if (left)
                    m_table->leftPassLabel()->clear();
                else
                    m_table->rightPassLabel()->clear();
                showPlayedCardsAnimated(seat, m_activityCards);
                SoundManager::instance().playDiscard();
            }
            updateOpponentCounts();
        }

        m_waitingForMyTurn = nextSeat == m_mySeat;
        if (nextSeat == m_mySeat) {
            m_table->playerPlayed()->setCards({});
            showButtons(GameConstants::ButtonsDiscard);
            setStatus(QStringLiteral("轮到您出牌"));
            startTurnCountdown();
        } else {
            stopTurnCountdown();
        }
        break;
    }
    case RoomProtocol::ResponseGameOver: {
        const QString victory = data.value(QStringLiteral("victory")).toString();
        const bool landlordWin = victory == QLatin1String(RoomProtocol::VictoryLandlord);
        const bool win = (landlordWin && m_mySeat == m_landlordSeat)
            || (!landlordWin && m_mySeat != m_landlordSeat);
        m_gameStarted = false;
        stopTurnCountdown();
        showResult(win);
        break;
    }
    default:
        break;
    }
}

void ClassicGameWidget::onDealTick()
{
    if (m_dealIndex >= m_playerCards.size()) {
        m_dealTimer->stop();
        m_dealing = false;
        refreshPlayerHand();
        setStatus(QStringLiteral("等待叫地主..."));
        return;
    }

    m_table->playerHand()->showDealingHand(m_playerCards, m_dealIndex + 1);
    ++m_dealIndex;
    SoundManager::instance().playDeal();
}

void ClassicGameWidget::refreshPlayerHand()
{
    m_table->playerHand()->syncHand(m_playerCards, CardHandWidget::LayoutMode::PlayerHand);
    m_table->playerHand()->setSelectable(true);
}

void ClassicGameWidget::showButtons(GameConstants::ButtonGroup group)
{
    GameplayHelper::showButtonGroup(
        m_table->buttonPanel(), group,
        [this]() { onBidPass(); },
        [this]() { onBidConfirm(); },
        [this]() { onHint(); },
        [this]() { onPlayCards(); },
        [this]() { onLeaveRoom(); },
        [this]() { onRestart(); });
}

void ClassicGameWidget::hideButtons()
{
    GameplayHelper::hideButtons(m_table->buttonPanel());
}

void ClassicGameWidget::setStatus(const QString &text)
{
    m_statusLabel->setText(text);
}

void ClassicGameWidget::updateLandlordUi()
{
    GameUiKit::updateLandlordBadge(m_table->selfBadge(), m_mySeat == m_landlordSeat);
    GameUiKit::updateLandlordBadge(m_table->leftBadge(), m_leftPlayer.seat == m_landlordSeat);
    GameUiKit::updateLandlordBadge(m_table->rightBadge(), m_rightPlayer.seat == m_landlordSeat);
}

void ClassicGameWidget::updateOpponentCounts()
{
    m_table->leftCountLabel()->setText(QString::number(m_leftPlayer.cardCount));
    m_table->rightCountLabel()->setText(QString::number(m_rightPlayer.cardCount));
}

void ClassicGameWidget::startTurnCountdown()
{
    m_countdown = 15;
    m_timerLabel->setText(QString::number(m_countdown));
    m_timerLabel->show();
    m_countdownTimer->start();
}

void ClassicGameWidget::stopTurnCountdown()
{
    m_countdownTimer->stop();
    m_timerLabel->hide();
    m_timerLabel->clear();
}

void ClassicGameWidget::onCountdownTick()
{
    if (m_countdown > 0)
        --m_countdown;
    m_timerLabel->setText(m_countdown > 0 ? QString::number(m_countdown) : QString());
}

void ClassicGameWidget::sendLandlordBid(const QString &action)
{
    m_client->sendMessage(RoomProtocol::RequestCallLandlord,
                          {{QStringLiteral("roomId"), m_roomId},
                           {QStringLiteral("userId"), m_userId},
                           {QStringLiteral("action"), action}});
}

void ClassicGameWidget::sendDiscard(const QList<Poker> &cards)
{
    QString ids;
    for (const Poker &card : cards)
        ids += QString::number(card.id()) + QLatin1Char(',');
    m_client->sendMessage(RoomProtocol::RequestDiscard,
                          {{QStringLiteral("roomId"), m_roomId},
                           {QStringLiteral("userId"), m_userId},
                           {QStringLiteral("pokers"), ids}});
}

void ClassicGameWidget::sendPass()
{
    m_client->sendMessage(RoomProtocol::RequestDiscard,
                          {{QStringLiteral("roomId"), m_roomId},
                           {QStringLiteral("userId"), m_userId}});
}

void ClassicGameWidget::onBidPass()
{
    const QString action = m_bidButtonGroup == GameConstants::ButtonsFightLandlord
        ? QString::fromLatin1(RoomProtocol::BidSkip)
        : QString::fromLatin1(RoomProtocol::BidPass);
    sendLandlordBid(action);
    hideButtons();
    SoundManager::instance().playClick();
}

void ClassicGameWidget::onBidConfirm()
{
    const QString action = m_bidButtonGroup == GameConstants::ButtonsFightLandlord
        ? QString::fromLatin1(RoomProtocol::BidGrab)
        : QString::fromLatin1(RoomProtocol::BidCall);
    sendLandlordBid(action);
    hideButtons();
    SoundManager::instance().playClick();
}

void ClassicGameWidget::onPlayCards()
{
    m_selectedCards = m_table->playerHand()->selectedCards();
    const bool landlordFirst = m_mySeat == m_landlordSeat && m_playerCards.size() == 20;
    const auto validation = GameplayHelper::validatePlay(
        m_selectedCards, m_playerCards, m_activityCards, m_activitySeat, m_mySeat, landlordFirst);
    if (!validation.ok) {
        setStatus(validation.message);
        return;
    }

    const QList<Poker> flying = PokerUtils::sortDesc(m_selectedCards);
    CardFlyAnimator::fly(this, flying, m_table->handCenterGlobal(), m_table->playerPlayedCenterGlobal(),
                         [this, flying]() {
                             m_table->playerPlayed()->setCards(flying, CardHandWidget::LayoutMode::PlayerPlayed);
                             BoomEffectAnimator::playIfNeeded(
                                 this, m_table->playerPlayedCenterGlobal(), flying, [this, flying]() {
                                     sendDiscard(flying);
                                 });
                         });
    SoundManager::instance().playClick();
}

void ClassicGameWidget::onPass()
{
    if (m_playerCards.size() == 20 || m_activitySeat == m_mySeat) {
        setStatus(QStringLiteral("当前不能不出"));
        return;
    }
    sendPass();
    hideButtons();
    SoundManager::instance().playClick();
}

void ClassicGameWidget::onHint()
{
    QList<Poker> hint;
    if (m_activitySeat == m_mySeat || m_activityCards.isEmpty())
        hint = PokerSeekUtils::randomPlay(m_playerCards);
    else
        hint = PokerSeekUtils::seekRight(m_playerCards, m_activityCards);
    if (hint.isEmpty()) {
        setStatus(QStringLiteral("没有可出的牌"));
        return;
    }
    m_table->playerHand()->setSelectedCards(hint);
    m_selectedCards = hint;
    setStatus(QStringLiteral("已为您选择一手牌"));
}

void ClassicGameWidget::onLeaveRoom()
{
    m_gameStarted = false;
    emit backToHallRequested();
}

void ClassicGameWidget::onRestart()
{
    emit restartRequested();
    m_client->disconnectFromServer();
    m_gameStarted = false;
    m_playerCards.clear();
    m_activityCards.clear();
    m_publicCards.clear();
    m_table->clearPlayAreas();
    m_table->bottomCards()->setCards({});
    hideButtons();
    stopTurnCountdown();
    connectServer();
}

void ClassicGameWidget::showResult(bool victory)
{
    hideButtons();
    if (victory)
        SoundManager::instance().playWin();
    else
        SoundManager::instance().playLose();

    auto *dialog = new GameResultDialog(
        victory,
        victory ? QStringLiteral("联网对战胜利！") : QStringLiteral("本局失利，再来一局吧。"),
        this);
    connect(dialog, &GameResultDialog::restartRequested, this, &ClassicGameWidget::onRestart);
    connect(dialog, &GameResultDialog::leaveRequested, this, &ClassicGameWidget::onLeaveRoom);
    dialog->open();
}

void ClassicGameWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    GameUiKit::paintBackground(this, painter, ResourceManager::instance().gameBackgroundPath());
}
