#include "gamehall.h"
#include "core/resourcemanager.h"
#include "ui/gameuikit.h"

#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

GameHall::GameHall(QWidget *parent)
    : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(32, 24, 32, 24);

    auto *topBar = new QHBoxLayout;
    auto *backButton = GameUiKit::createFlatButton(this, QStringLiteral("← 返回"), "#607D8B");
    connect(backButton, &QPushButton::clicked, this, &GameHall::backToStartRequested);

    auto *settingsButton = GameUiKit::createFlatButton(this, QStringLiteral("设置"), "#546E7A");
    connect(settingsButton, &QPushButton::clicked, this, &GameHall::settingsRequested);

    auto *hallTitle = new QLabel(QStringLiteral("游戏大厅"), this);
    hallTitle->setStyleSheet("color: #FFE082; font-size: 28px; font-weight: bold;");

    topBar->addWidget(backButton);
    topBar->addStretch();
    topBar->addWidget(hallTitle);
    topBar->addStretch();
    topBar->addWidget(settingsButton);
    root->addLayout(topBar);

    auto *modeRow = new QHBoxLayout;
    modeRow->setSpacing(24);
    modeRow->addStretch();

    auto *offlineCard = createModeCard(
        QStringLiteral("单机模式"),
        QStringLiteral("与 AI 对战，完整斗地主体验"),
        QStringLiteral("menu/a.png"),
        QStringLiteral("#43A047"),
        true);
    connect(offlineCard->findChild<QPushButton *>(), &QPushButton::clicked, this, &GameHall::offlineModeRequested);
    modeRow->addWidget(offlineCard);

    auto *classicCard = createModeCard(
        QStringLiteral("经典模式"),
        QStringLiteral("联网匹配，与真实玩家对战"),
        QStringLiteral("menu/b.png"),
        QStringLiteral("#1976D2"),
        true);
    connect(classicCard->findChild<QPushButton *>(), &QPushButton::clicked, this, &GameHall::classicModeRequested);
    modeRow->addWidget(classicCard);

    auto *friendCard = createModeCard(
        QStringLiteral("好友模式"),
        QStringLiteral("创建或加入好友房间"),
        QStringLiteral("menu/menu_friendMode.png"),
        QStringLiteral("#8E24AA"),
        true);
    connect(friendCard->findChild<QPushButton *>(), &QPushButton::clicked, this, &GameHall::friendModeRequested);
    modeRow->addWidget(friendCard);
    modeRow->addStretch();

    root->addStretch();
    root->addLayout(modeRow);
    root->addStretch();
}

QFrame *GameHall::createModeCard(const QString &title, const QString &subtitle, const QString &iconResource,
                                 const QString &accent, bool enabled)
{
    auto *frame = new QFrame(this);
    frame->setFixedSize(280, 360);
    frame->setCursor(enabled ? Qt::PointingHandCursor : Qt::ArrowCursor);
    frame->setStyleSheet(QString(
        "QFrame {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 rgba(30,40,55,0.92), stop:1 rgba(18,24,36,0.96));"
        "  border-radius: 20px;"
        "  border: 1px solid rgba(255,255,255,0.12);"
        "  border-top: 4px solid %1;"
        "}"
        "QLabel#title { color: #FFF8E1; font-size: 24px; font-weight: bold; }"
        "QLabel#subtitle { color: #B0BEC5; font-size: 14px; }").arg(accent));

    auto *shadow = new QGraphicsDropShadowEffect(frame);
    shadow->setBlurRadius(24);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0, 0, 0, 80));
    frame->setGraphicsEffect(shadow);

    auto *layout = new QVBoxLayout(frame);
    layout->setContentsMargins(24, 32, 24, 24);

    auto *icon = new QLabel(frame);
    icon->setFixedHeight(120);
    icon->setAlignment(Qt::AlignCenter);
    const QPixmap iconPix = ResourceManager::instance().pixmap(
        QStringLiteral(":/image/%1").arg(iconResource));
    if (!iconPix.isNull())
        icon->setPixmap(iconPix.scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else
        icon->setText(QStringLiteral("🃏"));

    auto *titleLabel = new QLabel(title, frame);
    titleLabel->setObjectName(QStringLiteral("title"));
    auto *subtitleLabel = new QLabel(subtitle, frame);
    subtitleLabel->setObjectName(QStringLiteral("subtitle"));
    subtitleLabel->setWordWrap(true);

    layout->addWidget(icon);
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addStretch();

    auto *enterButton = new QPushButton(enabled ? QStringLiteral("进入游戏") : QStringLiteral("敬请期待"), frame);
    enterButton->setEnabled(enabled);
    enterButton->setStyleSheet(QString(
        "QPushButton { background: %1; color: white; border: none; border-radius: 20px;"
        " padding: 10px; font-size: 16px; font-weight: bold; }"
        "QPushButton:disabled { background: #B0BEC5; }").arg(accent));
    layout->addWidget(enterButton);
    return frame;
}

void GameHall::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    GameUiKit::paintBackground(this, painter, ResourceManager::instance().hallBackgroundPath());
}
