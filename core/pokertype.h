#ifndef POKERTYPE_H
#define POKERTYPE_H

#include <QString>

class PokerType
{
public:
    PokerType() : m_sort(-1) {}
    PokerType(const QString &type, int sort) : m_type(type), m_sort(sort) {}

    QString type() const { return m_type; }
    int sort() const { return m_sort; }
    bool isValid() const { return !m_type.isEmpty() && m_sort >= 0; }

private:
    QString m_type;
    int m_sort;
};

#endif // POKERTYPE_H
