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

	Fight fight{ score, score };
	fight.rules = std::make_shared<Ipponboard::Rules2013>();

	auto first = FighterEnum::First;
	auto second = FighterEnum::Second;

	REQUIRE_FALSE(fight.HasWon(second));
	REQUIRE_FALSE(fight.HasWon(first));

	fight.GetScore(first) = scoreWithShido;
	fight.GetScore(second) = scoreWithShido;
	REQUIRE_FALSE(fight.HasWon(second));
	REQUIRE_FALSE(fight.HasWon(first));

	fight.GetScore(first) = scoreWithThreeShido;
	fight.GetScore(second) = scoreWithThreeShido;
	REQUIRE_FALSE(fight.HasWon(second));
	REQUIRE_FALSE(fight.HasWon(first));

	fight.GetScore(first) = score;
	fight.GetScore(second) = scoreWithThreeShido;
	REQUIRE_FALSE(fight.HasWon(second));
	REQUIRE(fight.HasWon(first));

	fight.GetScore(first) = scoreWithShido;
	fight.GetScore(second) = scoreWithThreeShido;
	REQUIRE_FALSE(fight.HasWon(second));
	REQUIRE(fight.HasWon(first));
}

TEST_CASE("Validate score points (subscore)")
{
	auto first = FighterEnum::First;
	auto second = FighterEnum::Second;

	auto emptyScore = Score();
	auto shidoScore = Score().Add(Point::Shido);
	auto yukoScore = Score().Add(Point::Yuko);
	auto yukoWithShidoScore = Score(yukoScore).Add(Point::Shido);
	auto twoYukoScore = Score().Add(Point::Yuko).Add(Point::Yuko);
	auto twoYukoWithShidoScore = Score(twoYukoScore).Add(Point::Shido);
	auto twoYukoWithTwoShidoScore = Score(twoYukoScore).Add(Point::Shido).Add(Point::Shido);

	auto IpponScore = Score().Add(Point::Ippon);

	Fight f1{ emptyScore, shidoScore };
	f1.rules = std::make_shared<Ipponboard::Rules2013>();
	REQUIRE(f1.GetScorePoints(first) == 1);
	REQUIRE(f1.GetScorePoints(second) == 0);

	Fight f2{ yukoScore, yukoWithShidoScore };
	f2.rules = std::make_shared<Ipponboard::Rules2013>();
	REQUIRE(f2.GetScorePoints(first) == 1);
	REQUIRE(f2.GetScorePoints(second) == 0);

	// Hikewake
	Fight f3{ twoYukoScore, twoYukoScore };
	f3.rules = std::make_shared<Ipponboard::Rules2013>();
	REQUIRE(f3.GetScorePoints(first) == 0);
	REQUIRE(f3.GetScorePoints(second) == 0);

	Fight f4{ yukoScore, twoYukoScore };
	f4.rules = std::make_shared<Ipponboard::Rules2013>();
	REQUIRE(f4.GetScorePoints(first) == 0);
	REQUIRE(f4.GetScorePoints(second) == 5);

	Fight f5{ twoYukoWithShidoScore, twoYukoWithTwoShidoScore };
	f5.rules = std::make_shared<Ipponboard::Rules2013>();
	REQUIRE(f5.GetScorePoints(first) == 1);
	REQUIRE(f5.GetScorePoints(second) == 0);
}

TEST_CASE("TimeRemaining accounts for golden score")
{
	Fight f;
	f.max_time_in_seconds = 240;
	f.time_in_seconds = -65;
	REQUIRE(f.GetTimeRemaining().toStdString() == "-1:05");
}

TEST_CASE("TimeFaught accounts for golden score")
{
	Fight f;
	f.max_time_in_seconds = 240;
	f.time_in_seconds = -65;
	REQUIRE(f.GetTimeFaught().toStdString() == "5:05");
}

TEST_CASE("rules2017: score points will return 1 for shido won in golden score only")
{
	Fight f;
	f.rules = std::make_shared<Ipponboard::Rules2017>();

	REQUIRE(f.GetScorePoints(FighterEnum::First) == 0);

	f.GetScore(FighterEnum::First).Add(Point::Shido);
	REQUIRE(f.GetScorePoints(FighterEnum::First) == 0);
	REQUIRE(f.GetScorePoints(FighterEnum::Second) == 0);

	f.GetScore(FighterEnum::First).Add(Point::Shido);
	REQUIRE(f.GetScorePoints(FighterEnum::First) == 0);
	REQUIRE(f.GetScorePoints(FighterEnum::Second) == 0);

	f.SetGoldenScore(true);
	REQUIRE(f.GetScorePoints(FighterEnum::First) == 0);
	REQUIRE(f.GetScorePoints(FighterEnum::Second) == 1);
}