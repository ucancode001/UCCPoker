#ifndef LANDLORDBIDENGINE_H
#define LANDLORDBIDENGINE_H

#include "core/gameconstants.h"
#include "poker.h"

#include <QHash>
#include <QList>
#include <QString>

class LandlordBidEngine
{
public:
    enum class State {
        Idle,
        Calling,
        Grabbing,
        Finished,
        Redeal
    };

    enum class Action {
        Pass,
        Call,
        Grab
    };

    void reset(int firstSeat = GameConstants::SeatPlayer);
    State state() const { return m_state; }
    int activeSeat() const { return m_activeSeat; }
    int landlordSeat() const { return m_candidate; }
    bool isFinished() const { return m_state == State::Finished || m_state == State::Redeal; }

    GameConstants::ButtonGroup buttonGroup() const;
    void applyAction(Action action);
    Action aiAction(const QList<Poker> &hand) const;

    static Action actionFromString(const QString &value);

private:
    void advanceCallTurn();
    void advanceGrabTurn();
    int nextSeat(int seat) const;
    int handStrength(const QList<Poker> &hand) const;

    State m_state = State::Idle;
    int m_firstSeat = GameConstants::SeatPlayer;
    int m_activeSeat = GameConstants::SeatPlayer;
    int m_candidate = 0;
    int m_grabTurnIndex = 0;
    QList<int> m_grabSeats;
    int m_callPasses = 0;
};

#endif // LANDLORDBIDENGINE_H
