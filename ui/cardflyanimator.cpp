#include "cardflyanimator.h"
#include "cardwidget.h"
#include "core/resourcemanager.h"

#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QEasingCurve>

CardFlyAnimator::CardFlyAnimator(QWidget *overlayParent)
    : QObject(overlayParent)
    , m_parent(overlayParent)
{
}

void CardFlyAnimator::fly(QWidget *overlayParent,
                          const QList<Poker> &cards,
                          const QPointF &fromGlobal,
                          const QPointF &toGlobal,
                          const std::function<void()> &finished)
{
    if (!overlayParent || cards.isEmpty()) {
        if (finished)
            finished();
        return;
    }

    auto *animator = new CardFlyAnimator(overlayParent);
    animator->flyCards(cards, fromGlobal, toGlobal, finished);
}

void CardFlyAnimator::flyCards(const QList<Poker> &cards,
                               const QPointF &fromGlobal,
                               const QPointF &toGlobal,
                               const std::function<void()> &finished)
{
    if (!m_parent || cards.isEmpty()) {
        if (finished)
            finished();
        deleteLater();
        return;
    }

    const QSize cardSize = CardWidget::cardSize();
    const QPointF start = m_parent->mapFromGlobal(fromGlobal.toPoint());
    const QPointF end = m_parent->mapFromGlobal(toGlobal.toPoint());
    const int spread = qMin(120, 12 * (cards.size() - 1));
    const int duration = qBound(280, 220 + cards.size() * 18, 520);

    auto *group = new QParallelAnimationGroup(this);
    connect(group, &QParallelAnimationGroup::finished, this, [this, finished]() {
        if (finished)
            finished();
        deleteLater();
    });

    for (int i = 0; i < cards.size(); ++i) {
        const Poker &card = cards.at(i);
        auto *ghost = new QLabel(m_parent);
        ghost->setAttribute(Qt::WA_TransparentForMouseEvents);
        ghost->setFixedSize(cardSize);
        ghost->setPixmap(ResourceManager::instance().cardPixmap(card.id()).scaled(
            cardSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ghost->setStyleSheet("background: transparent;");

        auto *opacity = new QGraphicsOpacityEffect(ghost);
        opacity->setOpacity(0.95);
        ghost->setGraphicsEffect(opacity);

        const qreal offset = i - (cards.size() - 1) / 2.0;
        const QPointF startPos(start.x() + offset * 18 - cardSize.width() / 2.0,
                               start.y() - cardSize.height() / 2.0);
        const QPointF endPos(end.x() + offset * 14 - cardSize.width() / 2.0,
                             end.y() - cardSize.height() / 2.0 - 8);
        ghost->move(startPos.toPoint());
        ghost->show();
        ghost->raise();

        auto *moveAnim = new QPropertyAnimation(ghost, "pos", group);
        moveAnim->setDuration(duration);
        moveAnim->setStartValue(startPos.toPoint());
        moveAnim->setEndValue(endPos.toPoint());
        moveAnim->setEasingCurve(QEasingCurve::OutCubic);

        auto *fadeAnim = new QPropertyAnimation(opacity, "opacity", group);
        fadeAnim->setDuration(duration);
        fadeAnim->setStartValue(0.95);
        fadeAnim->setEndValue(1.0);

        connect(moveAnim, &QPropertyAnimation::finished, ghost, &QLabel::deleteLater);
    }

    group->start(QAbstractAnimation::DeleteWhenStopped);
}
