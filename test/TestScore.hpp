#pragma once

#include "../../../../devtools/Catch/include/catch.hpp"
#include "../core/Score.h"
#include "../core/Enums.h"
#include "../core/Rules.h"

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

    auto rules2013 = std::make_shared<Ipponboard::Rules2013>();
    REQUIRE_FALSE(rules2013->IsScoreLess(shido, shido));
    REQUIRE(rules2013->IsScoreLess(shido, empty));
    REQUIRE(rules2013->IsScoreLess(twoShido, shido));
    REQUIRE(rules2013->IsScoreLess(shido, yukoWithTwoShido));
    REQUIRE(rules2013->IsScoreLess(twoShido, yukoWithTwoShido));
    REQUIRE(rules2013->IsScoreLess(empty, yukoWithTwoShido));
    REQUIRE(rules2013->IsScoreLess(wazaariWithShido, wazaari));
    REQUIRE(rules2013->IsScoreLess(wazaariWithTwoShido, wazaari));
    REQUIRE(rules2013->IsScoreLess(wazaariWithTwoShido, wazaariWithShido));
    REQUIRE(rules2013->IsScoreLess(wazaariWithThreeShido, wazaariWithShido));
    REQUIRE(rules2013->IsScoreLess(wazaariWithThreeShido, wazaariWithTwoShido));

	// classic rules
    auto classicRules = std::make_shared<Ipponboard::ClassicRules>();
    REQUIRE_FALSE(classicRules->IsScoreLess(shido, empty));
    REQUIRE_FALSE(classicRules->IsScoreLess(shido, shido));
    REQUIRE(classicRules->IsScoreLess(twoShido, yukoWithShido));
    REQUIRE(classicRules->IsScoreLess(shido, yukoWithTwoShido));
    REQUIRE(classicRules->IsScoreLess(twoShido, yukoWithTwoShido));
    REQUIRE(classicRules->IsScoreLess(empty, yukoWithTwoShido));
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

TEST_CASE("Each fighter can have Hansokumake")
{
	Score score1;
	Score score2;
    auto classicRules = std::make_shared<Ipponboard::ClassicRules>();
    auto rules2013 = std::make_shared<Ipponboard::Rules2013>();

	score1.Add(Score::Point::Hansokumake);

    REQUIRE(classicRules->IsScoreLess(score1, score2));
    REQUIRE_FALSE(classicRules->IsScoreLess(score2, score1));

    REQUIRE(rules2013->IsScoreLess(score1, score2));
    REQUIRE_FALSE(rules2013->IsScoreLess(score2, score1));

    score2.Add(Score::Point::Hansokumake);
	
    REQUIRE_FALSE(classicRules->IsScoreLess(score1, score2));
    REQUIRE_FALSE(classicRules->IsScoreLess(score2, score1));

    REQUIRE_FALSE(rules2013->IsScoreLess(score1, score2));
    REQUIRE_FALSE(rules2013->IsScoreLess(score2, score1));
}

TEST_CASE("is awasette ippon")
{
    Score score;
    auto classicRules = std::make_shared<Ipponboard::ClassicRules>();
    auto rules2013 = std::make_shared<Ipponboard::Rules2013>();
    auto rules2017 = std::make_shared<Ipponboard::Rules2017>();

    REQUIRE_FALSE(classicRules->IsAwaseteIppon(score));
    REQUIRE_FALSE(rules2013->IsAwaseteIppon(score));
    REQUIRE_FALSE(rules2017->IsAwaseteIppon(score));

    score.Add(Score::Point::Wazaari);

    REQUIRE_FALSE(classicRules->IsAwaseteIppon(score));
    REQUIRE_FALSE(rules2013->IsAwaseteIppon(score));
    REQUIRE_FALSE(rules2017->IsAwaseteIppon(score));

    score.Add(Score::Point::Wazaari);

    REQUIRE(classicRules->IsAwaseteIppon(score));
    REQUIRE(rules2013->IsAwaseteIppon(score));
    REQUIRE_FALSE(rules2017->IsAwaseteIppon(score));
}
