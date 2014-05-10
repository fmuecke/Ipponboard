// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#ifndef FIGHTER_H
#define FIGHTER_H

#include <QString>
#include <tuple>

namespace Ipponboard
{

class Fighter
{
public:
	Fighter(QString const& firstName,
			QString const& lastName);

	inline bool operator==(Fighter const& other) const
	{
		return this == &other ||
			   (first_name == other.first_name &&
				last_name == other.last_name &&
                year == other.year &&
                nation == other.nation &&
				club == other.club &&
                //team == other.team &&
				weight == other.weight &&
				category == other.category);
	}

	inline bool operator<(Fighter const& other) const
	{
		return std::tie(last_name, first_name, club, /*team, */nation, year, category, weight) <
			std::tie(other.last_name, other.first_name, other.club, /*other.team,*/ other.nation, other.year, other.category, other.weight);
	}

	QString first_name;
	QString last_name;
	QString club;
	QString weight;
	QString category;
    QString nation;
    //QString team;
    QString year;
};

}  // namespace Ipponboard

#endif // FIGHTER_H
