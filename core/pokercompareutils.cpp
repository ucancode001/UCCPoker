#include "pokercompareutils.h"
#include "pokerutils.h"
#include "pokertypeutils.h"

bool PokerCompareUtils::comparePokers(QList<Poker> a, QList<Poker> b)
{
    QList<Poker> aSort = PokerUtils::sortDesc(a);
    QList<Poker> bSort = PokerUtils::sortDesc(b);
    if (aSort.isEmpty() || bSort.isEmpty())
        return false;

    if (PokerTypeUtils::isKingBoom(bSort) != -1)
        return false;
    if (PokerTypeUtils::isKingBoom(aSort) != -1)
        return true;

    if (PokerTypeUtils::isBoom(bSort) != -1) {
        if (PokerTypeUtils::isBoom(aSort) != -1)
            return compareOne(aSort.at(0), bSort.at(0));
        return false;
    }
    if (PokerTypeUtils::isBoom(aSort) != -1)
        return true;

    if (a.size() != b.size())
        return false;

    PokerType aType = PokerTypeUtils::getType(aSort);
    PokerType bType = PokerTypeUtils::getType(bSort);
    if (!aType.isValid() || !bType.isValid() || aType.type() != bType.type())
        return false;

    return aType.sort() > bType.sort();
}

bool PokerCompareUtils::compareOne(const Poker &a, const Poker &b)
{
    return a.orderValue() > b.orderValue();
}
