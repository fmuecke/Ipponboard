#include "CompetitionNavigator.h"

namespace Ipponboard
{

namespace
{
constexpr unsigned int kDefaultIndex = 0;
}

CompetitionNavigator::CompetitionNavigator(Competition& competition) : m_tournament(competition) {}

unsigned int CompetitionNavigator::roundCount() const
{
    return static_cast<unsigned int>(m_tournament.size());
}

unsigned int CompetitionNavigator::fightCount() const
{
    if (m_tournament.empty())
    {
        return 0;
    }

    return static_cast<unsigned int>(m_tournament.front()->size());
}

void CompetitionNavigator::reset()
{
    m_currentRound = kDefaultIndex;
    m_currentFight = kDefaultIndex;
}

void CompetitionNavigator::setCurrentRound(unsigned int index)
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

void CompetitionNavigator::setCurrentFight(unsigned int index)
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

void CompetitionNavigator::nextFight()
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

void CompetitionNavigator::prevFight()
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
