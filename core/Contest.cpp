// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "Contest.h"

using namespace Ipponboard;

Contest::Contest() : weight("-"), rules(new ClassicRules)
{
    m_scoresBySide[ToIndex(ContestSide::SideA)] = Score();
    m_scoresBySide[ToIndex(ContestSide::SideB)] = Score();

    m_athletesBySide[ToIndex(ContestSide::SideA)] = ContestAthlete();
    m_athletesBySide[ToIndex(ContestSide::SideB)] = ContestAthlete();

    rules->SetCountSubscores(false);
}

int Contest::GetSecondsElapsed() const { return m_secondsElapsed; }

void Contest::SetSecondsElapsed(int s)
{
    //SetGoldenScore(s < 0);
    m_secondsElapsed = s;
}

int Contest::GetRoundSeconds() const { return m_roundTimeSeconds; }

void Contest::SetRoundTime(int secs) { m_roundTimeSeconds = secs; }

int Contest::GetRemainingTime() const
{
    if (IsGoldenScore())
    {
        return 0;
    }

    return m_roundTimeSeconds - m_secondsElapsed;
}

int Contest::GetGoldenScoreTime() const
{
    if (IsGoldenScore() && m_secondsElapsed < 0)
    {
        return -m_secondsElapsed;
    }

    return 0;
}

QString Contest::GetTotalTimeElapsedString() const
{
    auto elapsed = IsGoldenScore() ? -m_secondsElapsed + m_roundTimeSeconds : m_secondsElapsed;

    int minutes = elapsed / 60;
    int seconds = elapsed % 60;

    return QString("%1:%3%4").arg(
        QString::number(minutes), seconds < 10 ? "0" : "", QString::number(seconds));
}

bool Contest::SetElapsedFromTotalTime(QString s)
{
    int secs = 0;
    auto parts = s.split(":");

    if (parts.size() > 1)
    {
        secs += parts.at(0).toUInt() * 60;
        secs += parts.at(1).toUInt();
    }
    else
    {
        return false;
    }

    if (secs > m_roundTimeSeconds)
    {
        secs = m_roundTimeSeconds - secs;
        SetGoldenScore(true);
    }
    else
    {
        SetGoldenScore(false);
    }

    SetSecondsElapsed(secs);

    return true;
}

QString Contest::GetTimeRemainingString() const
{
    auto isGoldenScore = m_secondsElapsed < 0;

    auto timeRemaining = isGoldenScore ? GetGoldenScoreTime() : GetRemainingTime();
    auto minutes = timeRemaining / 60;
    auto seconds = timeRemaining % 60;

    return QString("%1%2:%3%4")
        .arg(isGoldenScore ? "-" : "",
             QString::number(minutes),
             seconds < 10 ? "0" : "",
             QString::number(seconds));
}

bool Contest::HasWon(ContestSide side) const
{
    auto result = rules->CompareScore(*this);

    if ((side == ContestSide::SideA && result < 0) || (side == ContestSide::SideB && result > 0))
    {
        return true;
    }

    return false;
}

int Contest::GetScorePoints(ContestSide side) const
{
    const ContestSide otherSide = OpposingSide(side);

    if (HasWon(side))
    {
        if (GetScore(side).Ippon() || rules->IsAwaseteIppon(GetScore(side)))
        {
            return eScore_Ippon;
        }

        if (GetScore(side).Wazaari() > 0 &&
            GetScore(side).Wazaari() > GetScore(otherSide).Wazaari())
        {
            return eScore_Wazaari;
        }

        if (GetScore(side).Yuko() > GetScore(otherSide).Yuko())
        {
            return eScore_Yuko;
        }

        if ((!rules->IsOption_ShidoAddsPoint() || IsGoldenScore()) &&
            GetScore(side).Shido() < GetScore(otherSide).Shido())
        {
            return eScore_Shido;
        }
    }
    else
    {
        if (!HasWon(otherSide))
        {
            return eScore_Hikewake;
        }
        else if (rules->IsOption_CountSubscores())
        {
            if (GetScore(side).Wazaari() > GetScore(otherSide).Wazaari())
            {
                return eScore_Wazaari;
            }
            else if (GetScore(side).Yuko() > GetScore(otherSide).Yuko())
            {
                return eScore_Yuko;
            }
        }
    }

    return eScore_Lost;
}
