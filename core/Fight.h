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
    Fight();

	Score scores[2];
	SimpleFighter fighters[2];
	QString weight;
	int time_in_seconds;
	int max_time_in_seconds;
	bool is_saved;
    bool allSubscoresCount; // option (first of many ;-)

    QString GetTimeFaught() const;
    QString GetTimeRemaining() const;
    bool HasWon(EFighter who) const;
    int ScorePoints(EFighter who) const;
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
    ar& BOOST_SERIALIZATION_NVP(m.max_time_in_seconds);
    ar& BOOST_SERIALIZATION_NVP(m.is_saved);
    ar& BOOST_SERIALIZATION_NVP(m.allSubscoresCount);
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
