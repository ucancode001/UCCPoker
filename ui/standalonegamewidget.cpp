#include "standalonegamewidget.h"
#include "cardhandwidget.h"
#include "core/pokerseekutils.h"
#include "core/appsettings.h"
#include "core/resourcemanager.h"
#include "core/soundmanager.h"
#include "ui/gameplayhelper.h"
#include "ui/gameresultdialog.h"
#include "ui/gametableview.h"
#include "ui/gameuikit.h"
#include "ui/cardflyanimator.h"
#include "ui/boomeffectanimator.h"
#include "ui/cardlayout.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QRandomGenerator>
#include <QTimer>
#include <QVBoxLayout>

StandaloneGameWidget::StandaloneGameWidget(QWidget *parent)
    : QWidget(parent)
{
    m_playerName = AppSettings::instance().playerName();
    m_leftPlayer = PokerUtils::randomUser();
    m_rightPlayer = PokerUtils::randomUser();

    m_dealTimer = new QTimer(this);
    m_dealTimer->setInterval(CardLayout::DealTickMs);
    connect(m_dealTimer, &QTimer::timeout, this, &StandaloneGameWidget::onDealTick);

    m_autoTimer = new QTimer(this);
    m_autoTimer->setSingleShot(true);
    connect(m_autoTimer, &QTimer::timeout, this, &StandaloneGameWidget::onAutoPlayTick);

    m_bidTimer = new QTimer(this);
    m_bidTimer->setSingleShot(true);
    connect(m_bidTimer, &QTimer::timeout, this, &StandaloneGameWidget::onBidAiTick);

    m_countdownTimer = new QTimer(this);
    m_countdownTimer->setInterval(1000);
    connect(m_countdownTimer, &QTimer::timeout, this, [this]() {
        if (m_countdown > 0)
            --m_countdown;
        if (m_timerLabel)
            m_timerLabel->setText(m_countdown > 0 ? QString::number(m_countdown) : QString());
    });

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 12, 16, 12);
    root->setSpacing(8);

    auto *topBar = new QHBoxLayout;
    auto *backButton = GameUiKit::createFlatButton(this, QStringLiteral("返回大厅"), "#546E7A");
    connect(backButton, &QPushButton::clicked, this, &StandaloneGameWidget::backToHallRequested);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: #FFE082; font-size: 16px; font-weight: bold;");

    m_timerLabel = GameUiKit::createTimerBadge(this);

    topBar->addWidget(backButton);
    topBar->addStretch();
    topBar->addWidget(m_statusLabel);
    topBar->addStretch();
    topBar->addWidget(m_timerLabel);
    root->addLayout(topBar);

    m_table = new GameTableView(this);
    m_table->setOpponentCountsVisible(true);
    m_table->setSelfName(m_playerName);
    m_table->setLeftName(m_leftPlayer.name);
    m_table->setRightName(m_rightPlayer.name);
    m_table->playerHand()->setSelectable(true);
    connect(m_table->playerHand(), &CardHandWidget::selectionChanged, this, [this]() {
        m_selectedCards = m_table->playerHand()->selectedCards();
    });
    root->addWidget(m_table, 1);

    SoundManager::instance().startBackgroundMusic();
    resetGame();
}

void StandaloneGameWidget::resetGame()
{
    m_phase = PhaseDealing;
    m_dealIndex = 0;
    m_landlordSeat = 0;
    m_playingSeat = 0;
    m_playedSeat = 0;
    m_playedCards.clear();
    m_selectedCards.clear();
    m_gameFinished = false;

    m_leftPlayer = PokerUtils::randomUser();
    m_rightPlayer = PokerUtils::randomUser();
    m_table->setLeftName(m_leftPlayer.name);
    m_table->setRightName(m_rightPlayer.name);

    m_deck = PokerUtils::randomDeck();
    m_playerCards = PokerUtils::sortDesc(m_deck.mid(0, 17));
    m_rightCards = PokerUtils::sortDesc(m_deck.mid(17, 17));
    m_leftCards = PokerUtils::sortDesc(m_deck.mid(34, 17));
    m_bottomCardsData = m_deck.mid(51, 3);

    m_table->playerHand()->setCards({}, CardHandWidget::LayoutMode::PlayerHand);
    m_table->bottomCards()->setCards(m_bottomCardsData, CardHandWidget::LayoutMode::BottomCards, true);
    m_table->clearPlayAreas();
    hideButtons();
    updateLandlordBadges();
    m_statusLabel->setText(QStringLiteral("发牌中..."));
    startDealing();
}

void StandaloneGameWidget::startDealing()
{
    m_dealIndex = 0;
    m_table->playerHand()->setSelectable(false);
    m_dealTimer->start();
}

void StandaloneGameWidget::onDealTick()
{
    if (m_dealIndex >= 17) {
        m_dealTimer->stop();
        finishDealing();
        return;
    }

    m_table->playerHand()->showDealingHand(m_playerCards, m_dealIndex + 1);
    m_table->leftCountLabel()->setText(QString::number(m_dealIndex + 1));
    m_table->rightCountLabel()->setText(QString::number(m_dealIndex + 1));
    ++m_dealIndex;
    SoundManager::instance().playDeal();
}

void StandaloneGameWidget::finishDealing()
{
    m_table->playerHand()->syncHand(m_playerCards, CardHandWidget::LayoutMode::PlayerHand);
    m_table->playerHand()->setSelectable(true);
    m_table->leftCountLabel()->setText(QStringLiteral("17"));
    m_table->rightCountLabel()->setText(QStringLiteral("17"));
    startBidding();
}

void StandaloneGameWidget::startBidding()
{
    m_phase = PhaseBidLandlord;
    m_bidEngine.reset(m_mySeat);
    m_statusLabel->setText(QStringLiteral("叫地主阶段"));
    processBidTurn();
}

void StandaloneGameWidget::processBidTurn()
{
    if (m_bidEngine.isFinished()) {
        if (m_bidEngine.state() == LandlordBidEngine::State::Redeal) {
            m_statusLabel->setText(QStringLiteral("无人叫地主，重新发牌"));
            QTimer::singleShot(1200, this, [this]() { resetGame(); });
            return;
        }
        finishBidding(m_bidEngine.landlordSeat());
        return;
    }

    const int seat = m_bidEngine.activeSeat();
    if (seat == m_mySeat) {
        showButtonGroup(m_bidEngine.buttonGroup());
        m_statusLabel->setText(m_bidEngine.buttonGroup() == GameConstants::ButtonsFightLandlord
                                   ? QStringLiteral("是否抢地主？")
                                   : QStringLiteral("是否叫地主？"));
        return;
    }

    hideButtons();
    m_bidTimer->start(600 + QRandomGenerator::global()->bounded(800));
}

void StandaloneGameWidget::onBidAiTick()
{
    const int seat = m_bidEngine.activeSeat();
    const QList<Poker> hand = seat == GameConstants::SeatRight ? m_rightCards : m_leftCards;
    const LandlordBidEngine::Action action = m_bidEngine.aiAction(hand);
    m_bidEngine.applyAction(action);

    const QString name = seat == GameConstants::SeatRight ? m_rightPlayer.name : m_leftPlayer.name;
    if (action == LandlordBidEngine::Action::Call)
        m_statusLabel->setText(QStringLiteral("%1 叫地主").arg(name));
    else if (action == LandlordBidEngine::Action::Grab)
        m_statusLabel->setText(QStringLiteral("%1 抢地主").arg(name));
    else
        m_statusLabel->setText(QStringLiteral("%1 不叫").arg(name));

    processBidTurn();
}

void StandaloneGameWidget::onBidPass()
{
    m_bidEngine.applyAction(LandlordBidEngine::Action::Pass);
    hideButtons();
    SoundManager::instance().playClick();
    processBidTurn();
}

void StandaloneGameWidget::onBidConfirm()
{
    const LandlordBidEngine::Action action =
        m_bidEngine.buttonGroup() == GameConstants::ButtonsFightLandlord
            ? LandlordBidEngine::Action::Grab
            : LandlordBidEngine::Action::Call;
    m_bidEngine.applyAction(action);
    hideButtons();
    SoundManager::instance().playClick();
    processBidTurn();
}

void StandaloneGameWidget::finishBidding(int landlordSeat)
{
    m_landlordSeat = landlordSeat;
    m_playingSeat = landlordSeat;
    m_playedSeat = 0;
    m_playedCards.clear();
    m_phase = PhasePlaying;

    if (landlordSeat == m_mySeat) {
        m_playerCards = PokerUtils::sortDesc(m_playerCards + m_bottomCardsData);
        refreshPlayerHand();
        m_statusLabel->setText(QStringLiteral("你是地主，请出牌"));
        showButtonGroup(GameConstants::ButtonsDiscard);
    } else if (landlordSeat == GameConstants::SeatRight) {
        m_rightCards = PokerUtils::sortDesc(m_rightCards + m_bottomCardsData);
        m_table->rightCountLabel()->setText(QStringLiteral("20"));
        m_statusLabel->setText(QStringLiteral("%1 成为地主").arg(m_rightPlayer.name));
        scheduleAutoPlay();
    } else {
        m_leftCards = PokerUtils::sortDesc(m_leftCards + m_bottomCardsData);
        m_table->leftCountLabel()->setText(QStringLiteral("20"));
        m_statusLabel->setText(QStringLiteral("%1 成为地主").arg(m_leftPlayer.name));
        scheduleAutoPlay();
    }
    refreshBottomCards();
    updateLandlordBadges();
}

void StandaloneGameWidget::updateLandlordBadges()
{
    GameUiKit::updateLandlordBadge(m_table->selfBadge(), m_mySeat == m_landlordSeat);
    GameUiKit::updateLandlordBadge(m_table->leftBadge(), GameConstants::SeatLeft == m_landlordSeat);
    GameUiKit::updateLandlordBadge(m_table->rightBadge(), GameConstants::SeatRight == m_landlordSeat);
}

void StandaloneGameWidget::refreshPlayerHand()
{
    m_table->playerHand()->syncHand(m_playerCards, CardHandWidget::LayoutMode::PlayerHand);
}

void StandaloneGameWidget::refreshBottomCards()
{
    m_table->bottomCards()->setCards(m_bottomCardsData, CardHandWidget::LayoutMode::BottomCards, false);
}

void StandaloneGameWidget::showButtonGroup(GameConstants::ButtonGroup group)
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

void StandaloneGameWidget::hideButtons()
{
    GameplayHelper::hideButtons(m_table->buttonPanel());
}

void StandaloneGameWidget::onPlayCards()
{
    m_selectedCards = m_table->playerHand()->selectedCards();
    const bool landlordFirst = m_landlordSeat == m_mySeat && m_playerCards.size() == 20;
    const auto validation = GameplayHelper::validatePlay(
        m_selectedCards, m_playerCards, m_playedCards, m_playedSeat, m_mySeat, landlordFirst);
    if (!validation.ok) {
        m_statusLabel->setText(validation.message);
        return;
    }

    m_playerCards = PokerUtils::removePokers(m_playerCards, m_selectedCards);
    m_playedCards = PokerUtils::sortDesc(m_selectedCards);
    m_playedSeat = m_mySeat;
    m_playingSeat = GameConstants::SeatRight;

    refreshPlayerHand();
    m_table->playerHand()->clearSelection();
    hideButtons();
    clearOtherPlayAreas();
    SoundManager::instance().playDiscard();

    const QList<Poker> flying = m_playedCards;
    CardFlyAnimator::fly(this, flying, m_table->handCenterGlobal(), m_table->playerPlayedCenterGlobal(),
                         [this, flying]() {
                             m_table->playerPlayed()->setCards(flying, CardHandWidget::LayoutMode::PlayerPlayed);
                             BoomEffectAnimator::playIfNeeded(
                                 this, m_table->playerPlayedCenterGlobal(), flying, [this]() {
                                     if (checkGameOver())
                                         return;
                                     m_statusLabel->setText(QStringLiteral("等待其他玩家..."));
                                     scheduleAutoPlay();
                                 });
                         });
}

void StandaloneGameWidget::onPass()
{
    if (m_landlordSeat == m_mySeat && m_playerCards.size() == 20) {
        m_statusLabel->setText(QStringLiteral("地主首张必须出牌"));
        return;
    }
    if (m_playedSeat == m_mySeat || m_playedCards.isEmpty()) {
        m_statusLabel->setText(QStringLiteral("当前不能不出"));
        return;
    }

    hideButtons();
    m_playingSeat = GameConstants::SeatRight;
    m_statusLabel->setText(QStringLiteral("您选择不出"));
    scheduleAutoPlay();
}

void StandaloneGameWidget::onHint()
{
    QList<Poker> hint;
    if (m_playedSeat == m_mySeat || m_playedCards.isEmpty())
        hint = PokerSeekUtils::randomPlay(m_playerCards);
    else
        hint = PokerSeekUtils::seekRight(m_playerCards, m_playedCards);

    if (hint.isEmpty()) {
        m_statusLabel->setText(QStringLiteral("没有可出的牌"));
        return;
    }

    m_table->playerHand()->setSelectedCards(hint);
    m_selectedCards = hint;
    m_statusLabel->setText(QStringLiteral("已为您选择一手牌"));
}

void StandaloneGameWidget::clearOtherPlayAreas()
{
    m_table->leftPlayed()->setCards({});
    m_table->rightPlayed()->setCards({});
    m_table->clearPassTips();
}

void StandaloneGameWidget::showPassTip(int seat)
{
    const QString text = PokerUtils::randomPassText();
    if (seat == GameConstants::SeatLeft)
        m_table->leftPassLabel()->setText(text);
    else
        m_table->rightPassLabel()->setText(text);
}

void StandaloneGameWidget::scheduleAutoPlay()
{
    m_countdown = 15;
    m_timerLabel->setText(QString::number(m_countdown));
    m_countdownTimer->start();
    const int delay = (QRandomGenerator::global()->bounded(4) + 1) * 500;
    m_autoTimer->start(delay);
}

void StandaloneGameWidget::onAutoPlayTick()
{
    m_countdownTimer->stop();
    m_timerLabel->clear();
    executeAutoPlay();
}

void StandaloneGameWidget::executeAutoPlay()
{
    if (m_phase != PhasePlaying || m_gameFinished)
        return;

    if (m_playingSeat == GameConstants::SeatRight) {
        const QList<Poker> play = PokerSeekUtils::autoPlay(
            m_rightCards, GameConstants::SeatRight, m_landlordSeat, m_playedCards, m_playedSeat);

        m_table->rightPassLabel()->clear();
        if (play.isEmpty()) {
            showPassTip(GameConstants::SeatRight);
            if (checkGameOver())
                return;
            m_playingSeat = GameConstants::SeatLeft;
            scheduleAutoPlay();
        } else {
            const QList<Poker> sorted = PokerUtils::sortDesc(play);
            m_rightCards = PokerUtils::removePokers(m_rightCards, play);
            m_playedCards = play;
            m_playedSeat = GameConstants::SeatRight;
            m_table->rightCountLabel()->setText(QString::number(m_rightCards.size()));
            SoundManager::instance().playDiscard();
            CardFlyAnimator::fly(this, sorted,
                                 m_table->rightBadge()->mapToGlobal(m_table->rightBadge()->rect().center()),
                                 m_table->rightPlayedCenterGlobal(),
                                 [this, sorted]() {
                                     m_table->rightPlayed()->setCards(sorted, CardHandWidget::LayoutMode::OpponentRight);
                                     BoomEffectAnimator::playIfNeeded(
                                         this, m_table->rightPlayedCenterGlobal(), sorted, [this]() {
                                             if (checkGameOver())
                                                 return;
                                             m_playingSeat = GameConstants::SeatLeft;
                                             scheduleAutoPlay();
                                         });
                                 });
        }
    } else if (m_playingSeat == GameConstants::SeatLeft) {
        const QList<Poker> play = PokerSeekUtils::autoPlay(
            m_leftCards, GameConstants::SeatLeft, m_landlordSeat, m_playedCards, m_playedSeat);

        m_table->leftPassLabel()->clear();
        if (play.isEmpty()) {
            showPassTip(GameConstants::SeatLeft);
            if (checkGameOver())
                return;
            m_playingSeat = m_mySeat;
            m_table->playerPlayed()->setCards({});
            m_statusLabel->setText(QStringLiteral("轮到您出牌"));
            showButtonGroup(GameConstants::ButtonsDiscard);
        } else {
            const QList<Poker> sorted = PokerUtils::sortDesc(play);
            m_leftCards = PokerUtils::removePokers(m_leftCards, play);
            m_playedCards = play;
            m_playedSeat = GameConstants::SeatLeft;
            m_table->leftCountLabel()->setText(QString::number(m_leftCards.size()));
            SoundManager::instance().playDiscard();
            CardFlyAnimator::fly(this, sorted,
                                 m_table->leftBadge()->mapToGlobal(m_table->leftBadge()->rect().center()),
                                 m_table->leftPlayedCenterGlobal(),
                                 [this, sorted]() {
                                     m_table->leftPlayed()->setCards(sorted, CardHandWidget::LayoutMode::OpponentLeft);
                                     BoomEffectAnimator::playIfNeeded(
                                         this, m_table->leftPlayedCenterGlobal(), sorted, [this]() {
                                             if (checkGameOver())
                                                 return;
                                             m_playingSeat = m_mySeat;
                                             m_table->playerPlayed()->setCards({});
                                             m_statusLabel->setText(QStringLiteral("轮到您出牌"));
                                             showButtonGroup(GameConstants::ButtonsDiscard);
                                         });
                                 });
        }
    }
}

bool StandaloneGameWidget::checkGameOver()
{
    if (m_playerCards.isEmpty()) {
        showGameResult(true);
        return true;
    }
    if (m_rightCards.isEmpty()) {
        showGameResult(m_landlordSeat != GameConstants::SeatLeft);
        return true;
    }
    if (m_leftCards.isEmpty()) {
        showGameResult(m_landlordSeat != GameConstants::SeatRight);
        return true;
    }
    return false;
}

void StandaloneGameWidget::showGameResult(bool victory)
{
    m_phase = PhaseFinished;
    m_gameFinished = true;
    m_playerVictory = victory;
    m_dealTimer->stop();
    m_autoTimer->stop();
    m_countdownTimer->stop();
    m_timerLabel->clear();
    hideButtons();

    if (victory)
        SoundManager::instance().playWin();
    else
        SoundManager::instance().playLose();

    auto *dialog = new GameResultDialog(
        victory,
        victory ? QStringLiteral("你赢得了本局斗地主！") : QStringLiteral("再接再厉，下局一定能赢。"),
        this);
    connect(dialog, &GameResultDialog::restartRequested, this, [this]() {
        emit restartRequested();
        resetGame();
    });
    connect(dialog, &GameResultDialog::leaveRequested, this, &StandaloneGameWidget::backToHallRequested);
    dialog->open();
}

void StandaloneGameWidget::onLeaveRoom()
{
    emit backToHallRequested();
}

void StandaloneGameWidget::onRestart()
{
    emit restartRequested();
    resetGame();
}

void StandaloneGameWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    GameUiKit::paintBackground(this, painter, ResourceManager::instance().gameBackgroundPath());
}
