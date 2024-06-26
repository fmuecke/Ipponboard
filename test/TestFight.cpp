// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../core/Score.h"
#include "../core/Enums.h"
#include "../core/Fight.h"
#include "../core/Rules.h"

#include "../core/Score.cpp"
#include "../core/Fight.cpp"
#include "../core/Rules.cpp"

using Ipponboard::Fight;
using Ipponboard::Score;
using Ipponboard::FighterEnum;
using Point = Ipponboard::Score::Point;

TEST_CASE("[Fight] Fighter with less Shidos wins if points are equal (rules 2013)")
{
	auto score = Score().Add(Point::Yuko).Add(Point::Yuko);
	auto scoreWithShido = Score(score).Add(Point::Shido);
	auto scoreWithThreeShido = Score(score).Add(Point::Shido).Add(Point::Shido).Add(Point::Shido);

	Fight fight { score, score };
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

TEST_CASE("[Fight] Validate score points (subscore)")
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

	Fight f1 { emptyScore, shidoScore };
	f1.rules = std::make_shared<Ipponboard::Rules2013>();
	REQUIRE(f1.GetScorePoints(first) == 1);
	REQUIRE(f1.GetScorePoints(second) == 0);

	Fight f2 { yukoScore, yukoWithShidoScore };
	f2.rules = std::make_shared<Ipponboard::Rules2013>();
	REQUIRE(f2.GetScorePoints(first) == 1);
	REQUIRE(f2.GetScorePoints(second) == 0);

	// Hikewake
	Fight f3 { twoYukoScore, twoYukoScore };
	f3.rules = std::make_shared<Ipponboard::Rules2013>();
	REQUIRE(f3.GetScorePoints(first) == 0);
	REQUIRE(f3.GetScorePoints(second) == 0);

	Fight f4 { yukoScore, twoYukoScore };
	f4.rules = std::make_shared<Ipponboard::Rules2013>();
	REQUIRE(f4.GetScorePoints(first) == 0);
	REQUIRE(f4.GetScorePoints(second) == 5);

	Fight f5 { twoYukoWithShidoScore, twoYukoWithTwoShidoScore };
	f5.rules = std::make_shared<Ipponboard::Rules2013>();
	REQUIRE(f5.GetScorePoints(first) == 1);
	REQUIRE(f5.GetScorePoints(second) == 0);
}

TEST_CASE("[Fight] TimeRemaining accounts for golden score")
{
	Fight f;
	f.SetRoundTime(240);
	f.SetGoldenScore(true);
	f.SetSecondsElapsed(-65);
	REQUIRE(f.GetTimeRemainingString().toStdString() == "-1:05");
}

TEST_CASE("[Fight] TimeFaught accounts for golden score")
{
	Fight f;
	f.SetRoundTime(240);
	f.SetGoldenScore(true);
	f.SetSecondsElapsed(-65);
	REQUIRE(f.GetTotalTimeElapsedString().toStdString() == "5:05");
}

TEST_CASE("[Fight] time string with Golden Score is propertly converted")
{
	Fight f;
	f.SetRoundTime(240);
	f.SetGoldenScore(true);
	f.SetElapsedFromTotalTime("5:23");
	REQUIRE(f.GetTotalTimeElapsedString().toStdString() == "5:23");

	f.SetElapsedFromTotalTime("14:20");
	REQUIRE(f.GetTotalTimeElapsedString().toStdString() == "14:20");
}

TEST_CASE("[Fight] rules2017: score points will return 1 for shido won in golden score only")
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

TEST_CASE("[Fight] rules2017: no one has won if points are equal and shidos aren't in golden score")
{
	auto score1 = Score().Add(Point::Yuko).Add(Point::Shido);
	auto score2 = Score().Add(Point::Yuko);
	Fight fight { score1, score2 };
	fight.rules = std::make_shared<Ipponboard::Rules2017>();

	auto first = FighterEnum::First;
	auto second = FighterEnum::Second;

	REQUIRE_FALSE(fight.HasWon(second));
	REQUIRE_FALSE(fight.HasWon(first));
}

//TEST_CASE("Setting negative seconds enabled golden score")
//{
//    Fight f;
//    f.SetSecondsElapsed(-5);
//    REQUIRE(f.IsGoldenScore());

//    f.SetSecondsElapsed(5);
//    REQUIRE_FALSE(f.IsGoldenScore());
//}
