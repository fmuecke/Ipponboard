#include "../util/catch/catch.hpp"
#include "../core/Score.h"
#include "../core/Enums.h"
#include "../core/Rules.h"
//#include "../core/Fight.h"

#include <iostream>

using namespace Ipponboard;
using Point = Score::Point;

TEST_CASE("Rulesfactory creates correct rule object")
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
}

TEST_CASE("Default rules are IJF-2018")
{
	auto rules = RulesFactory::Create("");
	REQUIRE(strcmp(rules->Name(), Rules2018::StaticName) == 0);
}