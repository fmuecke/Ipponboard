#ifndef FIGHTCATEGORYPARSER_H
#define FIGHTCATEGORYPARSER_H

#include "../core/FightCategory.h"
#include <string>

class FightCategoryParser
{
public:
	FightCategoryParser();

	//> for internal storage
	static std::string ToJsonString_UNUSED(Ipponboard::FightCategoryList const& categories);
	static std::string ToTomlString(Ipponboard::FightCategoryList const& categories);

	//> for internal storage
	static Ipponboard::FightCategoryList ParseJsonString_UNUSED(std::string const& str);
	static Ipponboard::FightCategoryList ParseTomlString(std::string const& str);

	//> legacy (old format)
	static Ipponboard::FightCategoryList ParseLegacyJsonFile(const char* file);

	// Qt settings format
	static Ipponboard::FightCategoryList ParseIniFile_UNUSED(const char* file);
	static void ToIniFile_UNUSED(const char* file, Ipponboard::FightCategoryList const& categories);

	// Qt settings format
	static Ipponboard::FightCategoryList ParseTomlFile(const char* file);
	static void ToTomlFile(const char* file, Ipponboard::FightCategoryList const& categories);
};

#endif // FIGHTCATEGORYPARSER_H
