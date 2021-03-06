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

	static char const* const str_FIRSTNAME;
	static char const* const str_LASTNAME;
	static char const* const str_CLUB;
	static char const* const str_WEIGHT;
	static char const* const str_CATEGORY;

	static const std::array<char const* const, 5> Specifiers;

	static QString GetSpecifiererDescription();
	//static bool IsValidSpecifier(QString const& str);
	static bool IsFormatSatisfying(QString const& formatStr);
	static bool DetermineSeparator(QString const& str, QString& sep);

	static QString DefaultExportFormat();

	bool ImportFighters(QString const& fileName, QString const& formatStr, QString& errorMsg);
	bool ExportFighters(QString const& fileName, QString const& formatStr, QString& errorMsg);

	bool AddFighter(Ipponboard::Fighter f);
	bool RemoveFighter(Ipponboard::Fighter f);
	QStringList GetClubFighterNames(QString const& filter) const;

//private:
	std::set<Ipponboard::Fighter> m_fighters; //TODO: encapsulate
private:

};

}  // namespace Ipponboard

#endif // _BASE_FIGHTERMANAGER_H
