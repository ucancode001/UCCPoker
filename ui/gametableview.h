#ifndef GAMETABLEVIEW_H
#define GAMETABLEVIEW_H

#include <QFrame>
#include <QWidget>

class CardHandWidget;
class QLabel;

class GameTableView : public QWidget
{
    Q_OBJECT

public:
    explicit GameTableView(QWidget *parent = nullptr);

    QFrame *selfBadge() const { return m_selfBadge; }
    QFrame *leftBadge() const { return m_leftBadge; }
    QFrame *rightBadge() const { return m_rightBadge; }
    QLabel *leftCountLabel() const { return m_leftCountLabel; }
    QLabel *rightCountLabel() const { return m_rightCountLabel; }
    CardHandWidget *playerHand() const { return m_playerHand; }
    CardHandWidget *playerPlayed() const { return m_playerPlayed; }
    CardHandWidget *leftPlayed() const { return m_leftPlayed; }
    CardHandWidget *rightPlayed() const { return m_rightPlayed; }
    CardHandWidget *bottomCards() const { return m_bottomCards; }
    QLabel *leftPassLabel() const { return m_leftPassLabel; }
    QLabel *rightPassLabel() const { return m_rightPassLabel; }
    QWidget *buttonPanel() const { return m_buttonPanel; }

    void setSelfName(const QString &name);
    void setLeftName(const QString &name);
    void setRightName(const QString &name);
    void setOpponentCountsVisible(bool visible);
    void clearPlayAreas();
    void clearPassTips();

    QPointF handCenterGlobal() const;
    QPointF playerPlayedCenterGlobal() const;
    QPointF leftPlayedCenterGlobal() const;
    QPointF rightPlayedCenterGlobal() const;
    QPointF seatCenterGlobal(int seat, int mySeat) const;

private:
    QFrame *m_selfBadge = nullptr;
    QFrame *m_leftBadge = nullptr;
    QFrame *m_rightBadge = nullptr;
    QLabel *m_leftCountLabel = nullptr;
    QLabel *m_rightCountLabel = nullptr;
    CardHandWidget *m_playerHand = nullptr;
    CardHandWidget *m_playerPlayed = nullptr;
    CardHandWidget *m_leftPlayed = nullptr;
    CardHandWidget *m_rightPlayed = nullptr;
    CardHandWidget *m_bottomCards = nullptr;
    QLabel *m_leftPassLabel = nullptr;
    QLabel *m_rightPassLabel = nullptr;
    QWidget *m_buttonPanel = nullptr;
};

#endif // GAMETABLEVIEW_H
