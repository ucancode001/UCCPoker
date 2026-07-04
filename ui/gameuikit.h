#ifndef GAMEUIKIT_H
#define GAMEUIKIT_H

#include "core/gameconstants.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QWidget>
#include <functional>

class QFrame;
class QLabel;
class QPushButton;

namespace GameUiKit {
void paintBackground(QWidget *widget, QPainter &painter, const QString &resourcePath);
QPushButton *createImageButton(QWidget *parent, const QString &resourceName, const QString &fallbackText);
QPushButton *createFlatButton(QWidget *parent, const QString &text, const QString &color);
void bindLandlordButtons(QWidget *panel, QHBoxLayout *layout, GameConstants::ButtonGroup group,
                         const QObject *receiver,
                         const char *passSlot, const char *confirmSlot);
void bindLandlordButtonsModern(QWidget *panel, QHBoxLayout *layout, GameConstants::ButtonGroup group,
                               const std::function<void()> &onPass,
                               const std::function<void()> &onConfirm);
QFrame *createPlayerBadge(QWidget *parent, const QString &name, const QString &sex, int portraitIndex,
                          bool isLandlord);
void updateLandlordBadge(QFrame *badge, bool isLandlord);
QLabel *createCountBadge(QWidget *parent);
QLabel *createTimerBadge(QWidget *parent);
void paintTableOverlay(QPainter &painter, const QRect &rect);
double scaleFactorForWidget(const QWidget *widget);
}

#endif // GAMEUIKIT_H
