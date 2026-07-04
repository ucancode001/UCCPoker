#include "gametableview.h"
#include "cardhandwidget.h"
#include "ui/gameuikit.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

GameTableView::GameTableView(QWidget *parent)
    : QWidget(parent)
{
    auto *table = new QGridLayout(this);
    table->setContentsMargins(0, 0, 0, 0);
    table->setRowStretch(0, 2);
    table->setRowStretch(1, 1);
    table->setRowStretch(2, 3);
    table->setColumnStretch(0, 1);
    table->setColumnStretch(1, 2);
    table->setColumnStretch(2, 1);

    auto *leftPanel = new QVBoxLayout;
    m_leftBadge = GameUiKit::createPlayerBadge(this, QStringLiteral("上家"), QStringLiteral("man"), 3, false);
    leftPanel->addWidget(m_leftBadge, 0, Qt::AlignHCenter);
    m_leftCountLabel = GameUiKit::createCountBadge(this);
    leftPanel->addWidget(m_leftCountLabel, 0, Qt::AlignHCenter);
    m_leftPlayed = new CardHandWidget(this);
    m_leftPassLabel = new QLabel(this);
    m_leftPassLabel->setAlignment(Qt::AlignCenter);
    m_leftPassLabel->setStyleSheet("color: #FFECB3; font-size: 18px; font-weight: bold;");
    leftPanel->addStretch();
    leftPanel->addWidget(m_leftPlayed, 0, Qt::AlignHCenter);
    leftPanel->addWidget(m_leftPassLabel, 0, Qt::AlignHCenter);
    table->addLayout(leftPanel, 0, 0, 3, 1);

    auto *center = new QVBoxLayout;
    m_bottomCards = new CardHandWidget(this);
    center->addWidget(m_bottomCards, 0, Qt::AlignHCenter);
    m_playerPlayed = new CardHandWidget(this);
    center->addStretch();
    center->addWidget(m_playerPlayed, 0, Qt::AlignHCenter);
    table->addLayout(center, 1, 1);

    auto *rightPanel = new QVBoxLayout;
    m_rightBadge = GameUiKit::createPlayerBadge(this, QStringLiteral("下家"), QStringLiteral("lady"), 5, false);
    rightPanel->addWidget(m_rightBadge, 0, Qt::AlignHCenter);
    m_rightCountLabel = GameUiKit::createCountBadge(this);
    rightPanel->addWidget(m_rightCountLabel, 0, Qt::AlignHCenter);
    m_rightPlayed = new CardHandWidget(this);
    m_rightPassLabel = new QLabel(this);
    m_rightPassLabel->setAlignment(Qt::AlignCenter);
    m_rightPassLabel->setStyleSheet(m_leftPassLabel->styleSheet());
    rightPanel->addStretch();
    rightPanel->addWidget(m_rightPlayed, 0, Qt::AlignHCenter);
    rightPanel->addWidget(m_rightPassLabel, 0, Qt::AlignHCenter);
    table->addLayout(rightPanel, 0, 2, 3, 1);

    auto *bottom = new QVBoxLayout;
    m_selfBadge = GameUiKit::createPlayerBadge(this, QStringLiteral("我"), QStringLiteral("man"), 1, false);
    bottom->addWidget(m_selfBadge, 0, Qt::AlignLeft);
    m_playerHand = new CardHandWidget(this);
    bottom->addWidget(m_playerHand, 0, Qt::AlignHCenter);
    m_buttonPanel = new QWidget(this);
    new QHBoxLayout(m_buttonPanel);
    bottom->addWidget(m_buttonPanel, 0, Qt::AlignHCenter);
    table->addLayout(bottom, 2, 0, 1, 3);
}

void GameTableView::setSelfName(const QString &name)
{
    if (QLabel *label = m_selfBadge->findChild<QLabel *>(QStringLiteral("playerName")))
        label->setText(name);
}

void GameTableView::setLeftName(const QString &name)
{
    if (QLabel *label = m_leftBadge->findChild<QLabel *>(QStringLiteral("playerName")))
        label->setText(name);
}

void GameTableView::setRightName(const QString &name)
{
    if (QLabel *label = m_rightBadge->findChild<QLabel *>(QStringLiteral("playerName")))
        label->setText(name);
}

void GameTableView::setOpponentCountsVisible(bool visible)
{
    m_leftCountLabel->setVisible(visible);
    m_rightCountLabel->setVisible(visible);
}

void GameTableView::clearPlayAreas()
{
    m_leftPlayed->setCards({});
    m_rightPlayed->setCards({});
    m_playerPlayed->setCards({});
    clearPassTips();
}

void GameTableView::clearPassTips()
{
    m_leftPassLabel->clear();
    m_rightPassLabel->clear();
}

QPointF GameTableView::handCenterGlobal() const
{
    return m_playerHand->mapToGlobal(m_playerHand->rect().center());
}

QPointF GameTableView::playerPlayedCenterGlobal() const
{
    return m_playerPlayed->mapToGlobal(m_playerPlayed->rect().center());
}

QPointF GameTableView::leftPlayedCenterGlobal() const
{
    return m_leftPlayed->mapToGlobal(m_leftPlayed->rect().center());
}

QPointF GameTableView::rightPlayedCenterGlobal() const
{
    return m_rightPlayed->mapToGlobal(m_rightPlayed->rect().center());
}

QPointF GameTableView::seatCenterGlobal(int seat, int mySeat) const
{
    Q_UNUSED(mySeat)
    if (seat == 1)
        return handCenterGlobal();
    if (m_leftBadge && seat == 3)
        return m_leftBadge->mapToGlobal(m_leftBadge->rect().center());
    if (m_rightBadge)
        return m_rightBadge->mapToGlobal(m_rightBadge->rect().center());
    return rect().center();
}
