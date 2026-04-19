// Copyright 2013 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __CORE_CONTEST_H
#define __CORE_CONTEST_H

#include "Enums.h"
#include "QString"
#include "Rules.h"
#include "Score.h"

#include <QtGlobal>
#include <memory>

namespace Ipponboard
{
struct ContestAthlete
{
    QString name;
    QString club;
};

class Contest
{
  private:
    enum
    {
        eScore_Ippon = 10,
        eScore_Wazaari = 7,
        eScore_Yuko = 5,
        eScore_Hantai = 1,
        eScore_Shido = 1,
        eScore_Hikewake = 0,
        eScore_Lost = 0
    };

    bool m_isGoldenScore{ false };

  public:
    Contest();

    Contest(Score const& sideA, Score const& sideB) : m_scoreSideA(sideA), m_scoreSideB(sideB) {}

    Score const& GetScore(ContestSide side) const;
    Score& GetScore(ContestSide side);

    ContestAthlete const& GetAthlete(ContestSide side) const;

    ContestAthlete& GetAthlete(ContestSide side);

    bool IsGoldenScore() const { return m_isGoldenScore; }
    void SetGoldenScore(bool val) { m_isGoldenScore = val; }

    int GetSecondsElapsed() const;
    void SetSecondsElapsed(int s);
    bool SetElapsedFromTotalTime(QString s);
    int GetRoundSeconds() const;
    void SetRoundTime(int secs);
    QString GetTotalTimeElapsedString() const;
    QString GetTimeRemainingString() const;

    // returns remaining seconds
    int GetRemainingTime() const;
    int GetGoldenScoreTime() const;
    bool HasWon(ContestSide side) const;
    int GetScorePoints(ContestSide side) const;

    ContestAthlete m_athleteSideA{};
    ContestAthlete m_athleteSideB{};
    QString weight;
    bool is_saved{ false };
    std::shared_ptr<AbstractRules> rules; // TODO: this should be removed if possible

  private:
    [[noreturn]] static void InvalidContestSide(ContestSide side);

    Score m_scoreSideA{};
    Score m_scoreSideB{};
    int m_secondsElapsed{ 0 };
    int m_roundTimeSeconds{ 0 };
};
} // namespace Ipponboard

#endif // __CORE_CONTEST_H
