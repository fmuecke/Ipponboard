#pragma once

#include "../../../../devtools/Catch/include/catch.hpp"
#include "../core/Score.h"
#include "../core/Enums.h"
#include "../core/Fight.h"
#include "../core/Rules.h"

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
    fight.ruleSet = std::make_shared<Ipponboard::Rules2013>();

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

TEST_CASE("Validate score points (subscore)")
{
	auto first = FighterEnum::First;
	auto second = FighterEnum::Second;

	Fight fight;
    fight.ruleSet = std::make_shared<Ipponboard::Rules2013>();

	auto emptyScore = Score();
	auto shidoScore = Score().Add(Point::Shido);
	auto yukoScore = Score().Add(Point::Yuko);
	auto yukoWithShidoScore = Score(yukoScore).Add(Point::Shido);
	auto twoYukoScore = Score().Add(Point::Yuko).Add(Point::Yuko);
	auto twoYukoWithShidoScore = Score(twoYukoScore).Add(Point::Shido);
	auto twoYukoWithTwoShidoScore = Score(twoYukoScore).Add(Point::Shido).Add(Point::Shido);

	auto IpponScore = Score().Add(Point::Ippon);

	fight.scores[0] = emptyScore;
	fight.scores[1] = shidoScore;
	REQUIRE(fight.ScorePoints(first) == 1);
	REQUIRE(fight.ScorePoints(second) == 0);

	fight.scores[0] = yukoScore;
	fight.scores[1] = yukoWithShidoScore;
	REQUIRE(fight.ScorePoints(first) == 1);
	REQUIRE(fight.ScorePoints(second) == 0);

	// Hikewake
	fight.scores[0] = twoYukoScore;
	fight.scores[1] = twoYukoScore;
	REQUIRE(fight.ScorePoints(first) == 0);
	REQUIRE(fight.ScorePoints(second) == 0);

	fight.scores[0] = yukoScore;
	fight.scores[1] = twoYukoScore;
	REQUIRE(fight.ScorePoints(first) == 0);
	REQUIRE(fight.ScorePoints(second) == 5);

	fight.scores[0] = twoYukoWithShidoScore;
	fight.scores[1] = twoYukoWithTwoShidoScore;
	REQUIRE(fight.ScorePoints(first) == 1);
	REQUIRE(fight.ScorePoints(second) == 0);
}
