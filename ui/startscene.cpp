#include "startscene.h"
#include "core/resourcemanager.h"
#include "ui/gameuikit.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

StartScene::StartScene(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->addStretch(2);

    auto *title = new QLabel(QStringLiteral("UCC 斗地主"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "color: #FFD54F; font-size: 56px; font-weight: bold; "
        "font-family: 'Microsoft YaHei'; letter-spacing: 6px;"
        "background: rgba(0,0,0,0.25); border-radius: 18px; padding: 12px 24px;");
    layout->addWidget(title, 0, Qt::AlignHCenter);

    auto *subtitle = new QLabel(QStringLiteral("经典单机 · 联网对战 · 专业牌局"), this);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("color: rgba(255,255,255,0.9); font-size: 20px; margin-top: 8px;");
    layout->addWidget(subtitle);

    layout->addStretch(1);

    auto *buttonRow = new QHBoxLayout;
    buttonRow->addStretch();
    m_startButton = GameUiKit::createImageButton(this, QStringLiteral("button_begin.png"),
                                                 QStringLiteral("开始游戏"));
    connect(m_startButton, &QPushButton::clicked, this, &StartScene::startRequested);
    buttonRow->addWidget(m_startButton);

    auto *settingsButton = GameUiKit::createFlatButton(this, QStringLiteral("设置"), "#546E7A");
    connect(settingsButton, &QPushButton::clicked, this, &StartScene::settingsRequested);
    buttonRow->addWidget(settingsButton);
    buttonRow->addStretch();
    layout->addLayout(buttonRow);
    layout->addStretch(2);
}

void StartScene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    GameUiKit::paintBackground(this, painter, ResourceManager::instance().startBackgroundPath());
}

void StartScene::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}
