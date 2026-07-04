#ifndef POKERSEEKUTILS_H
#define POKERSEEKUTILS_H

#include "poker.h"

#include <QList>
#include <QMap>

class PokerSeekUtils
{
public:
    static QList<Poker> seekRight(QList<Poker> hand, QList<Poker> target);
    static QList<Poker> randomPlay(const QList<Poker> &hand);
    static QList<Poker> autoPlay(const QList<Poker> &hand, int mySeat, int landlordSeat,
                                 const QList<Poker> &played, int playedSeat);

private:
    static QList<Poker> seekSingle(const QList<Poker> &hand, int typeSort);
    static QList<Poker> seekPairs(const QList<Poker> &hand, int typeSort);
    static QList<Poker> seekThree(const QList<Poker> &hand, int typeSort);
    static QList<Poker> seekThreeSingle(const QList<Poker> &hand, int typeSort);
    static QList<Poker> seekThreePair(const QList<Poker> &hand, int typeSort);
    static QList<Poker> seekStraight(const QList<Poker> &hand, int typeSort, int length);
    static QList<Poker> seekStraightPairs(const QList<Poker> &hand, int typeSort, int length);
    static QList<Poker> seekPlane(const QList<Poker> &hand, int typeSort);
    static QList<Poker> seekPlane2Single(const QList<Poker> &hand, int typeSort);
    static QList<Poker> seekPlane2Pairs(const QList<Poker> &hand, int typeSort);
    static QList<Poker> seekFour2Single(const QList<Poker> &hand, int typeSort);
    static QList<Poker> seekFour2Pairs(const QList<Poker> &hand, int typeSort);
    static QList<Poker> seekBoom(const QList<Poker> &hand, int typeSort);
    static QList<Poker> seekKingBoom(const QList<Poker> &hand);

    static QMap<int, QList<Poker>> groupByOrder(const QList<Poker> &hand);
    static QList<Poker> pickSingles(const QList<Poker> &hand, int count, const QList<Poker> &exclude);
    static QList<Poker> pickPairs(const QList<Poker> &hand, int pairCount, const QList<Poker> &exclude);
    static QList<Poker> findConsecutiveTripletRun(const QList<Poker> &hand, int runLength, int minSort);
};

#endif // POKERSEEKUTILS_H
