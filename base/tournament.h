#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include <QString>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/array.hpp>

#include "score.h"

namespace Ipponboard
{
	enum { eTournament_MatchCount = 10 };
	enum EScore
	{
		eScore_Ippon = 10,
		eScore_Wazaari = 7,
		eScore_Yuko = 5,
		eScore_Hantai = 1,
		eScore_Hikewake = 0,
		eScore_Lost = 0
	};

	struct Fighter
	{
		QString name;
		QString club;
	};

	struct Match
	{
		Match() : weight("-"), time_in_seconds(0), is_saved(false)
		{
		}

		Score scores[2];
		Fighter fighters[2];
		QString weight;
		int time_in_seconds;
		bool is_saved;

		const QString GetRoundTimeText() const
		{
			// get time display
			const int minutes = time_in_seconds / 60;
			const int seconds = time_in_seconds - minutes*60;
			QString ret = QString::number(minutes) + ":";
			if (seconds < 10)	// append leading zero?
				ret += "0";
			return  ret + QString::number(seconds);
		}

		bool HasWon(EFighter who) const
		{
			const EFighter tori = who;
            const EFighter uke = (tori == eFighter_Blue)?
                                 eFighter_White : eFighter_Blue;

            if( scores[tori].Ippon() || scores[tori].IsAwaseteIppon() )
				return true;
			if( scores[uke].Ippon() || scores[uke].IsAwaseteIppon() )
				return false;
			if( scores[tori].Wazaari() > scores[uke].Wazaari() )
				return true;
			if( scores[tori].Wazaari() < scores[uke].Wazaari() )
				return false;
            if( scores[tori].Yuko() > scores[uke].Yuko() )
				return true;
			return false;
		}
		int ScorePoints(EFighter who) const
		{
            if( HasWon(who) )
            {
                if( scores[who].Ippon() || scores[who].IsAwaseteIppon() )
                    return eScore_Ippon;
                if( scores[who].Wazaari() > 0)
                    return eScore_Wazaari;
                if( scores[who].Yuko() > 0 )
                    return eScore_Yuko;
                //TODO: Hantei!
            }
            else
            {
                const EFighter tori = who;
                const EFighter uke = (tori == eFighter_Blue)?
                                    eFighter_White : eFighter_Blue;
                if ( !HasWon(uke) )
                    return eScore_Hikewake;

                return eScore_Lost;
            }
		}
	};

	typedef boost::array<Match, eTournament_MatchCount> Tournament;

} // namespace Ipponboard


namespace boost
{
namespace serialization
{

	template<class Archive>
	void serialize(Archive& ar, Ipponboard::Fighter& f, const unsigned int /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(c.name);
		ar & BOOST_SERIALIZATION_NVP(c.club);
	}

	template<class Archive>
	void serialize(Archive& ar, Ipponboard::Match& m, const unsigned int /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(m.score);
		ar & BOOST_SERIALIZATION_NVP(m.fighters);
		ar & BOOST_SERIALIZATION_NVP(m.weight);
		ar & BOOST_SERIALIZATION_NVP(m.time_in_seconds);
	}

	template<class Archive>
	void serialize(Archive& ar, Ipponboard::Score& s, const unsigned int /*version*/)
	{
		ar & BOOST_SERIALIZATION_NVP(s._ippon);
		ar & BOOST_SERIALIZATION_NVP(s._wazaari);
		ar & BOOST_SERIALIZATION_NVP(s._yuko);
		ar & BOOST_SERIALIZATION_NVP(s._shido);
		ar & BOOST_SERIALIZATION_NVP(s._hansokumake);
		ar & BOOST_SERIALIZATION_NVP(s._isAwaseteIppon);
	}

} // namespace serialization
} // namespace boost


#endif // TOURNAMENT_H
