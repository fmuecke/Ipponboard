// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef _BASE_FIGHTERMANAGER_H
#define _BASE_FIGHTERMANAGER_H

#include "../core/Fighter.h"

#include <boost/noncopyable.hpp>
#include <array>
#include <set>

class QString;
class QStringList;

namespace Ipponboard
{

class FighterManager : public boost::noncopyable
{
public:
	FighterManager();

	using FighterList = std::set<Ipponboard::Fighter>;

	static char const* const str_FIRSTNAME;
	static char const* const str_LASTNAME;
	static char const* const str_CLUB;
	static char const* const str_WEIGHT;
	static char const* const str_CATEGORY;

	static const std::array<char const* const, 5> Specifiers;

	static QString GetCsvHeaderFormat(); // returns all specifiers separated by Separator

	bool Contains(Ipponboard::Fighter const& f) const;

	// clears fighter list and imports them from a csv-file
	bool LoadFighters(QString const& csvFile, QString& errorMsg);

	// imports fighters from a csv-file
	bool AddFighters(QString const& csvFile, QString& errorMsg);

	// saves all fighters to a csv-file
	bool SaveFighters(QString const& csvFile, QString& errorMsg);

	Ipponboard::Fighter AddNewFighter();
	bool AddFighter(Ipponboard::Fighter f);
	bool RemoveFighter(Ipponboard::Fighter f);
	QStringList GetClubFighterNames(QString const& filter) const;

	static FighterList Filter(FighterList const& fighters, QString const& specifier, QString const& value);

//private:
	FighterList m_fighters; //TODO: encapsulate
private:
	static const char Separator {';'};

};

}  // namespace Ipponboard

#endif // _BASE_FIGHTERMANAGER_H
