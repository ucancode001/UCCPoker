#include "pokertypeutils.h"
#include "pokerutils.h"

const QString PokerTypeUtils::TypeBoom = QStringLiteral("typeBoom");
const QString PokerTypeUtils::TypeKingBoom = QStringLiteral("typeKingBoom");
const QString PokerTypeUtils::TypeSingle = QStringLiteral("typeSingle");
const QString PokerTypeUtils::TypePair = QStringLiteral("typePair");
const QString PokerTypeUtils::TypeThree = QStringLiteral("typeThree");
const QString PokerTypeUtils::TypeThreeSingle = QStringLiteral("typeThreeSingle");
const QString PokerTypeUtils::TypeThreePair = QStringLiteral("typeThreePair");
const QString PokerTypeUtils::TypeStraight = QStringLiteral("typeStraight");
const QString PokerTypeUtils::TypeStraightPairs = QStringLiteral("typeStraightPairs");
const QString PokerTypeUtils::TypePlane = QStringLiteral("typePlane");
const QString PokerTypeUtils::TypePlane2Single = QStringLiteral("typePlane2Single");
const QString PokerTypeUtils::TypePlane2Pairs = QStringLiteral("typePlane2Pairs");
const QString PokerTypeUtils::TypeFour2Single = QStringLiteral("typeFour2Single");
const QString PokerTypeUtils::TypeFour2Pairs = QStringLiteral("typeFour2Pairs");

PokerType PokerTypeUtils::getType(QList<Poker> pokers)
{
    pokers = PokerUtils::sortDesc(pokers);
    int orderValue = -1;

    if ((orderValue = isBoom(pokers)) != -1)
        return PokerType(TypeBoom, orderValue);
    if ((orderValue = isKingBoom(pokers)) != -1)
        return PokerType(TypeKingBoom, orderValue);
    if ((orderValue = isSingle(pokers)) != -1)
        return PokerType(TypeSingle, orderValue);
    if ((orderValue = isPair(pokers)) != -1)
        return PokerType(TypePair, orderValue);
    if ((orderValue = isThree(pokers)) != -1)
        return PokerType(TypeThree, orderValue);
    if ((orderValue = isThreeSingle(pokers)) != -1)
        return PokerType(TypeThreeSingle, orderValue);
    if ((orderValue = isThreePairs(pokers)) != -1)
        return PokerType(TypeThreePair, orderValue);
    if ((orderValue = isStraight(pokers)) != -1)
        return PokerType(TypeStraight, orderValue);
    if ((orderValue = isStraightPairs(pokers)) != -1)
        return PokerType(TypeStraightPairs, orderValue);
    if ((orderValue = isPlane(pokers)) != -1)
        return PokerType(TypePlane, orderValue);
    if ((orderValue = isPlane2Single(pokers)) != -1)
        return PokerType(TypePlane2Single, orderValue);
    if ((orderValue = isPlane2Pairs(pokers)) != -1)
        return PokerType(TypePlane2Pairs, orderValue);
    if ((orderValue = isFour2Single(pokers)) != -1)
        return PokerType(TypeFour2Single, orderValue);
    if ((orderValue = isFour2Pairs(pokers)) != -1)
        return PokerType(TypeFour2Pairs, orderValue);

    return PokerType();
}

int PokerTypeUtils::isBoom(const QList<Poker> &array)
{
    if (array.size() == 4
        && array.at(1).orderValue() == array.at(0).orderValue()
        && array.at(2).orderValue() == array.at(0).orderValue()
        && array.at(3).orderValue() == array.at(0).orderValue()) {
        return array.at(0).orderValue();
    }
    return -1;
}

int PokerTypeUtils::isKingBoom(const QList<Poker> &array)
{
    if (array.size() == 2
        && array.at(0).orderValue() == PokerUtils::BigKingValue
        && array.at(1).orderValue() == PokerUtils::SmallKingValue) {
        return 0;
    }
    return -1;
}

int PokerTypeUtils::isSingle(const QList<Poker> &array)
{
    if (array.size() == 1)
        return array.at(0).orderValue();
    return -1;
}

int PokerTypeUtils::isPair(const QList<Poker> &array)
{
    if (array.size() == 2 && array.at(0).orderValue() == array.at(1).orderValue())
        return array.at(0).orderValue();
    return -1;
}

int PokerTypeUtils::isStraight(QList<Poker> array)
{
    if (array.size() < 5 || array.size() > 12)
        return -1;
    if (array.at(0).orderValue() > PokerUtils::AValue)
        return -1;
    for (int i = 0; i < array.size() - 1; ++i) {
        if (array.at(i).orderValue() != array.at(i + 1).orderValue() + 1)
            return -1;
    }
    return array.at(0).orderValue();
}

int PokerTypeUtils::isThree(const QList<Poker> &array)
{
    if (array.size() != 3)
        return -1;
    if (array.at(1).orderValue() == array.at(0).orderValue()
        && array.at(2).orderValue() == array.at(0).orderValue()) {
        return array.at(0).orderValue();
    }
    return -1;
}

int PokerTypeUtils::isThreeSingle(const QList<Poker> &array)
{
    if (array.size() != 4)
        return -1;
    if (array.at(1).orderValue() == array.at(0).orderValue()
        && array.at(2).orderValue() == array.at(0).orderValue()
        && array.at(3).orderValue() != array.at(0).orderValue()) {
        return array.at(0).orderValue();
    }
    if (array.at(1).orderValue() == array.at(3).orderValue()
        && array.at(2).orderValue() == array.at(3).orderValue()
        && array.at(0).orderValue() != array.at(3).orderValue()) {
        return array.at(1).orderValue();
    }
    return -1;
}

int PokerTypeUtils::isThreePairs(const QList<Poker> &array)
{
    if (array.size() != 5)
        return -1;
    if (array.at(1).orderValue() == array.at(0).orderValue()
        && array.at(2).orderValue() == array.at(0).orderValue()
        && array.at(3).orderValue() != array.at(0).orderValue()
        && array.at(3).orderValue() == array.at(4).orderValue()) {
        return array.at(0).orderValue();
    }
    if (array.at(0).orderValue() == array.at(1).orderValue()
        && array.at(2).orderValue() == array.at(3).orderValue()
        && array.at(3).orderValue() == array.at(4).orderValue()
        && array.at(0).orderValue() != array.at(2).orderValue()) {
        return array.at(2).orderValue();
    }
    return -1;
}

int PokerTypeUtils::isStraightPairs(const QList<Poker> &array)
{
    if (array.size() < 6 || array.size() > 20 || array.size() % 2 != 0)
        return -1;
    if (array.at(0).orderValue() < 3)
        return -1;

    for (int i = 0; i < array.size() - 1; i += 2) {
        if (array.at(i).orderValue() != array.at(i + 1).orderValue())
            return -1;
    }
    for (int i = 0; i < array.size() - 2; i += 2) {
        if (array.at(i).orderValue() != array.at(i + 2).orderValue() + 1)
            return -1;
    }
    return array.at(0).orderValue();
}

int PokerTypeUtils::isPlane(const QList<Poker> &array)
{
    if (array.size() != 6)
        return -1;
    if (array.at(0).orderValue() < 3)
        return -1;
    if (array.at(0).orderValue() == array.at(1).orderValue()
        && array.at(1).orderValue() == array.at(2).orderValue()
        && array.at(3).orderValue() == array.at(4).orderValue()
        && array.at(4).orderValue() == array.at(5).orderValue()
        && array.at(0).orderValue() == array.at(3).orderValue() + 1) {
        return array.at(0).orderValue();
    }
    return -1;
}

int PokerTypeUtils::isPlane2Single(const QList<Poker> &array)
{
    if (array.size() != 8)
        return -1;

    QList<Poker> triplets;
    for (int i = 0; i < array.size() - 2; ++i) {
        if (array.at(i).orderValue() == array.at(i + 1).orderValue()
            && array.at(i).orderValue() == array.at(i + 2).orderValue()) {
            triplets.append(array.at(i));
        }
    }
    if (triplets.size() != 2)
        return -1;
    if (triplets.at(0).orderValue() > PokerUtils::AValue)
        return -1;
    if (triplets.at(0).orderValue() - 1 == triplets.at(1).orderValue())
        return triplets.at(0).orderValue();
    return -1;
}

int PokerTypeUtils::isPlane2Pairs(const QList<Poker> &array)
{
    if (array.size() != 10)
        return -1;

    QList<Poker> triplets;
    QList<Poker> removed;
    for (int i = 0; i < array.size() - 2; ++i) {
        if (array.at(i).orderValue() == array.at(i + 1).orderValue()
            && array.at(i).orderValue() == array.at(i + 2).orderValue()) {
            triplets.append(array.at(i));
            removed.append(array.at(i));
            removed.append(array.at(i + 1));
            removed.append(array.at(i + 2));
        }
    }
    if (triplets.size() != 2)
        return -1;

    QList<Poker> rest = PokerUtils::removePokers(array, removed);
    rest = PokerUtils::removePokers(rest, removed);
    for (int i = 0; i < rest.size() - 1; i += 2) {
        if (rest.at(i).orderValue() != rest.at(i + 1).orderValue())
            return -1;
    }
    if (triplets.at(0).orderValue() > PokerUtils::AValue)
        return -1;
    if (triplets.at(0).orderValue() - 1 == triplets.at(1).orderValue())
        return triplets.at(0).orderValue();
    return -1;
}

int PokerTypeUtils::isFour2Single(const QList<Poker> &array)
{
    if (array.size() != 6)
        return -1;
    for (int i = 0; i < array.size() - 3; ++i) {
        if (array.at(i).orderValue() == array.at(i + 1).orderValue()
            && array.at(i).orderValue() == array.at(i + 2).orderValue()
            && array.at(i).orderValue() == array.at(i + 3).orderValue()) {
            return array.at(i).orderValue();
        }
    }
    return -1;
}

int PokerTypeUtils::isFour2Pairs(const QList<Poker> &array)
{
    if (array.size() != 8)
        return -1;

    QList<Poker> rest;
    int fourValue = -1;
    for (int i = 0; i < array.size() - 3; ++i) {
        if (array.at(i).orderValue() == array.at(i + 1).orderValue()
            && array.at(i).orderValue() == array.at(i + 2).orderValue()
            && array.at(i).orderValue() == array.at(i + 3).orderValue()) {
            fourValue = array.at(i).orderValue();
            rest = PokerUtils::removePokers(array, {array.at(i), array.at(i + 1), array.at(i + 2), array.at(i + 3)});
            break;
        }
    }
    if (fourValue == -1)
        return -1;

    for (int i = 0; i < rest.size() - 1; i += 2) {
        if (rest.at(i).orderValue() != rest.at(i + 1).orderValue())
            return -1;
    }
    return fourValue;
}
