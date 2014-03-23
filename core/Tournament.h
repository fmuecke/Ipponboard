#ifndef BASE__TOURNAMENT_H_
#define BASE__TOURNAMENT_H_

#include <QString>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <vector>

#include "score.h"

namespace Ipponboard
{
enum { eTournament_FightCount = 10 };
enum EScore
{
	eScore_Ippon = 10,
	eScore_Wazaari = 7,
	eScore_Yuko = 5,
	eScore_Shido = 1,
	eScore_Hantai = 1,
	eScore_Hikewake = 0,
	eScore_Lost = 0
};

struct Fighter
{
	QString name;
	QString club;
};

struct Fight
{
	Fight()
		: weight("-")
		, time_in_seconds(0)
		, is_saved(false)
		, ruleSet(eClassicRules)
	{
		scores[0].Clear();
		scores[1].Clear();
	}

	Score scores[2];
	Fighter fighters[2];
	QString weight;
	int time_in_seconds;
	bool is_saved;
	RuleSet ruleSet;

	const QString GetRoundTimeRemainingText() const
	{
		// get time display
		const int minutes = time_in_seconds / 60;
		const int seconds = time_in_seconds - minutes * 60;
		QString ret = QString::number(minutes) + ":";

		if (seconds < 10)	// append leading zero?
			ret += "0";

		return  ret + QString::number(seconds);
	}

	const QString GetRoundTimeUsedText(int max_time_in_seconds) const
	{
		// get time display
		const int time_used = max_time_in_seconds - time_in_seconds;
		const int minutes = time_used / 60;
		const int seconds = time_used - minutes * 60;
		QString ret = QString::number(minutes) + ":";

		if (seconds < 10)	// append leading zero?
			ret += "0";

		return  ret + QString::number(seconds);
	}

	bool HasWon(EFighter who) const
	{
		const EFighter other = GetUkeFromTori(who);
		return scores[other].IsLess(scores[who], ruleSet);
	}

	int ScorePoints(EFighter who) const
	{
		const EFighter other = GetUkeFromTori(who);

		if (HasWon(who))
		{
			if (scores[who].Ippon() || scores[who].IsAwaseteIppon())
			{
				return eScore_Ippon;
			}

			// Only the fight deciding point is taken into account!
			if (scores[who].Wazaari() > 0
					&& scores[who].Wazaari() > scores[other].Wazaari())
			{
				return eScore_Wazaari;
			}

			if (scores[who].Yuko() > 0)
			{
				return eScore_Yuko;
			}

			if (e2013RuleSet == ruleSet
					&& scores[who].Shido() < scores[other].Shido())
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
		}

		return eScore_Lost;
	}
};

//FIXME:typedef std::vector<Fight> Tournament;
typedef boost::array<Fight, eTournament_FightCount> Tournament;

} // namespace Ipponboard


namespace boost
{
namespace serialization
{
template<class Archive>
void serialize(Archive& ar,
			   Ipponboard::Fighter& f,
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


#endif  // BASE__TOURNAMENT_H_
