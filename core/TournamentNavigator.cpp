#include "TournamentNavigator.h"

namespace Ipponboard
{

namespace
{
constexpr unsigned int kDefaultIndex = 0;
}

TournamentNavigator::TournamentNavigator(Tournament& tournament) : m_tournament(tournament) {}

unsigned int TournamentNavigator::roundCount() const
{
    return static_cast<unsigned int>(m_tournament.size());
}

unsigned int TournamentNavigator::fightCount() const
{
    if (m_tournament.empty())
    {
        return 0;
    }

    return static_cast<unsigned int>(m_tournament.front()->size());
}

void TournamentNavigator::reset()
{
    m_currentRound = kDefaultIndex;
    m_currentFight = kDefaultIndex;
}

void TournamentNavigator::setCurrentRound(unsigned int index)
{
    if (index < roundCount())
    {
        m_currentRound = index;
        if (m_currentFight >= fightCount())
        {
            m_currentFight = fightCount() > 0 ? fightCount() - 1 : 0;
        }
    }
}

void TournamentNavigator::setCurrentFight(unsigned int index)
{
    if (fightCount() == 0)
    {
        m_currentFight = 0;
        return;
    }

    if (index < fightCount())
    {
        m_currentFight = index;
    }
    else
    {
        m_currentFight = fightCount() - 1;
    }
}

void TournamentNavigator::nextFight()
{
    if (fightCount() == 0)
    {
        return;
    }

    if (m_currentFight + 1 < fightCount())
    {
        ++m_currentFight;
        return;
    }

    if (m_currentRound + 1 < roundCount())
    {
        ++m_currentRound;
        m_currentFight = kDefaultIndex;
    }
}

void TournamentNavigator::prevFight()
{
    if (fightCount() == 0)
    {
        return;
    }

    if (m_currentFight > 0)
    {
        --m_currentFight;
        return;
    }

    if (m_currentRound > 0)
    {
        --m_currentRound;
        m_currentFight = fightCount() > 0 ? fightCount() - 1 : 0;
    }
}

} // namespace Ipponboard
