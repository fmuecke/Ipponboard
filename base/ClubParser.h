#ifndef CLUBPARSER_H
#define CLUBPARSER_H

#include "Club.h"
#include "../util/json.hpp"
#include "../util/qt_helpers.hpp"

#include <QSettings>
#include <QStringList>

namespace ClubParser
{

namespace Tags
{
static const char* Address = "Address";
static const char* LogoFile = "LogoFile";
}

static Ipponboard::ClubList ParseJsonFile(const char* filePath)
{
	Ipponboard::ClubList clubs;

	auto const& jsonClubs = fm::Json::ReadFile(filePath);

	for (auto const & jsonClub : jsonClubs)
	{
		Ipponboard::Club club;
		club.name = fm::qt::from_utf8_str(jsonClub["name"].asString());
		club.address = fm::qt::from_utf8_str(jsonClub["address"].asString());
		club.logoFile = fm::qt::from_utf8_str(jsonClub["image"].asString());

		clubs.push_back(club);
	}

	return clubs;
}

static Ipponboard::ClubList ParseIniFile(const char* filePath)
{
	QSettings settings(filePath, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");

	Ipponboard::ClubList clubs;

	for (auto const & group : settings.childGroups())
	{
		settings.beginGroup(group);
		Ipponboard::Club club;
		club.name = group;

		if (settings.contains(Tags::Address))
		{
			club.address = settings.value(Tags::Address).toString();
		}

		if (settings.contains(Tags::LogoFile))
		{
			club.logoFile = settings.value(Tags::LogoFile).toString();
		}

		settings.endGroup();

		clubs.push_back(club);
	}

	return clubs;
}

static void ToIniFile(const char* filePath, Ipponboard::ClubList const& clubs)
{
	QSettings settings(filePath, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");

	settings.clear();

	for (auto const & club : clubs)
	{
		settings.beginGroup(club.name);
		settings.setValue(Tags::Address, club.address);
		settings.setValue(Tags::LogoFile, club.logoFile);
		settings.endGroup();
	}
}

static void ToJsonFile_UNUSED(const char* filePath, Ipponboard::ClubList const& clubs)
{
	fm::Json::Value jsonClubs;

	for (Ipponboard::Club const & club : clubs)
	{
		fm::Json::Value jsonClub;
		jsonClub["name"] = fm::qt::to_utf8_str(club.name);
		jsonClub["address"] = fm::qt::to_utf8_str(club.address);
		jsonClub["image"] = fm::qt::to_utf8_str(club.logoFile);

		jsonClubs.append(jsonClub);
	}

	fm::Json::WriteFile(filePath, jsonClubs);
}

}

#endif // CLUBPARSER_H
