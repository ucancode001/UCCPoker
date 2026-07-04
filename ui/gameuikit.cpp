#include "gameuikit.h"
#include "core/gameconstants.h"
#include "core/resourcemanager.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QRadialGradient>
void GameUiKit::paintBackground(QWidget *widget, QPainter &painter, const QString &resourcePath)
{
    const QPixmap bg = ResourceManager::instance().pixmap(resourcePath);
    if (!bg.isNull()) {
        painter.drawPixmap(widget->rect(), bg.scaled(widget->size(), Qt::KeepAspectRatioByExpanding,
                                                     Qt::SmoothTransformation));
        painter.fillRect(widget->rect(), QColor(0, 0, 0, 72));
        paintTableOverlay(painter, widget->rect());
        return;
    }

    QRadialGradient gradient(widget->width() / 2.0, widget->height() / 2.0,
                             qMax(widget->width(), widget->height()) * 0.75);
    gradient.setColorAt(0, QColor(34, 110, 68));
    gradient.setColorAt(0.55, QColor(22, 78, 50));
    gradient.setColorAt(1, QColor(10, 42, 28));
    painter.fillRect(widget->rect(), gradient);
    paintTableOverlay(painter, widget->rect());
}

void GameUiKit::paintTableOverlay(QPainter &painter, const QRect &rect)
{
    QRadialGradient vignette(rect.center(), qMax(rect.width(), rect.height()) * 0.55);
    vignette.setColorAt(0, QColor(0, 0, 0, 0));
    vignette.setColorAt(1, QColor(0, 0, 0, 90));
    painter.fillRect(rect, vignette);
}

double GameUiKit::scaleFactorForWidget(const QWidget *widget)
{
    if (!widget)
        return 1.0;
    return qBound(0.75, widget->width() / static_cast<double>(GameConstants::StageWidth), 1.25);
}
QPushButton *GameUiKit::createImageButton(QWidget *parent, const QString &resourceName, const QString &fallbackText)
{
    auto *button = new QPushButton(parent);
    button->setCursor(Qt::PointingHandCursor);
    const QPixmap pix = ResourceManager::instance().pixmap(
        ResourceManager::instance().buttonPath(resourceName));
    if (!pix.isNull()) {
        button->setIcon(QIcon(pix));
        button->setIconSize(QSize(140, 90));
        button->setFixedSize(150, 96);
        button->setStyleSheet("QPushButton { border: none; background: transparent; }"
                              "QPushButton:hover { background: rgba(255,255,255,0.08); border-radius: 12px; }");
    } else {
        button->setText(fallbackText);
        button->setFixedSize(120, 44);
        button->setStyleSheet(
            "QPushButton { background: #EF6C00; color: white; border: none; border-radius: 22px;"
            " font-size: 16px; font-weight: bold; }");
    }
    return button;
}

QPushButton *GameUiKit::createFlatButton(QWidget *parent, const QString &text, const QString &color)
{
    auto *button = new QPushButton(text, parent);
    button->setFixedSize(120, 44);
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet(QString(
        "QPushButton { background: %1; color: white; border: none; border-radius: 22px;"
        " font-size: 16px; font-weight: bold; }"
        "QPushButton:hover { background: %2; }").arg(color, color));
    return button;
}

void GameUiKit::bindLandlordButtons(QWidget *panel, QHBoxLayout *layout, GameConstants::ButtonGroup group,
                                    const QObject *receiver, const char *passSlot, const char *confirmSlot)
{
    if (group == GameConstants::ButtonsCallLandlord) {
        auto *skipBtn = createImageButton(panel, QStringLiteral("button_nocall.png"), QStringLiteral("不叫"));
        auto *callBtn = createImageButton(panel, QStringLiteral("button_call.png"), QStringLiteral("叫地主"));
        layout->addWidget(skipBtn);
        layout->addWidget(callBtn);
        QObject::connect(skipBtn, SIGNAL(clicked()), receiver, passSlot);
        QObject::connect(callBtn, SIGNAL(clicked()), receiver, confirmSlot);
        return;
    }

    if (group == GameConstants::ButtonsFightLandlord) {
        auto *skipBtn = createImageButton(panel, QStringLiteral("button_pass.png"), QStringLiteral("不抢"));
        auto *grabBtn = createImageButton(panel, QStringLiteral("button_call.png"), QStringLiteral("抢地主"));
        layout->addWidget(skipBtn);
        layout->addWidget(grabBtn);
        QObject::connect(skipBtn, SIGNAL(clicked()), receiver, passSlot);
        QObject::connect(grabBtn, SIGNAL(clicked()), receiver, confirmSlot);
    }
}

QFrame *GameUiKit::createPlayerBadge(QWidget *parent, const QString &name, const QString &sex, int portraitIndex,
                                     bool isLandlord)
{
    auto *badge = new QFrame(parent);
    badge->setStyleSheet(
        "QFrame {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 rgba(255,255,255,0.14), stop:1 rgba(255,255,255,0.05));"
        "  border: 1px solid rgba(255,255,255,0.18);"
        "  border-radius: 16px;"
        "}");
    auto *layout = new QHBoxLayout(badge);
    layout->setContentsMargins(8, 4, 10, 4);
    layout->setSpacing(8);

    auto *avatar = new QLabel(badge);
    avatar->setFixedSize(40, 40);
    avatar->setScaledContents(true);
    const QPixmap portrait = ResourceManager::instance().pixmap(
        ResourceManager::instance().portraitPath(sex, portraitIndex));
    if (!portrait.isNull())
        avatar->setPixmap(portrait.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    layout->addWidget(avatar);

    auto *nameLabel = new QLabel(name, badge);
    nameLabel->setObjectName(QStringLiteral("playerName"));
    nameLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold;");
    layout->addWidget(nameLabel);

    if (isLandlord) {
        auto *tag = new QLabel(QStringLiteral("地主"), badge);
        tag->setObjectName(QStringLiteral("landlordTag"));
        tag->setStyleSheet("color: #FFD54F; font-size: 12px; font-weight: bold;");
        layout->addWidget(tag);
    }

    return badge;
}

void GameUiKit::updateLandlordBadge(QFrame *badge, bool isLandlord)
{
    if (!badge)
        return;
    if (QLabel *existing = badge->findChild<QLabel *>(QStringLiteral("landlordTag"))) {
        existing->setVisible(isLandlord);
        return;
    }
    if (isLandlord) {
        auto *tag = new QLabel(QStringLiteral("地主"), badge);
        tag->setObjectName(QStringLiteral("landlordTag"));
        tag->setStyleSheet("color: #FFD54F; font-size: 12px; font-weight: bold;");
        badge->layout()->addWidget(tag);
    }
}

QLabel *GameUiKit::createCountBadge(QWidget *parent)
{
    auto *label = new QLabel(parent);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(
        "color: #FFF8E1; background: rgba(0,0,0,0.55); border: 1px solid rgba(255,213,79,0.35);"
        " border-radius: 14px; padding: 4px 12px; font-weight: bold; font-size: 13px;");
    return label;
}

QLabel *GameUiKit::createTimerBadge(QWidget *parent)
{
    auto *label = new QLabel(parent);
    label->setFixedSize(52, 52);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(
        "color: white; background: qradialgradient(cx:0.5,cy:0.5,radius:0.9,"
        " stop:0 rgba(239,83,80,0.95), stop:1 rgba(183,28,28,0.95));"
        " border: 2px solid rgba(255,255,255,0.35); border-radius: 26px;"
        " font-size: 20px; font-weight: bold;");
    return label;
}

void GameUiKit::bindLandlordButtonsModern(QWidget *panel, QHBoxLayout *layout, GameConstants::ButtonGroup group,
                                          const std::function<void()> &onPass,
                                          const std::function<void()> &onConfirm)
{
    if (group == GameConstants::ButtonsCallLandlord) {
        auto *skipBtn = createImageButton(panel, QStringLiteral("button_nocall.png"), QStringLiteral("不叫"));
        auto *callBtn = createImageButton(panel, QStringLiteral("button_call.png"), QStringLiteral("叫地主"));
        layout->addWidget(skipBtn);
        layout->addWidget(callBtn);
        QObject::connect(skipBtn, &QPushButton::clicked, skipBtn, onPass);
        QObject::connect(callBtn, &QPushButton::clicked, callBtn, onConfirm);
        return;
    }

    if (group == GameConstants::ButtonsFightLandlord) {
        auto *skipBtn = createImageButton(panel, QStringLiteral("button_pass.png"), QStringLiteral("不抢"));
        auto *grabBtn = createImageButton(panel, QStringLiteral("button_call.png"), QStringLiteral("抢地主"));
        layout->addWidget(skipBtn);
        layout->addWidget(grabBtn);
        QObject::connect(skipBtn, &QPushButton::clicked, skipBtn, onPass);
        QObject::connect(grabBtn, &QPushButton::clicked, grabBtn, onConfirm);
    }
}