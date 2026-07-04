#ifndef POKERTYPEUTILS_H
#define POKERTYPEUTILS_H

#include "poker.h"
#include "pokertype.h"

#include <QList>
#include <QString>

class PokerTypeUtils
{
public:
    static const QString TypeBoom;
    static const QString TypeKingBoom;
    static const QString TypeSingle;
    static const QString TypePair;
    static const QString TypeThree;
    static const QString TypeThreeSingle;
    static const QString TypeThreePair;
    static const QString TypeStraight;
    static const QString TypeStraightPairs;
    static const QString TypePlane;
    static const QString TypePlane2Single;
    static const QString TypePlane2Pairs;
    static const QString TypeFour2Single;
    static const QString TypeFour2Pairs;

    static PokerType getType(QList<Poker> pokers);

    static int isBoom(const QList<Poker> &array);
    static int isKingBoom(const QList<Poker> &array);
    static int isSingle(const QList<Poker> &array);
    static int isPair(const QList<Poker> &array);
    static int isStraight(QList<Poker> array);
    static int isThree(const QList<Poker> &array);
    static int isThreeSingle(const QList<Poker> &array);
    static int isThreePairs(const QList<Poker> &array);
    static int isStraightPairs(const QList<Poker> &array);
    static int isPlane(const QList<Poker> &array);
    static int isPlane2Single(const QList<Poker> &array);
    static int isPlane2Pairs(const QList<Poker> &array);
    static int isFour2Single(const QList<Poker> &array);
    static int isFour2Pairs(const QList<Poker> &array);
};

#endif // POKERTYPEUTILS_H
