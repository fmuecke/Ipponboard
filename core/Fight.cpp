// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#include "Fight.h"

using namespace Ipponboard;

Fight::Fight()
	: weight("-")
	, rules(new ClassicRules)
{
	scores[0] = Score();
	scores[1] = Score();

	fighters[0] = SimpleFighter();
	fighters[1] = SimpleFighter();

    rules->SetCountSubscores(false);
}

int Fight::GetSecondsElapsed() const
{
    return seconds_elapsed;
}

void Fight::SetSecondsElapsed(int s)
{
    //SetGoldenScore(s < 0);
    seconds_elapsed = s;
}

int Fight::GetRoundSeconds() const { return round_time_seconds; }

void Fight::SetRoundTime(int secs) { round_time_seconds = secs; }

int Fight::GetRemainingTime() const
{
    if (IsGoldenScore())
    {
        return 0;
    }

    return round_time_seconds - seconds_elapsed;
}

int Fight::GetGoldenScoreTime() const
{
    if (IsGoldenScore() && seconds_elapsed < 0)
    {
        return -seconds_elapsed;
    }

    return 0;
}


QString Fight::GetTotalTimeElapsedString() const
{
	// get time display
    auto elapsed = IsGoldenScore() ? -seconds_elapsed + round_time_seconds : seconds_elapsed;

    int minutes = elapsed / 60;
    int seconds = elapsed % 60;

    return QString("%1:%3%4").arg(
                QString::number(minutes),
                seconds < 10 ? "0" : "",
                QString::number(seconds));
}

bool Fight::SetElapsedFromTotalTime(QString s)
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

    if (secs > round_time_seconds)
    {
        secs = round_time_seconds - secs;
    }

    SetSecondsElapsed(secs);

    return true;
}

QString Fight::GetTimeRemainingString() const
{
	// get time display
    auto isGoldenScore = seconds_elapsed < 0;

    auto time_remaining = isGoldenScore ? GetGoldenScoreTime() : GetRemainingTime();
	auto minutes = time_remaining / 60;
    auto seconds = time_remaining % 60;

    return QString("%1%2:%3%4").arg(
                isGoldenScore ? "-" : "",
                QString::number(minutes),
                seconds < 10 ? "0" : "",
                QString::number(seconds));
}

bool Fight::HasWon(FighterEnum who) const
{
	const FighterEnum other = GetUkeFromTori(who);

	auto result = rules->CompareScore(*this);

	if (who == FighterEnum::First && result < 0 ||
		who == FighterEnum::Second && result > 0)
	{
		return true;
	}
	return false;
}

int Fight::GetScorePoints(FighterEnum who) const
{
	const FighterEnum other = GetUkeFromTori(who);

	if (HasWon(who))
	{
		if (GetScore(who).Ippon() || rules->IsAwaseteIppon(GetScore(who)))
		{
			return eScore_Ippon;
		}

		// Only the fight deciding point is taken into account!
		if (GetScore(who).Wazaari() > 0 && GetScore(who).Wazaari() > GetScore(other).Wazaari())
		{
			return eScore_Wazaari;
		}

		if (GetScore(who).Yuko() > GetScore(other).Yuko())
		{
			return eScore_Yuko;
		}

		if ((!rules->IsOption_ShidoAddsPoint() || IsGoldenScore()) && GetScore(who).Shido() < GetScore(other).Shido())
		{
			return eScore_Shido;
		}
		//TODO: Hantei!
	}
	else
	{
		if (!HasWon(other))
		{
			return eScore_Hikewake;
		}
		else if (rules->IsOption_CountSubscores())
		{
			// Special rule for Jugendliga
			if (GetScore(who).Wazaari() > GetScore(other).Wazaari())
			{
				return eScore_Wazaari;
			}
			else if (GetScore(who).Yuko() > GetScore(other).Yuko())
			{
				return eScore_Yuko;
			}
		}
	}

	return eScore_Lost;
}
