#include "gameresultdialog.h"
#include "ui/gameuikit.h"

#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

GameResultDialog::GameResultDialog(bool victory, const QString &subtitle, QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    setupUi(victory, subtitle);
}

void GameResultDialog::setupUi(bool victory, const QString &subtitle)
{
    auto *frame = new QFrame(this);
    frame->setObjectName(QStringLiteral("resultFrame"));
    frame->setStyleSheet(
        "#resultFrame {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 rgba(30,40,55,0.96), stop:1 rgba(18,24,36,0.98));"
        "  border: 2px solid rgba(255,213,79,0.55);"
        "  border-radius: 24px;"
        "}");

    auto *shadow = new QGraphicsDropShadowEffect(frame);
    shadow->setBlurRadius(32);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0, 0, 0, 160));
    frame->setGraphicsEffect(shadow);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->addWidget(frame, 0, Qt::AlignCenter);

    auto *inner = new QVBoxLayout(frame);
    inner->setContentsMargins(48, 40, 48, 36);
    inner->setSpacing(16);

    auto *title = new QLabel(victory ? QStringLiteral("胜利") : QStringLiteral("失败"), frame);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QString(
        "color: %1; font-size: 42px; font-weight: bold; letter-spacing: 4px;")
                             .arg(victory ? QStringLiteral("#FFD54F") : QStringLiteral("#90A4AE")));

    auto *sub = new QLabel(subtitle, frame);
    sub->setAlignment(Qt::AlignCenter);
    sub->setWordWrap(true);
    sub->setStyleSheet("color: #ECEFF1; font-size: 16px;");

    inner->addWidget(title);
    inner->addWidget(sub);

    auto *buttons = new QHBoxLayout;
    buttons->setSpacing(16);
    auto *restartBtn = GameUiKit::createFlatButton(frame, QStringLiteral("再来一局"), "#EF6C00");
    auto *leaveBtn = GameUiKit::createFlatButton(frame, QStringLiteral("离开房间"), "#546E7A");
    buttons->addWidget(restartBtn);
    buttons->addWidget(leaveBtn);
    inner->addLayout(buttons);

    connect(restartBtn, &QPushButton::clicked, this, [this]() {
        emit restartRequested();
        accept();
    });
    connect(leaveBtn, &QPushButton::clicked, this, [this]() {
        emit leaveRequested();
        reject();
    });

    setFixedSize(480, 280);
}
