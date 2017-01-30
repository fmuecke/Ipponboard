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
    , time_in_seconds(0)
    , max_time_in_seconds(0)
    , is_saved(false)
    , rules(new ClassicRules)
{
    scores[0] = Score();
    scores[1] = Score();

    fighters[0] = SimpleFighter();
    fighters[1] = SimpleFighter();

    rules->SetCountSubscores(false);
}


QString Fight::GetTimeFaught() const
{
    // get time display
    const int minutes = time_in_seconds / 60;
    const int seconds = time_in_seconds - minutes * 60;
    QString ret = QString::number(minutes) + ":";

    if (seconds < 10)	// append leading zero?
        ret += "0";

    return  ret + QString::number(seconds);
}

QString Fight::GetTimeRemaining() const
{
    // get time display
    const int time_remaining = max_time_in_seconds - time_in_seconds;
    const int minutes = time_remaining / 60;
    const int seconds = time_remaining - minutes * 60;
    QString ret = QString::number(minutes) + ":";

    if (seconds < 10)	// append leading zero?
        ret += "0";

    return  ret + QString::number(seconds);
}

bool Fight::HasWon(FighterEnum who) const
{
	const FighterEnum other = GetUkeFromTori(who);
    return rules->IsScoreLess(GetScore(other), GetScore(who));
}

int Fight::ScorePoints(FighterEnum who) const
{
    const FighterEnum other = GetUkeFromTori(who);

    if (HasWon(who))
    {
        if (GetScore(who).Ippon() || rules->IsAwaseteIppon(GetScore(who)))
            return eScore_Ippon;

        // Only the fight deciding point is taken into account!
        if (GetScore(who).Wazaari() > 0 && GetScore(who).Wazaari() > GetScore(other).Wazaari())
        {
			return eScore_Wazaari;
		}

		if (GetScore(who).Yuko() > GetScore(other).Yuko())
		{
            return eScore_Yuko;
		}

        if (!rules->IsOption_ShidoAddsPoint() && GetScore(who).Shido() < GetScore(other).Shido())
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
            else if(GetScore(who).Yuko() > GetScore(other).Yuko())
            {
                return eScore_Yuko;
            }
        }
    }

    return eScore_Lost;
}
