#ifndef POKER_H
#define POKER_H

#include <QMetaType>

class Poker
{
public:
    Poker() : m_id(0), m_orderValue(0) {}
    Poker(int id, int orderValue) : m_id(id), m_orderValue(orderValue) {}

    int id() const { return m_id; }
    int orderValue() const { return m_orderValue; }

    bool operator==(const Poker &other) const
    {
        return m_id == other.m_id;
    }

private:
    int m_id;
    int m_orderValue;
};

Q_DECLARE_METATYPE(Poker)

#endif // POKER_H
