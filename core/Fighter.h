// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef FIGHTER_H
#define FIGHTER_H

#include <QString>
#include "../util/qstring_helper.h"

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

	inline Fighter GetNormalized() const
	{
		Fighter f(fm::NormalizeSpaces(first_name), fm::NormalizeSpaces(last_name));
		f.club = fm::NormalizeSpaces(club);
		f.weight = fm::NormalizeSpaces(weight);
		f.category = fm::NormalizeSpaces(category);

		return f;
	}

	// ignores weight or category if empty
	inline bool Matches(Fighter const& other) const
	{
		if (this == &other) return true;
		auto lhs = GetNormalized();
		auto rhs = other.GetNormalized();
		return (lhs.first_name == other.first_name &&
				lhs.last_name == other.last_name &&
				(lhs.club == rhs.club || club.isEmpty() || other.club.isEmpty()) &&
				(lhs.weight == rhs.weight || weight.isEmpty() || other.weight.isEmpty()) &&
				(lhs.category == rhs.category || category.isEmpty() || other.category.isEmpty()));
	}

	inline bool operator!=(Fighter const& other) const
	{
		return !(*this==other);
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
