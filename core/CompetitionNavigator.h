#pragma once

#include "Competition.h"

namespace Ipponboard
{

class CompetitionNavigator
{
  public:
    explicit CompetitionNavigator(Competition& competition);

    unsigned int currentRound() const { return m_currentRound; }
    unsigned int currentContest() const { return m_currentContest; }

    void setCurrentRound(unsigned int index);
    void setCurrentContest(unsigned int index);

    void nextContest();
    void prevContest();

    unsigned int roundCount() const;
    unsigned int contestCount() const;

    void reset();

  private:
    Competition& m_competition;
    unsigned int m_currentRound{ 0 };
    unsigned int m_currentContest{ 0 };
};

} // namespace Ipponboard
