// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef FIGHTCATEGORYPARSER_H
#define FIGHTCATEGORYPARSER_H

#include "../core/FightCategory.h"
#include <string>

class FightCategoryParser
{
public:
	FightCategoryParser();

	//> for internal storage
	static std::string ToJsonString(Ipponboard::FightCategoryList const& categories);

	//> for internal storage
	static Ipponboard::FightCategoryList ParseJsonString(std::string const& categories);

	//> legacy (old format)
	static Ipponboard::FightCategoryList ParseJsonFile(const QString& file);

	// Qt settings format
	static Ipponboard::FightCategoryList ParseIniFile(const QString& file);
	static void ToIniFile(const QString& file, Ipponboard::FightCategoryList const& categories);
};

#endif // FIGHTCATEGORYPARSER_H
