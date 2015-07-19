#pragma once

#include "../core/Score.h"
#include "../core/Enums.h"

using Ipponboard::Score;
using Ipponboard::EPoint;


TEST_CASE("Shido rules for fights")
{
	auto empty = Score();
	auto shido = Score().Add(ePoint_Shido);
	auto twoShido = Score().Add(ePoint_Shido).Add(ePoint_Shido);
	auto yukoWithShido = Score().Add(ePoint_Yuko).Add(ePoint_Shido);
	auto yukoWithTwoShido = Score().Add(ePoint_Yuko).Add(ePoint_Shido).Add(ePoint_Shido);
	auto wazaari = Score().Add(ePoint_Wazaari);
	auto wazaariWithShido = Score().Add(ePoint_Wazaari).Add(ePoint_Shido);
	auto wazaariWithTwoShido = Score().Add(ePoint_Wazaari).Add(ePoint_Shido).Add(ePoint_Shido);
	auto wazaariWithThreeShido = Score().Add(ePoint_Wazaari).Add(ePoint_Shido).Add(ePoint_Shido).Add(ePoint_Shido);

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
//	one.Add(ePoint_Shido);
//	one.Add(ePoint_Shido);
//	one.Add(ePoint_Shido);
//	one.Add(ePoint_Shido);
//	two.Add(ePoint_Hansokumake);
//
//	one.Hansokumake
//	REQUIRE_FALSE(one.IsLess(two));
//	REQUIRE_FALSE(two.IsLess(one));
//}

TEST_CASE("Score supports two Hansokumake")
{
	Score score1;
	Score score2;

	score1.Add(ePoint_Hansokumake);
	score2.Add(ePoint_Hansokumake);
	
	REQUIRE_FALSE(score1.IsLess(score2, Ipponboard::eClassicRules));
	REQUIRE_FALSE(score2.IsLess(score1, Ipponboard::eClassicRules));

	REQUIRE_FALSE(score1.IsLess(score2, Ipponboard::e2013RuleSet));
	REQUIRE_FALSE(score2.IsLess(score1, Ipponboard::e2013RuleSet));
}
