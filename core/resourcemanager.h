#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QHash>
#include <QPixmap>
#include <QString>

class ResourceManager
{
public:
    static ResourceManager &instance();

    QPixmap cardPixmap(int cardId) const;
    QPixmap cardBackPixmap() const;
    QPixmap pixmap(const QString &path) const;
    bool hasPixmap(const QString &path) const;

    QString startBackgroundPath() const { return QStringLiteral(":/image/bgImg/startBgImg.png"); }
    QString gameBackgroundPath() const { return QStringLiteral(":/image/bgImg/gameBgImg.png"); }
    QString hallBackgroundPath() const { return QStringLiteral(":/image/assets/bg.jpg"); }
    QString buttonPath(const QString &name) const { return QStringLiteral(":/image/button/%1").arg(name); }
    QString portraitPath(const QString &sex, int index) const;

private:
    ResourceManager();

    QPixmap loadOrGenerateCard(int cardId) const;
    QPixmap scaled(const QPixmap &source, const QSize &size) const;

    mutable QHash<int, QPixmap> m_cardCache;
    mutable QHash<QString, QPixmap> m_pathCache;
};

#endif // RESOURCEMANAGER_H
