#pragma once

#include "../core/Score.h"
#include "../core/Enums.h"

using Point = Ipponboard::Score::Point;

TEST_CASE("Shido rules for fights")
{
	auto empty = Score();
	auto shido = Score().Add(Point::Shido);
	auto twoShido = Score().Add(Point::Shido).Add(Point::Shido);
	auto yukoWithShido = Score().Add(Point::Yuko).Add(Point::Shido);
	auto yukoWithTwoShido = Score().Add(Point::Yuko).Add(Point::Shido).Add(Point::Shido);
	auto wazaari = Score().Add(Point::Wazaari);
	auto wazaariWithShido = Score().Add(Point::Wazaari).Add(Point::Shido);
	auto wazaariWithTwoShido = Score().Add(Point::Wazaari).Add(Point::Shido).Add(Point::Shido);
	auto wazaariWithThreeShido = Score().Add(Point::Wazaari).Add(Point::Shido).Add(Point::Shido).Add(Point::Shido);

	REQUIRE_FALSE(shido.IsLess(shido, e2013RuleSet));
	REQUIRE(shido.IsLess(empty, e2013RuleSet));
	REQUIRE(twoShido.IsLess(shido, e2013RuleSet));
	REQUIRE(shido.IsLess(yukoWithTwoShido, e2013RuleSet));
	REQUIRE(twoShido.IsLess(yukoWithTwoShido, e2013RuleSet));
	REQUIRE(empty.IsLess(yukoWithTwoShido, e2013RuleSet));
	REQUIRE(wazaariWithShido.IsLess(wazaari, e2013RuleSet));
	REQUIRE(wazaariWithTwoShido.IsLess(wazaari, e2013RuleSet));
	REQUIRE(wazaariWithTwoShido.IsLess(wazaariWithShido, e2013RuleSet));
	REQUIRE(wazaariWithThreeShido.IsLess(wazaariWithShido, e2013RuleSet));
	REQUIRE(wazaariWithThreeShido.IsLess(wazaariWithTwoShido, e2013RuleSet));

	// classic rules
	REQUIRE_FALSE(shido.IsLess(empty, eClassicRules));
	REQUIRE_FALSE(shido.IsLess(shido, eClassicRules));
	REQUIRE(twoShido.IsLess(yukoWithShido, eClassicRules));
	REQUIRE(shido.IsLess(yukoWithTwoShido, eClassicRules));
	REQUIRE(twoShido.IsLess(yukoWithTwoShido, eClassicRules));
	REQUIRE(empty.IsLess(yukoWithTwoShido, eClassicRules));
}

//TEST_CASE("4th shido sets hansokumake")
//{
//	Score one;
//	Score two;
//
//	one.Add(Point::Shido);
//	one.Add(Point::Shido);
//	one.Add(Point::Shido);
//	one.Add(Point::Shido);
//	two.Add(Hansokumake);
//
//	one.Hansokumake
//	REQUIRE_FALSE(one.IsLess(two));
//	REQUIRE_FALSE(two.IsLess(one));
//}

TEST_CASE("Score supports two Hansokumake")
{
	Score score1;
	Score score2;

	score1.Add(Score::Point::Hansokumake);
	score2.Add(Score::Point::Hansokumake);
	
	REQUIRE_FALSE(score1.IsLess(score2, Ipponboard::eClassicRules));
	REQUIRE_FALSE(score2.IsLess(score1, Ipponboard::eClassicRules));

	REQUIRE_FALSE(score1.IsLess(score2, Ipponboard::e2013RuleSet));
	REQUIRE_FALSE(score2.IsLess(score1, Ipponboard::e2013RuleSet));
}
