#ifndef POKERUTILS_H
#define POKERUTILS_H

#include "poker.h"

#include <QList>
#include <QString>

struct RandomUserInfo {
    QString name;
    QString sex;
};

class PokerUtils
{
public:
    static constexpr int AValue = 12;
    static constexpr int BigKingValue = 15;
    static constexpr int SmallKingValue = 14;

    static QList<Poker> randomDeck();
    static QList<Poker> sortDesc(const QList<Poker> &pokers);
    static QList<Poker> removePokers(const QList<Poker> &array, const QList<Poker> &elements);
    static RandomUserInfo randomUser();
    static QString randomPassText();

    static QString rankLabel(int orderValue);
    static QString suitSymbol(int cardId);
    static bool isRedSuit(int cardId);
    static bool isJoker(int cardId);
    static int orderValueForCardId(int cardId);

private:
    static bool sortDescCompare(const Poker &a, const Poker &b);

    static const QList<int> s_pukerSortValues;
    static const QList<int> s_pukerIds;
};

#endif // POKERUTILS_H
