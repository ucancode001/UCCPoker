#include "boomeffectanimator.h"
#include "core/pokertypeutils.h"
#include "core/soundmanager.h"

#include <QEasingCurve>
#include <QPainter>
#include <QtMath>
#include <QVariantAnimation>
#include <QWidget>

namespace {

class BoomFlashWidget : public QWidget
{
public:
    explicit BoomFlashWidget(QWidget *parent)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_TranslucentBackground);
    }

    void start(BoomEffectAnimator::Kind kind, const QPointF &centerLocal,
               const std::function<void()> &finished)
    {
        m_kind = kind;
        m_center = centerLocal;
        m_finished = finished;

        auto *anim = new QVariantAnimation(this);
        anim->setDuration(m_kind == BoomEffectAnimator::Kind::KingBomb ? 1100 : 850);
        anim->setStartValue(0.0);
        anim->setEndValue(1.0);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
            m_progress = value.toReal();
            update();
        });
        connect(anim, &QVariantAnimation::finished, this, [this]() {
            if (m_finished)
                m_finished();
            deleteLater();
        });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        show();
        raise();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const bool king = m_kind == BoomEffectAnimator::Kind::KingBomb;
        const QColor flashColor = king ? QColor(255, 215, 64) : QColor(255, 120, 48);
        const QColor ringColor = king ? QColor(255, 64, 129) : QColor(255, 87, 34);
        const QColor ringColor2 = king ? QColor(255, 193, 7) : QColor(255, 152, 0);

        const qreal flashAlpha = qBound(0.0, (0.35 - m_progress) * 2.2, 0.35);
        if (flashAlpha > 0.01) {
            QRadialGradient flash(m_center, qMax(width(), height()) * 0.65);
            flash.setColorAt(0, QColor(flashColor.red(), flashColor.green(), flashColor.blue(), int(255 * flashAlpha)));
            flash.setColorAt(0.45, QColor(flashColor.red(), flashColor.green(), flashColor.blue(), int(180 * flashAlpha)));
            flash.setColorAt(1, QColor(0, 0, 0, 0));
            painter.fillRect(rect(), flash);
        }

        const qreal maxRadius = qMax(width(), height()) * 0.42;
        const qreal ringRadius = maxRadius * m_progress;
        const int ringAlpha = int(220 * (1.0 - m_progress * 0.85));
        if (ringAlpha > 0) {
            QPen pen(ringColor);
            pen.setWidth(king ? 10 : 8);
            pen.setColor(QColor(ringColor.red(), ringColor.green(), ringColor.blue(), ringAlpha));
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(m_center, ringRadius, ringRadius);

            QPen pen2(ringColor2);
            pen2.setWidth(king ? 6 : 4);
            pen2.setColor(QColor(ringColor2.red(), ringColor2.green(), ringColor2.blue(), ringAlpha / 2));
            painter.setPen(pen2);
            painter.drawEllipse(m_center, ringRadius * 0.72, ringRadius * 0.72);
        }

        if (m_progress > 0.08 && m_progress < 0.72) {
            const qreal textT = qBound(0.0, (m_progress - 0.08) / 0.28, 1.0);
            const qreal textScale = 0.55 + textT * 0.45;
            const int textAlpha = int(255 * qBound(0.0, 1.0 - (m_progress - 0.35) * 1.8, 1.0));

            painter.save();
            painter.translate(m_center);
            painter.scale(textScale, textScale);

            QFont font(QStringLiteral("Microsoft YaHei"), king ? 54 : 46, QFont::Black);
            painter.setFont(font);

            const QString text = king ? QStringLiteral("王炸!") : QStringLiteral("炸弹!");
            QFontMetrics fm(font);
            const QRect textRect = fm.boundingRect(text);

            painter.setPen(QColor(0, 0, 0, textAlpha / 2));
            painter.drawText(textRect.translated(3, 3), Qt::AlignCenter, text);

            QLinearGradient textGradient(-textRect.width() / 2, 0, textRect.width() / 2, 0);
            if (king) {
                textGradient.setColorAt(0, QColor(255, 235, 59, textAlpha));
                textGradient.setColorAt(0.5, QColor(255, 255, 255, textAlpha));
                textGradient.setColorAt(1, QColor(255, 64, 129, textAlpha));
            } else {
                textGradient.setColorAt(0, QColor(255, 224, 130, textAlpha));
                textGradient.setColorAt(1, QColor(255, 87, 34, textAlpha));
            }
            painter.setPen(QPen(textGradient, 2));
            painter.drawText(textRect, Qt::AlignCenter, text);
            painter.restore();
        }

        if (king && m_progress < 0.55) {
            const int sparkAlpha = int(180 * (1.0 - m_progress / 0.55));
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(255, 255, 255, sparkAlpha));
            for (int i = 0; i < 8; ++i) {
                const qreal angle = i * 45.0 + m_progress * 120.0;
                const qreal dist = ringRadius * 0.35;
                const QPointF p(m_center.x() + qCos(qDegreesToRadians(angle)) * dist,
                                m_center.y() + qSin(qDegreesToRadians(angle)) * dist);
                painter.drawEllipse(p, 5, 5);
            }
        }
    }

private:
    BoomEffectAnimator::Kind m_kind = BoomEffectAnimator::Kind::Bomb;
    QPointF m_center;
    qreal m_progress = 0.0;
    std::function<void()> m_finished;
};

} // namespace

BoomEffectAnimator::BoomEffectAnimator(QWidget *overlayParent)
    : QObject(overlayParent)
    , m_parent(overlayParent)
{
}

void BoomEffectAnimator::play(QWidget *overlayParent,
                              const QPointF &centerGlobal,
                              Kind kind,
                              const std::function<void()> &finished)
{
    if (!overlayParent) {
        if (finished)
            finished();
        return;
    }

    auto *animator = new BoomEffectAnimator(overlayParent);
    animator->play(centerGlobal, kind, finished);
}

void BoomEffectAnimator::playIfNeeded(QWidget *overlayParent,
                                      const QPointF &centerGlobal,
                                      const QList<Poker> &cards,
                                      const std::function<void()> &finished)
{
    const PokerType type = PokerTypeUtils::getType(cards);
    if (type.type() == PokerTypeUtils::TypeKingBoom) {
        BoomEffectAnimator::play(overlayParent, centerGlobal, Kind::KingBomb, finished);
        return;
    }
    if (type.type() == PokerTypeUtils::TypeBoom) {
        BoomEffectAnimator::play(overlayParent, centerGlobal, Kind::Bomb, finished);
        return;
    }
    if (finished)
        finished();
}

void BoomEffectAnimator::play(const QPointF &centerGlobal, Kind kind,
                              const std::function<void()> &finished)
{
    if (!m_parent) {
        if (finished)
            finished();
        deleteLater();
        return;
    }

    auto *overlay = new QWidget(m_parent);
    overlay->setGeometry(m_parent->rect());
    overlay->setAttribute(Qt::WA_TransparentForMouseEvents);
    overlay->show();
    overlay->raise();

    if (kind == Kind::KingBomb)
        SoundManager::instance().playKingBoom();
    else
        SoundManager::instance().playBomb();

    const QPointF centerLocal = overlay->mapFromGlobal(centerGlobal.toPoint());
    auto *effect = new BoomFlashWidget(overlay);
    effect->setGeometry(overlay->rect());
    effect->start(kind, centerLocal, [overlay, finished]() {
        overlay->deleteLater();
        if (finished)
            finished();
    });

    connect(m_parent, &QWidget::destroyed, overlay, &QWidget::deleteLater);
    deleteLater();
}
