#include "pokerseekutils.h"
#include "pokerutils.h"
#include "pokertypeutils.h"

#include <algorithm>

QMap<int, QList<Poker>> PokerSeekUtils::groupByOrder(const QList<Poker> &hand)
{
    QMap<int, QList<Poker>> groups;
    for (const Poker &card : hand)
        groups[card.orderValue()].append(card);
    return groups;
}

QList<Poker> PokerSeekUtils::pickSingles(const QList<Poker> &hand, int count, const QList<Poker> &exclude)
{
    QList<Poker> result;
    QList<Poker> sorted = PokerUtils::sortDesc(hand);
    for (int i = sorted.size() - 1; i >= 0 && result.size() < count; --i) {
        bool skip = false;
        for (const Poker &ex : exclude) {
            if (sorted.at(i).id() == ex.id()) {
                skip = true;
                break;
            }
        }
        if (!skip)
            result.append(sorted.at(i));
    }
    return result.size() == count ? result : QList<Poker>{};
}

QList<Poker> PokerSeekUtils::pickPairs(const QList<Poker> &hand, int pairCount, const QList<Poker> &exclude)
{
    QList<Poker> result;
    const QMap<int, QList<Poker>> groups = groupByOrder(hand);
    QList<int> keys = groups.keys();
    std::sort(keys.begin(), keys.end());

    for (int key : keys) {
        if (groups.value(key).size() < 2)
            continue;
        bool used = false;
        for (const Poker &ex : exclude) {
            if (ex.orderValue() == key) {
                used = true;
                break;
            }
        }
        if (used)
            continue;
        result.append(groups.value(key).at(0));
        result.append(groups.value(key).at(1));
        if (result.size() / 2 >= pairCount)
            return result;
    }
    return {};
}

QList<Poker> PokerSeekUtils::findConsecutiveTripletRun(const QList<Poker> &hand, int runLength, int minSort)
{
    QList<int> tripletValues;
    const QMap<int, QList<Poker>> groups = groupByOrder(hand);
    for (auto it = groups.constBegin(); it != groups.constEnd(); ++it) {
        if (it.value().size() >= 3 && it.key() <= PokerUtils::AValue)
            tripletValues.append(it.key());
    }
    std::sort(tripletValues.begin(), tripletValues.end());

    for (int start = 0; start <= tripletValues.size() - runLength; ++start) {
        bool consecutive = true;
        for (int i = 1; i < runLength; ++i) {
            if (tripletValues.at(start + i) != tripletValues.at(start + i - 1) + 1) {
                consecutive = false;
                break;
            }
        }
        if (!consecutive)
            continue;
        if (tripletValues.at(start + runLength - 1) <= minSort)
            continue;

        QList<Poker> result;
        for (int i = 0; i < runLength; ++i) {
            const int value = tripletValues.at(start + i);
            const QList<Poker> cards = groups.value(value);
            result.append(cards.at(0));
            result.append(cards.at(1));
            result.append(cards.at(2));
        }
        return result;
    }
    return {};
}

QList<Poker> PokerSeekUtils::seekRight(QList<Poker> hand, QList<Poker> target)
{
    QList<Poker> myPuker = PokerUtils::sortDesc(hand);
    QList<Poker> puker = PokerUtils::sortDesc(target);
    PokerType bType = PokerTypeUtils::getType(puker);
    if (!bType.isValid())
        return {};

    QList<Poker> rightPuker;
    if (bType.type() == PokerTypeUtils::TypeKingBoom) {
        rightPuker = {};
    } else if (bType.type() == PokerTypeUtils::TypeBoom) {
        rightPuker = seekBoom(myPuker, bType.sort());
    } else if (bType.type() == PokerTypeUtils::TypeSingle) {
        rightPuker = seekSingle(myPuker, bType.sort());
    } else if (bType.type() == PokerTypeUtils::TypePair) {
        rightPuker = seekPairs(myPuker, bType.sort());
    } else if (bType.type() == PokerTypeUtils::TypeThree) {
        rightPuker = seekThree(myPuker, bType.sort());
    } else if (bType.type() == PokerTypeUtils::TypeThreeSingle) {
        rightPuker = seekThreeSingle(myPuker, bType.sort());
    } else if (bType.type() == PokerTypeUtils::TypeThreePair) {
        rightPuker = seekThreePair(myPuker, bType.sort());
    } else if (bType.type() == PokerTypeUtils::TypeStraight) {
        rightPuker = seekStraight(myPuker, bType.sort(), puker.size());
    } else if (bType.type() == PokerTypeUtils::TypeStraightPairs) {
        rightPuker = seekStraightPairs(myPuker, bType.sort(), puker.size());
    } else if (bType.type() == PokerTypeUtils::TypePlane) {
        rightPuker = seekPlane(myPuker, bType.sort());
    } else if (bType.type() == PokerTypeUtils::TypePlane2Single) {
        rightPuker = seekPlane2Single(myPuker, bType.sort());
    } else if (bType.type() == PokerTypeUtils::TypePlane2Pairs) {
        rightPuker = seekPlane2Pairs(myPuker, bType.sort());
    } else if (bType.type() == PokerTypeUtils::TypeFour2Single) {
        rightPuker = seekFour2Single(myPuker, bType.sort());
    } else if (bType.type() == PokerTypeUtils::TypeFour2Pairs) {
        rightPuker = seekFour2Pairs(myPuker, bType.sort());
    }

    if (!rightPuker.isEmpty())
        return rightPuker;

    rightPuker = seekBoom(myPuker, -1);
    if (!rightPuker.isEmpty())
        return rightPuker;

    return seekKingBoom(myPuker);
}

QList<Poker> PokerSeekUtils::seekSingle(const QList<Poker> &hand, int typeSort)
{
    for (int j = hand.size() - 1; j >= 0; --j) {
        if (hand.at(j).orderValue() > typeSort)
            return {hand.at(j)};
    }
    return {};
}

QList<Poker> PokerSeekUtils::seekPairs(const QList<Poker> &hand, int typeSort)
{
    for (int j = hand.size() - 1; j >= 1; --j) {
        if (hand.at(j).orderValue() == hand.at(j - 1).orderValue()
            && hand.at(j).orderValue() > typeSort) {
            return {hand.at(j), hand.at(j - 1)};
        }
    }
    return {};
}

QList<Poker> PokerSeekUtils::seekThree(const QList<Poker> &hand, int typeSort)
{
    for (int j = hand.size() - 1; j >= 2; --j) {
        if (hand.at(j).orderValue() == hand.at(j - 1).orderValue()
            && hand.at(j).orderValue() == hand.at(j - 2).orderValue()
            && hand.at(j).orderValue() > typeSort) {
            return {hand.at(j), hand.at(j - 1), hand.at(j - 2)};
        }
    }
    return {};
}

QList<Poker> PokerSeekUtils::seekThreeSingle(const QList<Poker> &hand, int typeSort)
{
    int index = 0;
    QList<Poker> result;
    for (int j = hand.size() - 1; j >= 2; --j) {
        if (hand.at(j).orderValue() == hand.at(j - 1).orderValue()
            && hand.at(j).orderValue() == hand.at(j - 2).orderValue()
            && hand.at(j).orderValue() > typeSort) {
            index = j;
            result = {hand.at(j), hand.at(j - 1), hand.at(j - 2)};
            break;
        }
    }
    if (index == 0)
        return {};

    const QList<Poker> singles = pickSingles(hand, 1, result);
    if (singles.isEmpty())
        return {};
    result.append(singles);
    return result;
}

QList<Poker> PokerSeekUtils::seekThreePair(const QList<Poker> &hand, int typeSort)
{
    int index = 0;
    QList<Poker> result;
    for (int j = hand.size() - 1; j >= 2; --j) {
        if (hand.at(j).orderValue() == hand.at(j - 1).orderValue()
            && hand.at(j).orderValue() == hand.at(j - 2).orderValue()
            && hand.at(j).orderValue() > typeSort) {
            index = j;
            result = {hand.at(j), hand.at(j - 1), hand.at(j - 2)};
            break;
        }
    }
    if (index == 0)
        return {};

    const QList<Poker> pairs = pickPairs(hand, 1, result);
    if (pairs.size() != 2)
        return {};
    result.append(pairs);
    return result;
}

QList<Poker> PokerSeekUtils::seekStraight(const QList<Poker> &hand, int typeSort, int length)
{
    if (typeSort == PokerUtils::AValue)
        return {};

    for (int j = hand.size() - 1; j >= 0; --j) {
        if (hand.at(j).orderValue() <= typeSort - length + 1)
            continue;

        QList<Poker> result;
        Poker current = hand.at(j);
        result.append(current);

        for (int k = j - 1; k >= 0; --k) {
            const int indexK = hand.at(k).orderValue();
            const int indexPuker = current.orderValue();
            if (indexK > PokerUtils::AValue)
                return {};
            if (j < length)
                return {};

            if (indexK == indexPuker)
                continue;
            if (indexK - 1 == indexPuker) {
                current = hand.at(k);
                result.append(current);
                if (result.size() == length)
                    return result;
            } else if (indexK - 1 > indexPuker) {
                break;
            }
        }
    }
    return {};
}

QList<Poker> PokerSeekUtils::seekStraightPairs(const QList<Poker> &hand, int typeSort, int length)
{
    if (length < 6 || length % 2 != 0)
        return {};

    const int pairCount = length / 2;
    QList<int> pairValues;
    for (int j = hand.size() - 1; j >= 1; --j) {
        if (hand.at(j).orderValue() == hand.at(j - 1).orderValue()) {
            const int value = hand.at(j).orderValue();
            if (value <= PokerUtils::AValue && !pairValues.contains(value))
                pairValues.append(value);
            --j;
        }
    }
    std::sort(pairValues.begin(), pairValues.end());

    for (int start = 0; start <= pairValues.size() - pairCount; ++start) {
        bool consecutive = true;
        for (int i = 1; i < pairCount; ++i) {
            if (pairValues.at(start + i) != pairValues.at(start + i - 1) + 1) {
                consecutive = false;
                break;
            }
        }
        if (!consecutive)
            continue;
        if (pairValues.at(start + pairCount - 1) <= typeSort - pairCount + 1)
            continue;

        QList<Poker> result;
        for (int i = 0; i < pairCount; ++i) {
            const int value = pairValues.at(start + i);
            int found = 0;
            for (int j = hand.size() - 1; j >= 1 && found < 2; --j) {
                if (hand.at(j).orderValue() == value) {
                    result.append(hand.at(j));
                    result.append(hand.at(j - 1));
                    found = 2;
                }
            }
        }
        if (result.size() == length && PokerTypeUtils::getType(result).isValid())
            return result;
    }
    return {};
}

QList<Poker> PokerSeekUtils::seekPlane(const QList<Poker> &hand, int typeSort)
{
    const QList<Poker> triplets = findConsecutiveTripletRun(hand, 2, typeSort);
    return triplets.size() == 6 ? triplets : QList<Poker>{};
}

QList<Poker> PokerSeekUtils::seekPlane2Single(const QList<Poker> &hand, int typeSort)
{
    const QList<Poker> triplets = findConsecutiveTripletRun(hand, 2, typeSort);
    if (triplets.isEmpty())
        return {};
    const QList<Poker> singles = pickSingles(hand, 2, triplets);
    if (singles.size() != 2)
        return {};
    QList<Poker> result = triplets;
    result.append(singles);
    return PokerTypeUtils::getType(result).isValid() ? result : QList<Poker>{};
}

QList<Poker> PokerSeekUtils::seekPlane2Pairs(const QList<Poker> &hand, int typeSort)
{
    const QList<Poker> triplets = findConsecutiveTripletRun(hand, 2, typeSort);
    if (triplets.isEmpty())
        return {};
    const QList<Poker> pairs = pickPairs(hand, 2, triplets);
    if (pairs.size() != 4)
        return {};
    QList<Poker> result = triplets;
    result.append(pairs);
    return PokerTypeUtils::getType(result).isValid() ? result : QList<Poker>{};
}

QList<Poker> PokerSeekUtils::seekFour2Single(const QList<Poker> &hand, int typeSort)
{
    for (int j = hand.size() - 1; j >= 3; --j) {
        if (hand.at(j).orderValue() == hand.at(j - 1).orderValue()
            && hand.at(j).orderValue() == hand.at(j - 2).orderValue()
            && hand.at(j).orderValue() == hand.at(j - 3).orderValue()
            && hand.at(j).orderValue() > typeSort) {
            QList<Poker> bomb = {hand.at(j), hand.at(j - 1), hand.at(j - 2), hand.at(j - 3)};
            const QList<Poker> singles = pickSingles(hand, 2, bomb);
            if (singles.size() == 2) {
                bomb.append(singles);
                return bomb;
            }
        }
    }
    return {};
}

QList<Poker> PokerSeekUtils::seekFour2Pairs(const QList<Poker> &hand, int typeSort)
{
    for (int j = hand.size() - 1; j >= 3; --j) {
        if (hand.at(j).orderValue() == hand.at(j - 1).orderValue()
            && hand.at(j).orderValue() == hand.at(j - 2).orderValue()
            && hand.at(j).orderValue() == hand.at(j - 3).orderValue()
            && hand.at(j).orderValue() > typeSort) {
            QList<Poker> bomb = {hand.at(j), hand.at(j - 1), hand.at(j - 2), hand.at(j - 3)};
            const QList<Poker> pairs = pickPairs(hand, 2, bomb);
            if (pairs.size() == 4) {
                bomb.append(pairs);
                return bomb;
            }
        }
    }
    return {};
}

QList<Poker> PokerSeekUtils::seekBoom(const QList<Poker> &hand, int typeSort)
{
    for (int j = hand.size() - 1; j >= 3; --j) {
        if (hand.at(j).orderValue() == hand.at(j - 1).orderValue()
            && hand.at(j).orderValue() == hand.at(j - 2).orderValue()
            && hand.at(j).orderValue() == hand.at(j - 3).orderValue()) {
            if (typeSort == -1 || hand.at(j).orderValue() > typeSort)
                return {hand.at(j), hand.at(j - 1), hand.at(j - 2), hand.at(j - 3)};
        }
    }
    return {};
}

QList<Poker> PokerSeekUtils::seekKingBoom(const QList<Poker> &hand)
{
    bool hasSmall = false;
    bool hasBig = false;
    Poker smallCard;
    Poker bigCard;
    for (const Poker &card : hand) {
        if (card.orderValue() == PokerUtils::SmallKingValue) {
            hasSmall = true;
            smallCard = card;
        }
        if (card.orderValue() == PokerUtils::BigKingValue) {
            hasBig = true;
            bigCard = card;
        }
    }
    if (hasSmall && hasBig)
        return {bigCard, smallCard};
    return {};
}

QList<Poker> PokerSeekUtils::randomPlay(const QList<Poker> &hand)
{
    if (hand.isEmpty())
        return {};

    QList<Poker> sorted = PokerUtils::sortDesc(hand);

    for (int len = sorted.size(); len >= 5; --len) {
        for (int start = 0; start <= sorted.size() - len; ++start) {
            QList<Poker> candidate;
            for (int i = start; i < start + len; ++i)
                candidate.append(sorted.at(i));
            if (PokerTypeUtils::getType(candidate).isValid())
                return candidate;
        }
    }

    for (int j = sorted.size() - 1; j >= 3; --j) {
        if (sorted.at(j).orderValue() == sorted.at(j - 1).orderValue()
            && sorted.at(j).orderValue() == sorted.at(j - 2).orderValue()
            && sorted.at(j).orderValue() == sorted.at(j - 3).orderValue()) {
            return {sorted.at(j), sorted.at(j - 1), sorted.at(j - 2), sorted.at(j - 3)};
        }
    }

    const QList<Poker> kingBoom = seekKingBoom(sorted);
    if (!kingBoom.isEmpty())
        return kingBoom;

    for (int j = sorted.size() - 1; j >= 2; --j) {
        if (sorted.at(j).orderValue() == sorted.at(j - 1).orderValue()
            && sorted.at(j).orderValue() == sorted.at(j - 2).orderValue()) {
            return {sorted.at(j), sorted.at(j - 1), sorted.at(j - 2)};
        }
    }

    for (int j = sorted.size() - 1; j >= 1; --j) {
        if (sorted.at(j).orderValue() == sorted.at(j - 1).orderValue())
            return {sorted.at(j), sorted.at(j - 1)};
    }

    return {sorted.last()};
}

QList<Poker> PokerSeekUtils::autoPlay(const QList<Poker> &hand, int mySeat, int landlordSeat,
                                      const QList<Poker> &played, int playedSeat)
{
    if (hand.isEmpty())
        return {};

    if (played.isEmpty())
        return randomPlay(hand);

    if (mySeat == landlordSeat) {
        if (playedSeat == mySeat)
            return randomPlay(hand);
        return seekRight(hand, played);
    }

    if (playedSeat == mySeat)
        return randomPlay(hand);
    if (playedSeat != landlordSeat)
        return {};
    return seekRight(hand, played);
}
