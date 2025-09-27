// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../core/TournamentMode.h"
#include "../core/TournamentMode.cpp"

#include <QString>
#include <QDir>
#include <iostream>
#include "TestDataPath.h"

using namespace Ipponboard;

struct IpponboardTest
{

	static bool parse_group(QSettings& config, QString const& group, QString& errorMsg)
	{
		TournamentMode tm;
		errorMsg.clear();

		config.beginGroup(group);
		static const QString templateDir = QString::fromStdString(ippn_test_data_root().string());
		bool readSuccess = TournamentMode::parse_current_group(config, tm, templateDir, errorMsg);
		config.endGroup();
		return readSuccess;
	}
};

TEST_CASE("[TournamentMode] Test_parse_current_group")
{
	const auto iniFile = QString::fromStdString(ippn_test_data_path("TournamentModes-test.ini").string());
	QSettings config(iniFile, QSettings::IniFormat);
	QStringList groups;
	groups << "basic"
	       << "with_weights_doubled"
	       << "with_time_overrides"
	       << "template_not_found"
	       << "no_title"
	       << "no_weights"
	       << "no_rounds"
	       << "no_template"
	       << "no_fight_time";

	REQUIRE(groups.count() == config.childGroups().count());

	QString errorMsg;

	bool success = IpponboardTest::parse_group(config, "basic", errorMsg);
	INFO(errorMsg.toStdString());
	REQUIRE(success);

	success = IpponboardTest::parse_group(config, "with_weights_doubled", errorMsg);
	INFO(errorMsg.toStdString());
	REQUIRE(success);

	success = IpponboardTest::parse_group(config, "with_time_overrides", errorMsg);
	INFO(errorMsg.toStdString());
	REQUIRE(success);

	success = IpponboardTest::parse_group(config, "template_not_found", errorMsg);
	INFO(errorMsg.toStdString());
	REQUIRE_FALSE(success);

	success = IpponboardTest::parse_group(config, "no_title", errorMsg);
	INFO(errorMsg.toStdString());
	REQUIRE_FALSE(success);

	success = IpponboardTest::parse_group(config, "no_weights", errorMsg);
	INFO(errorMsg.toStdString());
	REQUIRE_FALSE(success);

	success = IpponboardTest::parse_group(config, "no_rounds", errorMsg);
	INFO(errorMsg.toStdString());
	REQUIRE_FALSE(success);

	success = IpponboardTest::parse_group(config, "no_template", errorMsg);
	INFO(errorMsg.toStdString());
	REQUIRE_FALSE(success);

	success = IpponboardTest::parse_group(config, "no_fight_time", errorMsg);
	INFO(errorMsg.toStdString());
	REQUIRE_FALSE(success);
}
