#pragma once

#include "Competition.h"

namespace Ipponboard
{

class CompetitionNavigator
{
  public:
    explicit CompetitionNavigator(Competition& competition);

    unsigned int currentRound() const { return m_currentRound; }
    unsigned int currentFight() const { return m_currentFight; }

    void setCurrentRound(unsigned int index);
    void setCurrentFight(unsigned int index);

    void nextFight();
    void prevFight();

    unsigned int roundCount() const;
    unsigned int fightCount() const;

    void reset();

  private:
    Competition& m_tournament;
    unsigned int m_currentRound{ 0 };
    unsigned int m_currentFight{ 0 };
};

} // namespace Ipponboard
