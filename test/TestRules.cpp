// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../core/Score.h"
#include "../core/Enums.h"
#include "../core/Rules.h"
//#include "../core/Fight.h"

#include <iostream>

using namespace Ipponboard;
using Point = Score::Point;

TEST_CASE("[Rules] Rulesfactory creates correct rule object")
{
	auto rules = RulesFactory::Create(ClassicRules::StaticName);
	REQUIRE(strcmp(rules->Name(), ClassicRules::StaticName) == 0);

	rules = RulesFactory::Create(Rules2013::StaticName);
	REQUIRE(strcmp(rules->Name(), Rules2013::StaticName) == 0);

	rules = RulesFactory::Create(Rules2017U15::StaticName);
	REQUIRE(strcmp(rules->Name(), Rules2017U15::StaticName) == 0);

	rules = RulesFactory::Create(Rules2017::StaticName);
	REQUIRE(strcmp(rules->Name(), Rules2017::StaticName) == 0);

	rules = RulesFactory::Create(Rules2018::StaticName);
	REQUIRE(strcmp(rules->Name(), Rules2018::StaticName) == 0);

	rules = RulesFactory::Create(Rules2025::StaticName);
	REQUIRE(strcmp(rules->Name(), Rules2025::StaticName) == 0);
}

TEST_CASE("[Rules] Default rules are IJF-2025")
{
	auto rules = RulesFactory::Create("");
	REQUIRE(strcmp(rules->Name(), Rules2025::StaticName) == 0);
}

TEST_CASE("[Rules] String list contains all rules")
{
	auto names = RulesFactory::GetNames();
	REQUIRE(names.contains(ClassicRules::StaticName));
	REQUIRE(names.contains(Rules2013::StaticName));
	REQUIRE(names.contains(Rules2017U15::StaticName));
	REQUIRE(names.contains(Rules2017::StaticName));
	REQUIRE(names.contains(Rules2018::StaticName));
	REQUIRE(names.contains(Rules2025::StaticName));
	REQUIRE(names.size() == 6);
}

TEST_CASE("[Rules] 2025: osaekomi values")
{
	auto rules = std::make_shared<Rules2025>();
	
	INFO("ippon is gained after 20 seconds")
	REQUIRE(rules->GetOsaekomiValue(Score::Point::Ippon) == 20);

	INFO("wazaari is gained after 10 seconds")
	REQUIRE(rules->GetOsaekomiValue(Score::Point::Wazaari) == 10);

	INFO("yuko is gained after 5 seconds")
	REQUIRE(rules->GetOsaekomiValue(Score::Point::Yuko) == 5);
}