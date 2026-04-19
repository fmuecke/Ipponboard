#include "CompetitionNavigator.h"

namespace Ipponboard
{

namespace
{
constexpr unsigned int kDefaultIndex = 0;
}

CompetitionNavigator::CompetitionNavigator(Competition& competition) : m_competition(competition) {}

unsigned int CompetitionNavigator::roundCount() const
{
    return static_cast<unsigned int>(m_competition.size());
}

unsigned int CompetitionNavigator::contestCount() const
{
    if (m_competition.empty())
    {
        return 0;
    }

    return static_cast<unsigned int>(m_competition.front()->size());
}

void CompetitionNavigator::reset()
{
    m_currentRound = kDefaultIndex;
    m_currentContest = kDefaultIndex;
}

void CompetitionNavigator::setCurrentRound(unsigned int index)
{
    if (index < roundCount())
    {
        m_currentRound = index;
        if (m_currentContest >= contestCount())
        {
            m_currentContest = contestCount() > 0 ? contestCount() - 1 : 0;
        }
    }
}

void CompetitionNavigator::setCurrentContest(unsigned int index)
{
    if (contestCount() == 0)
    {
        m_currentContest = 0;
        return;
    }

    if (index < contestCount())
    {
        m_currentContest = index;
    }
    else
    {
        m_currentContest = contestCount() - 1;
    }
}

void CompetitionNavigator::nextContest()
{
    if (contestCount() == 0)
    {
        return;
    }

    if (m_currentContest + 1 < contestCount())
    {
        ++m_currentContest;
        return;
    }

    if (m_currentRound + 1 < roundCount())
    {
        ++m_currentRound;
        m_currentContest = kDefaultIndex;
    }
}

void CompetitionNavigator::prevContest()
{
    if (contestCount() == 0)
    {
        return;
    }

    if (m_currentContest > 0)
    {
        --m_currentContest;
        return;
    }

    if (m_currentRound > 0)
    {
        --m_currentRound;
        m_currentContest = contestCount() > 0 ? contestCount() - 1 : 0;
    }
}

} // namespace Ipponboard
