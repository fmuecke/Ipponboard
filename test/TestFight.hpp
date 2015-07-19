#pragma once

#include "../core/Score.h"
#include "../core/Enums.h"
#include "../core/Fight.h"

using Ipponboard::Fight;
using Ipponboard::Score;
using Ipponboard::FighterEnum;
using Point = Ipponboard::Score::Point;

TEST_CASE("Fighter with less Shidos wins if points are equal")
{
	auto score = Score().Add(Point::Yuko).Add(Point::Yuko);
	auto scoreWithShido = Score(score).Add(Point::Shido);
	auto scoreWithThreeShido = Score(score).Add(Point::Shido).Add(Point::Shido).Add(Point::Shido);

	Fight fight;
	fight.ruleSet = e2013RuleSet;

	fight.scores[0] = score;
	fight.scores[1] = score;

	auto first = FighterEnum::First;
	auto second = FighterEnum::Second;

	REQUIRE_FALSE(fight.HasWon(second));
	REQUIRE_FALSE(fight.HasWon(first));

	fight.scores[0] = scoreWithShido;
	fight.scores[1] = scoreWithShido;
	REQUIRE_FALSE(fight.HasWon(second));
	REQUIRE_FALSE(fight.HasWon(first));

	fight.scores[0] = scoreWithThreeShido;
	fight.scores[1] = scoreWithThreeShido;
	REQUIRE_FALSE(fight.HasWon(second));
	REQUIRE_FALSE(fight.HasWon(first));

	fight.scores[0] = score;
	fight.scores[1] = scoreWithThreeShido;
	REQUIRE_FALSE(fight.HasWon(second));
	REQUIRE(fight.HasWon(first));

	fight.scores[0] = scoreWithShido;
	fight.scores[1] = scoreWithThreeShido;
	REQUIRE_FALSE(fight.HasWon(second));
	REQUIRE(fight.HasWon(first));
}
