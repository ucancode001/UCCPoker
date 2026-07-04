#ifndef POKERCOMPAREUTILS_H
#define POKERCOMPAREUTILS_H

#include "poker.h"

#include <QList>

class PokerCompareUtils
{
public:
    static bool comparePokers(QList<Poker> a, QList<Poker> b);

private:
    static bool compareOne(const Poker &a, const Poker &b);
};

#endif // POKERCOMPAREUTILS_H
