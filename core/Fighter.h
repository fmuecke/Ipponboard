// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef FIGHTER_H
#define FIGHTER_H

#include <QString>

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
				club == other.club &&
				weight == other.weight &&
				category == other.category);
	}

	inline bool operator<(Fighter const& other) const
	{
		if (first_name < other.first_name)
			return true;

		if (other.first_name < first_name)
			return false;

		if (last_name < other.last_name)
			return true;

		if (other.last_name < last_name)
			return false;

		if (club < other.club)
			return true;

		if (other.club < club)
			return false;

		if (weight < other.weight)
			return true;

		if (other.weight < weight)
			return false;

		if (category < other.category)
			return true;

		if (other.category < category)
			return false;

		// equal
		return false;
	}

	QString first_name;
	QString last_name;
	QString club;
	QString weight;
	QString category;
	//QString nation;
};

}  // namespace Ipponboard

#endif // FIGHTER_H
