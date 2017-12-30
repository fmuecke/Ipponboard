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
	static Ipponboard::FightCategoryList ParseJsonFile(const char* file);

	// Qt settings format
	static Ipponboard::FightCategoryList ParseIniFile(const char* file);
	static void ToIniFile(const char* file, Ipponboard::FightCategoryList const& categories);
};

#endif // FIGHTCATEGORYPARSER_H
