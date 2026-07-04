#include "pokerutils.h"

#include <QRandomGenerator>
#include <algorithm>

const QList<int> PokerUtils::s_pukerSortValues = {
    12, 12, 12, 12,
    13, 13, 13, 13,
    1, 1, 1, 1,
    2, 2, 2, 2,
    3, 3, 3, 3,
    4, 4, 4, 4,
    5, 5, 5, 5,
    6, 6, 6, 6,
    7, 7, 7, 7,
    8, 8, 8, 8,
    9, 9, 9, 9,
    10, 10, 10, 10,
    11, 11, 11, 11,
    14, 15
};

const QList<int> PokerUtils::s_pukerIds = {
    1, 2, 3, 4,
    5, 6, 7, 8,
    9, 10, 11, 12,
    13, 14, 15, 16,
    17, 18, 19, 20,
    21, 22, 23, 24,
    25, 26, 27, 28,
    29, 30, 31, 32,
    33, 34, 35, 36,
    37, 38, 39, 40,
    41, 42, 43, 44,
    45, 46, 47, 48,
    49, 50, 51, 52,
    53, 54
};

QList<Poker> PokerUtils::randomDeck()
{
    QList<int> pool = s_pukerIds;
    QList<int> picked;
    picked.reserve(pool.size());

    while (!pool.isEmpty()) {
        const int index = QRandomGenerator::global()->bounded(pool.size());
        picked.append(pool.at(index));
        pool.removeAt(index);
    }

    QList<Poker> pokers;
    pokers.reserve(picked.size());
    for (int id : picked) {
        pokers.append(Poker(id, s_pukerSortValues.at(id - 1)));
    }
    return pokers;
}

bool PokerUtils::sortDescCompare(const Poker &a, const Poker &b)
{
    if (a.orderValue() > b.orderValue())
        return true;
    if (a.orderValue() < b.orderValue())
        return false;
    return a.id() < b.id();
}

QList<Poker> PokerUtils::sortDesc(const QList<Poker> &pokers)
{
    QList<Poker> sorted = pokers;
    std::sort(sorted.begin(), sorted.end(), sortDescCompare);
    return sorted;
}

QList<Poker> PokerUtils::removePokers(const QList<Poker> &array, const QList<Poker> &elements)
{
    QList<Poker> result;
    for (const Poker &card : array) {
        bool removed = false;
        for (const Poker &element : elements) {
            if (card.id() == element.id()) {
                removed = true;
                break;
            }
        }
        if (!removed)
            result.append(card);
    }
    return result;
}

RandomUserInfo PokerUtils::randomUser()
{
    static const QList<RandomUserInfo> users = {
        {QStringLiteral("大可"), QStringLiteral("man")},
        {QStringLiteral("Tiger"), QStringLiteral("lady")},
        {QStringLiteral("翔"), QStringLiteral("man")},
        {QStringLiteral("傻源源"), QStringLiteral("man")},
        {QStringLiteral("傻乐乐"), QStringLiteral("man")},
        {QStringLiteral("小巴西"), QStringLiteral("lady")},
        {QStringLiteral("油烟机"), QStringLiteral("man")},
        {QStringLiteral("可乐鸡翅"), QStringLiteral("lady")},
        {QStringLiteral("酸辣土豆丝"), QStringLiteral("lady")},
        {QStringLiteral("糖拌西红柿"), QStringLiteral("man")},
        {QStringLiteral("拍黄瓜"), QStringLiteral("man")},
        {QStringLiteral("空调没有遥控器"), QStringLiteral("man")},
        {QStringLiteral("这么晚了还不回家"), QStringLiteral("lady")},
        {QStringLiteral("程咬金"), QStringLiteral("man")},
        {QStringLiteral("猫砂不会盖"), QStringLiteral("man")},
        {QStringLiteral("风扇不摇头"), QStringLiteral("man")},
        {QStringLiteral("油炸花生米"), QStringLiteral("man")},
        {QStringLiteral("电视不能看"), QStringLiteral("man")},
        {QStringLiteral("绿萝"), QStringLiteral("lady")},
        {QStringLiteral("薄荷糖不麻"), QStringLiteral("lady")},
        {QStringLiteral("翔的小姐姐"), QStringLiteral("lady")},
        {QStringLiteral("孤单想吃西瓜"), QStringLiteral("man")},
        {QStringLiteral("一个人看烟花"), QStringLiteral("lady")},
        {QStringLiteral("大海啊你全是水"), QStringLiteral("man")},
        {QStringLiteral("骏马啊你四条腿"), QStringLiteral("man")}
    };

    return users.at(QRandomGenerator::global()->bounded(users.size()));
}

QString PokerUtils::randomPassText()
{
    static const QStringList tips = {
        QStringLiteral("不要"), QStringLiteral("没你的大"), QStringLiteral("要不起"),
        QStringLiteral("你厉害"), QStringLiteral("我认怂"), QStringLiteral("你牛"),
        QStringLiteral("过"), QStringLiteral("GO"), QStringLiteral("PASS"), QStringLiteral("0.0")
    };
    return tips.at(QRandomGenerator::global()->bounded(tips.size()));
}

QString PokerUtils::rankLabel(int orderValue)
{
    switch (orderValue) {
    case 1: return QStringLiteral("3");
    case 2: return QStringLiteral("4");
    case 3: return QStringLiteral("5");
    case 4: return QStringLiteral("6");
    case 5: return QStringLiteral("7");
    case 6: return QStringLiteral("8");
    case 7: return QStringLiteral("9");
    case 8: return QStringLiteral("10");
    case 9: return QStringLiteral("J");
    case 10: return QStringLiteral("Q");
    case 11: return QStringLiteral("K");
    case 12: return QStringLiteral("A");
    case 13: return QStringLiteral("2");
    case 14: return QStringLiteral("小");
    case 15: return QStringLiteral("大");
    default: return QStringLiteral("?");
    }
}

QString PokerUtils::suitSymbol(int cardId)
{
    if (cardId == 53)
        return QStringLiteral("🃏");
    if (cardId == 54)
        return QStringLiteral("🃏");

    static const QString suits[] = {
        QStringLiteral("♠"), QStringLiteral("♥"), QStringLiteral("♣"), QStringLiteral("♦")
    };
    return suits[(cardId - 1) % 4];
}

bool PokerUtils::isRedSuit(int cardId)
{
    if (cardId >= 53)
        return cardId == 54;
    const int suit = (cardId - 1) % 4;
    return suit == 1 || suit == 3;
}

bool PokerUtils::isJoker(int cardId)
{
    return cardId >= 53;
}

int PokerUtils::orderValueForCardId(int cardId)
{
    if (cardId < 1 || cardId > s_pukerSortValues.size())
        return 0;
    return s_pukerSortValues.at(cardId - 1);
}
