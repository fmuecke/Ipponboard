// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../core/Contest.cpp"
#include "../core/Contest.h"
#include "../core/Enums.h"
#include "../core/Rules.cpp"
#include "../core/Rules.h"
#include "../core/Score.cpp"
#include "../core/Score.h"

#include <catch2/catch_test_macros.hpp>

using Ipponboard::Contest;
using Ipponboard::ContestSide;
using Ipponboard::Score;
using Point = Ipponboard::Score::Point;

TEST_CASE("[Contest] Side with fewer shidos wins if points are equal (rules 2013)")
{
    auto score = Score().Add(Point::Yuko).Add(Point::Yuko);
    auto scoreWithShido = Score(score).Add(Point::Shido);
    auto scoreWithThreeShido = Score(score).Add(Point::Shido).Add(Point::Shido).Add(Point::Shido);

    Contest contest{ score, score };
    contest.rules = std::make_shared<Ipponboard::Rules2013>();

    auto sideA = ContestSide::SideA;
    auto sideB = ContestSide::SideB;

    REQUIRE_FALSE(contest.HasWon(sideB));
    REQUIRE_FALSE(contest.HasWon(sideA));

    contest.GetScore(sideA) = scoreWithShido;
    contest.GetScore(sideB) = scoreWithShido;
    REQUIRE_FALSE(contest.HasWon(sideB));
    REQUIRE_FALSE(contest.HasWon(sideA));

    contest.GetScore(sideA) = scoreWithThreeShido;
    contest.GetScore(sideB) = scoreWithThreeShido;
    REQUIRE_FALSE(contest.HasWon(sideB));
    REQUIRE_FALSE(contest.HasWon(sideA));

    contest.GetScore(sideA) = score;
    contest.GetScore(sideB) = scoreWithThreeShido;
    REQUIRE_FALSE(contest.HasWon(sideB));
    REQUIRE(contest.HasWon(sideA));

    contest.GetScore(sideA) = scoreWithShido;
    contest.GetScore(sideB) = scoreWithThreeShido;
    REQUIRE_FALSE(contest.HasWon(sideB));
    REQUIRE(contest.HasWon(sideA));
}

TEST_CASE("[Contest] Validate score points (subscore)")
{
    auto sideA = ContestSide::SideA;
    auto sideB = ContestSide::SideB;

    auto emptyScore = Score();
    auto shidoScore = Score().Add(Point::Shido);
    auto yukoScore = Score().Add(Point::Yuko);
    auto yukoWithShidoScore = Score(yukoScore).Add(Point::Shido);
    auto twoYukoScore = Score().Add(Point::Yuko).Add(Point::Yuko);
    auto twoYukoWithShidoScore = Score(twoYukoScore).Add(Point::Shido);
    auto twoYukoWithTwoShidoScore = Score(twoYukoScore).Add(Point::Shido).Add(Point::Shido);

    auto IpponScore = Score().Add(Point::Ippon);

    Contest f1{ emptyScore, shidoScore };
    f1.rules = std::make_shared<Ipponboard::Rules2013>();
    REQUIRE(f1.GetScorePoints(sideA) == 1);
    REQUIRE(f1.GetScorePoints(sideB) == 0);

    Contest f2{ yukoScore, yukoWithShidoScore };
    f2.rules = std::make_shared<Ipponboard::Rules2013>();
    REQUIRE(f2.GetScorePoints(sideA) == 1);
    REQUIRE(f2.GetScorePoints(sideB) == 0);

    // Hikewake
    Contest f3{ twoYukoScore, twoYukoScore };
    f3.rules = std::make_shared<Ipponboard::Rules2013>();
    REQUIRE(f3.GetScorePoints(sideA) == 0);
    REQUIRE(f3.GetScorePoints(sideB) == 0);

    Contest f4{ yukoScore, twoYukoScore };
    f4.rules = std::make_shared<Ipponboard::Rules2013>();
    REQUIRE(f4.GetScorePoints(sideA) == 0);
    REQUIRE(f4.GetScorePoints(sideB) == 5);

    Contest f5{ twoYukoWithShidoScore, twoYukoWithTwoShidoScore };
    f5.rules = std::make_shared<Ipponboard::Rules2013>();
    REQUIRE(f5.GetScorePoints(sideA) == 1);
    REQUIRE(f5.GetScorePoints(sideB) == 0);
}

TEST_CASE("[Contest] TimeRemaining accounts for golden score")
{
    Contest f;
    f.SetRoundTime(240);
    f.SetGoldenScore(true);
    f.SetSecondsElapsed(-65);
    REQUIRE(f.GetTimeRemainingString().toStdString() == "-1:05");
}

TEST_CASE("[Contest] TimeFaught accounts for golden score")
{
    Contest f;
    f.SetRoundTime(240);
    f.SetGoldenScore(true);
    f.SetSecondsElapsed(-65);
    REQUIRE(f.GetTotalTimeElapsedString().toStdString() == "5:05");
}

TEST_CASE("[Contest] time string with Golden Score is propertly converted")
{
    Contest f;
    f.SetRoundTime(240);
    f.SetGoldenScore(true);
    f.SetElapsedFromTotalTime("5:23");
    REQUIRE(f.GetTotalTimeElapsedString().toStdString() == "5:23");

    f.SetElapsedFromTotalTime("14:20");
    REQUIRE(f.GetTotalTimeElapsedString().toStdString() == "14:20");
}

TEST_CASE("[Contest] rules2017: score points will return 1 for shido won in golden score only")
{
    Contest f;
    f.rules = std::make_shared<Ipponboard::Rules2017>();

    REQUIRE(f.GetScorePoints(ContestSide::SideA) == 0);

    f.GetScore(ContestSide::SideA).Add(Point::Shido);
    REQUIRE(f.GetScorePoints(ContestSide::SideA) == 0);
    REQUIRE(f.GetScorePoints(ContestSide::SideB) == 0);

    f.GetScore(ContestSide::SideA).Add(Point::Shido);
    REQUIRE(f.GetScorePoints(ContestSide::SideA) == 0);
    REQUIRE(f.GetScorePoints(ContestSide::SideB) == 0);

    f.SetGoldenScore(true);
    REQUIRE(f.GetScorePoints(ContestSide::SideA) == 0);
    REQUIRE(f.GetScorePoints(ContestSide::SideB) == 1);
}

TEST_CASE(
    "[Contest] rules2017: no one has won if points are equal and shidos aren't in golden score")
{
    auto score1 = Score().Add(Point::Yuko).Add(Point::Shido);
    auto score2 = Score().Add(Point::Yuko);
    Contest contest{ score1, score2 };
    contest.rules = std::make_shared<Ipponboard::Rules2017>();

    auto sideA = ContestSide::SideA;
    auto sideB = ContestSide::SideB;

    REQUIRE_FALSE(contest.HasWon(sideB));
    REQUIRE_FALSE(contest.HasWon(sideA));
}

//TEST_CASE("Setting negative seconds enabled golden score")
//{
//    Fight f;
//    f.SetSecondsElapsed(-5);
//    REQUIRE(f.IsGoldenScore());

//    f.SetSecondsElapsed(5);
//    REQUIRE_FALSE(f.IsGoldenScore());
//}
