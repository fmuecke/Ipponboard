// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "FightCategoryParser.h"
#include "../util/qt_helpers.hpp"
#include "../util/json.hpp"

#include <QSettings>

namespace Tags
{
static const char* const Weights = "Weights";
static const char* const RoundTime = "RoundTimeInSecs";
static const char* const GoldenScoreTime = "GoldenScoreTimeInSecs";
}

FightCategoryParser::FightCategoryParser()
{

}

void FightCategoryParser::ToIniFile(const char* file, Ipponboard::FightCategoryList const& categories)
{
	QSettings settings(file, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");
	settings.clear();

	for (auto const & cat : categories)
	{
		settings.beginGroup(cat.ToString());
		{
			settings.setValue(Tags::RoundTime, cat.GetRoundTime());
			settings.setValue(Tags::GoldenScoreTime, cat.GetGoldenScoreTime());
			settings.setValue(Tags::Weights, cat.GetWeights());
		}
		settings.endGroup();
	}
}

Ipponboard::FightCategoryList FightCategoryParser::ParseIniFile(const char* file)
{
	QSettings settings(file, QSettings::IniFormat);
	settings.setIniCodec("UTF-8");

	Ipponboard::FightCategoryList categories;

	for (auto const & group : settings.childGroups())
	{
		Ipponboard::FightCategory cat(group);
		settings.beginGroup(group);
		{
			if (settings.contains(Tags::RoundTime))
			{
				auto secs = settings.value(Tags::RoundTime).toInt();
				cat.SetRoundTime(secs);
			}

			if (settings.contains(Tags::GoldenScoreTime))
			{
				auto secs = settings.value(Tags::GoldenScoreTime).toInt();
				cat.SetGoldenScoreTime(secs);
			}

			if (settings.contains(Tags::Weights))
			{
				auto val = settings.value(Tags::Weights).toString();
				cat.SetWeights(val);
			}
		}
		settings.endGroup();

		categories.push_back(cat);
	}

	return categories;
}


std::string FightCategoryParser::ToJsonString(Ipponboard::FightCategoryList const& categories)
{
	fm::Json::Value jsonCategories;

	for (Ipponboard::FightCategory const & cat : categories)
	{
		fm::Json::Value jsonCat;
		jsonCat["name"] = fm::qt::to_utf8_str(cat.ToString());
		jsonCat["round_time_secs"] = cat.GetRoundTime();
		jsonCat["golden_score_time_secs"] = cat.GetGoldenScoreTime();
		jsonCat["weights"] = fm::qt::to_utf8_str(cat.GetWeights());

		jsonCategories.append(jsonCat);
	}

	return jsonCategories.toStyledString();
}

Ipponboard::FightCategoryList ParseJson(Json::Value const& json)
{
	Ipponboard::FightCategoryList categories;

	for (fm::Json::Value const & jsonCat : json)
	{
		Ipponboard::FightCategory cat(fm::qt::from_utf8_str(jsonCat["name"].asString()));
		cat.SetRoundTime(jsonCat["round_time_secs"].asInt());
		cat.SetGoldenScoreTime(jsonCat["golden_score_time_secs"].asInt());
		cat.SetWeights(fm::qt::from_utf8_str(jsonCat["weights"].asString()));

		categories.push_back(cat);
	}

	return categories;
}

// May throw exception!
Ipponboard::FightCategoryList FightCategoryParser::ParseJsonString(std::string const& jsonString)
{
	auto json = fm::Json::ReadString(jsonString);
	return ParseJson(json);
}

// May throw exception!
Ipponboard::FightCategoryList FightCategoryParser::ParseJsonFile(const char* file)
{
	auto json = fm::Json::ReadFile(file);
	return ParseJson(json);
}

