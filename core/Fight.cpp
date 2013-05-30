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
    , allSubscoresCount(false)
{
    scores[0] = Score();
    scores[1] = Score();

    fighters[0] = SimpleFighter();
    fighters[1] = SimpleFighter();
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

bool Fight::HasWon(EFighter who) const
{
    const EFighter tori = who;
    const EFighter uke = (tori == eFighter1) ?
                         eFighter2 : eFighter1;

    if (scores[tori].Ippon() || scores[tori].IsAwaseteIppon())
        return true;

    if (scores[uke].Ippon() || scores[uke].IsAwaseteIppon())
        return false;

    if (scores[tori].Wazaari() > scores[uke].Wazaari())
        return true;

    if (scores[tori].Wazaari() < scores[uke].Wazaari())
        return false;

    if (scores[tori].Yuko() > scores[uke].Yuko())
        return true;

    return false;
}

int Fight::ScorePoints(EFighter who) const
{
    const EFighter tori = who;
    const EFighter uke = (tori == eFighter1) ?
                         eFighter2 : eFighter1;

    if (HasWon(tori))
    {
        if (scores[tori].Ippon() || scores[tori].IsAwaseteIppon())
            return eScore_Ippon;

        // Only the fight deciding point is taken into account!
        if (scores[tori].Wazaari() > 0 &&
                scores[tori].Wazaari() > scores[uke].Wazaari())
            return eScore_Wazaari;

        if (scores[tori].Yuko() > 0)
            return eScore_Yuko;

        //TODO: Hantei!
    }
    else
    {
        if (!HasWon(uke))
        {
            return eScore_Hikewake;
        }
        else if (allSubscoresCount)
        {
            // Special rule for Jugendliga
            if (scores[tori].Wazaari() > scores[uke].Wazaari())
            {
                return eScore_Wazaari;
            }
            else if(scores[tori].Yuko() > scores[uke].Yuko())
            {
                return eScore_Yuko;
            }
        }
    }

    return eScore_Lost;
}
