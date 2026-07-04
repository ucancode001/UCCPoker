#include "resourcemanager.h"
#include "pokerutils.h"

#include <QPainter>
#include <QPainterPath>

ResourceManager &ResourceManager::instance()
{
    static ResourceManager manager;
    return manager;
}

ResourceManager::ResourceManager() = default;

QString ResourceManager::portraitPath(const QString &sex, int index) const
{
    const int safeIndex = qBound(1, index, 10);
    if (sex == QStringLiteral("lady"))
        return QStringLiteral(":/image/defaultPortrait/lady/lady_%1.png").arg(safeIndex);
    return QStringLiteral(":/image/defaultPortrait/man/man_%1.png").arg(safeIndex);
}

bool ResourceManager::hasPixmap(const QString &path) const
{
    return !pixmap(path).isNull();
}

QPixmap ResourceManager::pixmap(const QString &path) const
{
    if (m_pathCache.contains(path))
        return m_pathCache.value(path);

    QPixmap loaded(path);
    m_pathCache.insert(path, loaded);
    return loaded;
}

QPixmap ResourceManager::scaled(const QPixmap &source, const QSize &size) const
{
    if (source.isNull())
        return {};
    return source.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QPixmap ResourceManager::loadOrGenerateCard(int cardId) const
{
    const QString path = QStringLiteral(":/image/puker/%1.png").arg(cardId);
    QPixmap loaded = pixmap(path);
    if (!loaded.isNull())
        return loaded;

    QPixmap fallback(140, 190);
    fallback.fill(Qt::transparent);
    QPainter painter(&fallback);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath pathShape;
    pathShape.addRoundedRect(4, 4, 132, 182, 12, 12);
    painter.fillPath(pathShape, QColor(250, 250, 252));
    painter.setPen(QPen(QColor(40, 40, 40), 2));
    painter.drawPath(pathShape);

    const bool red = PokerUtils::isRedSuit(cardId);
    painter.setPen(red ? QColor(200, 30, 30) : QColor(20, 20, 20));
    painter.setFont(QFont(QStringLiteral("Microsoft YaHei"), 18, QFont::Bold));
    const QString rank = PokerUtils::rankLabel(PokerUtils::orderValueForCardId(cardId));
    const QString suit = PokerUtils::suitSymbol(cardId);
    painter.drawText(QRect(12, 10, 60, 30), Qt::AlignLeft | Qt::AlignTop, rank);
    painter.drawText(QRect(12, 36, 60, 30), Qt::AlignLeft | Qt::AlignTop, suit);
    painter.setFont(QFont(QStringLiteral("Segoe UI Symbol"), 36));
    painter.drawText(fallback.rect(), Qt::AlignCenter, suit);
    return fallback;
}

QPixmap ResourceManager::cardPixmap(int cardId) const
{
    if (m_cardCache.contains(cardId))
        return m_cardCache.value(cardId);

    const QPixmap pix = loadOrGenerateCard(cardId);
    m_cardCache.insert(cardId, pix);
    return pix;
}

QPixmap ResourceManager::cardBackPixmap() const
{
    QPixmap loaded = pixmap(QStringLiteral(":/image/puker/back.png"));
    if (!loaded.isNull())
        return loaded;

    QPixmap fallback(140, 190);
    fallback.fill(Qt::transparent);
    QPainter painter(&fallback);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath pathShape;
    pathShape.addRoundedRect(4, 4, 132, 182, 12, 12);
    painter.fillPath(pathShape, QColor(25, 72, 128));
    painter.setPen(QPen(QColor(180, 200, 255), 2));
    painter.drawPath(pathShape);
    painter.setPen(Qt::white);
    painter.setFont(QFont(QStringLiteral("Microsoft YaHei"), 24, QFont::Bold));
    painter.drawText(fallback.rect(), Qt::AlignCenter, QStringLiteral("UCC"));
    return fallback;
}
