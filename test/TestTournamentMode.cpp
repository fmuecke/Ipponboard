// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../core/TournamentMode.h"
#include "../core/TournamentMode.cpp"

#include <QString>
#include <iostream>

using namespace Ipponboard;

struct IpponboardTest
{

	static bool parse_group(QSettings& config, QString const& group)
	{
		TournamentMode tm;

		config.beginGroup(group);
		QString errorMsg;
		bool readSuccess = TournamentMode::parse_current_group(config, tm, errorMsg);

		if (!readSuccess)
		{
			//std::cout << "--> Error in 'parse_group': " << errorMsg.toStdString() << std::endl;
		}

		config.endGroup();
		return readSuccess;
	}

};

TEST_CASE("Test_parse_current_group")
{
	QSettings config("TestData/TournamentModes-test.ini", QSettings::IniFormat, nullptr);
	QStringList groups;
	groups
			<< "basic"
			<< "with_weights_doubled"
			<< "with_time_overrides"
			<< "template_not_found"
			<< "no_title"
			<< "no_weights"
			<< "no_rounds"
			<< "no_template"
			<< "no_fight_time";

	REQUIRE(groups.count() == config.childGroups().count());

	REQUIRE(IpponboardTest::parse_group(config, "basic"));
	REQUIRE(IpponboardTest::parse_group(config, "with_weights_doubled"));
	REQUIRE(IpponboardTest::parse_group(config, "with_time_overrides"));
	REQUIRE_FALSE(IpponboardTest::parse_group(config, "template_not_found"));
	REQUIRE_FALSE(IpponboardTest::parse_group(config, "no_title"));
	REQUIRE_FALSE(IpponboardTest::parse_group(config, "no_weights"));
	REQUIRE_FALSE(IpponboardTest::parse_group(config, "no_rounds"));
	REQUIRE_FALSE(IpponboardTest::parse_group(config, "no_template"));
	REQUIRE_FALSE(IpponboardTest::parse_group(config, "no_fight_time"));

	//QCOMPARE(str.toUpper(), QString("HELLO"));
}

