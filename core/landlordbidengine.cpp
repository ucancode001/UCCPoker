#include "landlordbidengine.h"

#include <QRandomGenerator>

void LandlordBidEngine::reset(int firstSeat)
{
    m_firstSeat = firstSeat;
    m_activeSeat = firstSeat;
    m_candidate = 0;
    m_grabTurnIndex = 0;
    m_grabSeats.clear();
    m_callPasses = 0;
    m_state = State::Calling;
}

GameConstants::ButtonGroup LandlordBidEngine::buttonGroup() const
{
    if (m_state == State::Grabbing)
        return GameConstants::ButtonsFightLandlord;
    return GameConstants::ButtonsCallLandlord;
}

int LandlordBidEngine::nextSeat(int seat) const
{
    return seat >= GameConstants::SeatLeft ? GameConstants::SeatPlayer : seat + 1;
}

void LandlordBidEngine::applyAction(Action action)
{
    if (isFinished())
        return;

    if (m_state == State::Calling) {
        if (action == Action::Call) {
            m_candidate = m_activeSeat;
            m_grabSeats = {nextSeat(m_activeSeat), nextSeat(nextSeat(m_activeSeat))};
            m_grabTurnIndex = 0;
            m_activeSeat = m_grabSeats.first();
            m_state = State::Grabbing;
            return;
        }

        ++m_callPasses;
        m_activeSeat = nextSeat(m_activeSeat);
        if (m_callPasses >= 3)
            m_state = State::Redeal;
        return;
    }

    if (m_state == State::Grabbing) {
        if (action == Action::Grab)
            m_candidate = m_activeSeat;
        ++m_grabTurnIndex;
        if (m_grabTurnIndex >= m_grabSeats.size()) {
            m_state = State::Finished;
            return;
        }
        m_activeSeat = m_grabSeats.at(m_grabTurnIndex);
    }
}

LandlordBidEngine::Action LandlordBidEngine::actionFromString(const QString &value)
{
    const QString lower = value.trimmed().toLower();
    if (lower == QStringLiteral("call") || lower == QStringLiteral("calllandlord")
        || lower == QStringLiteral("jiao")) {
        return Action::Call;
    }
    if (lower == QStringLiteral("grab") || lower == QStringLiteral("fight")
        || lower == QStringLiteral("qiang")) {
        return Action::Grab;
    }
    return Action::Pass;
}

int LandlordBidEngine::handStrength(const QList<Poker> &hand) const
{
    int score = 0;
    QHash<int, int> counts;
    for (const Poker &card : hand)
        ++counts[card.orderValue()];

    for (auto it = counts.constBegin(); it != counts.constEnd(); ++it) {
        if (it.value() == 4)
            score += 8;
        else if (it.value() == 3)
            score += 3;
        else if (it.value() == 2)
            score += 1;
        if (it.key() >= 13)
            score += 2;
        if (it.key() >= 14)
            score += 4;
    }
    return score;
}

LandlordBidEngine::Action LandlordBidEngine::aiAction(const QList<Poker> &hand) const
{
    const int strength = handStrength(hand);
    const int threshold = m_state == State::Grabbing ? 10 : 7;
    const int randomBoost = QRandomGenerator::global()->bounded(3);

    if (strength + randomBoost >= threshold)
        return m_state == State::Grabbing ? Action::Grab : Action::Call;
    return Action::Pass;
}
