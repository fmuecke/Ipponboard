// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#ifndef __CORE_FIGHT_H
#define __CORE_FIGHT_H

#include "Score.h"
#include "Enums.h"

#include "QString"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

namespace Ipponboard
{

struct SimpleFighter
{
	QString name;
	QString club;
};

class Fight
{
private:
	enum
	{
		eScore_Ippon = 10,
		eScore_Wazaari = 7,
		eScore_Yuko = 5,
		eScore_Hantai = 1,
		eScore_Hikewake = 0,
		eScore_Lost = 0
	};

public:
	Fight() 
		: weight("-")
		, time_in_seconds(0)
		, max_time_in_seconds(0)
		, is_saved(false)
	{
	}

	Score scores[2];
	SimpleFighter fighters[2];
	QString weight;
	int time_in_seconds;
	int max_time_in_seconds;
	bool is_saved;

	QString GetTimeFaught() const
	{
		// get time display
		const int minutes = time_in_seconds / 60;
		const int seconds = time_in_seconds - minutes * 60;
		QString ret = QString::number(minutes) + ":";

		if (seconds < 10)	// append leading zero?
			ret += "0";

		return  ret + QString::number(seconds);
	}

	QString GetTimeRemaining() const
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

	bool HasWon(EFighter who) const
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

	int ScorePoints(EFighter who) const
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
				return eScore_Hikewake;
		}

		return eScore_Lost;
	}
};

} // namespace Ipponboard

namespace boost
{
namespace serialization
{
template<class Archive>
void serialize(Archive& ar,
			   Ipponboard::SimpleFighter& f,
			   const unsigned int /*version*/)
{
	ar& BOOST_SERIALIZATION_NVP(f.name);
	ar& BOOST_SERIALIZATION_NVP(f.club);
}

template<class Archive>
void serialize(Archive& ar,
			   Ipponboard::Fight& m,
			   const unsigned int /*version*/)
{
	ar& BOOST_SERIALIZATION_NVP(m.scores);
	ar& BOOST_SERIALIZATION_NVP(m.fighters);
	ar& BOOST_SERIALIZATION_NVP(m.weight);
	ar& BOOST_SERIALIZATION_NVP(m.time_in_seconds);
}

template<class Archive>
void serialize(Archive& ar,
			   Ipponboard::Score& s,
			   const unsigned int /*version*/)
{
	ar& BOOST_SERIALIZATION_NVP(s._points);
}

} // namespace serialization
} // namespace boost

#endif // __CORE_FIGHT_H
