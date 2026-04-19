#include "../core/Enums.h"
#include "../core/Rules.h"
#include "ControllerTestHelpers.h"

#include <QStringList>
#include <QTime>
#include <catch2/catch_test_macros.hpp>

using namespace Ipponboard;
using namespace TestSupport;

TEST_CASE("[Controller] Main timer stops when contest time elapses")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    controller.SetRoundTime(QTime(0, 0, 3));

    REQUIRE(controller.GetCurrentState() == eState_TimerStopped);

    controller.DoAction(eAction_Hajime_Mate);
    REQUIRE(controller.GetCurrentState() == eState_TimerRunning);

    controller.AdvanceTimerTicks(eTimer_Main, 2);
    REQUIRE(controller.GetCurrentState() == eState_TimerRunning);

    controller.AdvanceTimerTicks(eTimer_Main, 2);
    REQUIRE(controller.GetCurrentState() == eState_TimerStopped);
    REQUIRE(controller.GetTimeText(eTimer_Main) == QStringLiteral("0:00"));
}

TEST_CASE("[Controller] Golden score wazaari revocation")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    controller.SetRoundTime(QTime(0, 0, 3));
    controller.SetGoldenScore(true);

    controller.DoAction(eAction_Hajime_Mate);
    REQUIRE(controller.GetCurrentState() == eState_TimerRunning);

    controller.DoAction(eAction_Wazaari, ContestSide::SideA);
    REQUIRE(controller.GetScore(ContestSide::SideA, Score::Point::Wazaari) == 1);
    REQUIRE(controller.GetCurrentState() == eState_TimerStopped);

    controller.DoAction(eAction_Wazaari, ContestSide::SideA, true);
    REQUIRE(controller.GetScore(ContestSide::SideA, Score::Point::Wazaari) == 0);
    REQUIRE(controller.GetCurrentState() == eState_TimerStopped);

    controller.DoAction(eAction_Hajime_Mate);
    REQUIRE(controller.GetCurrentState() == eState_TimerRunning);

    controller.AdvanceTimerTicks(eTimer_Main, 1);
    REQUIRE(controller.GetCurrentState() == eState_TimerRunning);
}

TEST_CASE("[Controller] Golden score shido revocation keeps the contest running")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    controller.SetRules(std::make_shared<ClassicRules>());
    controller.SetRoundTime(QTime(0, 0, 45));
    controller.SetGoldenScore(true);

    controller.DoAction(eAction_Wazaari, ContestSide::SideA);
    controller.DoAction(eAction_Shido, ContestSide::SideB);
    controller.DoAction(eAction_Shido, ContestSide::SideB);
    REQUIRE(controller.GetScore(ContestSide::SideB, Score::Point::Shido) == 2);

    controller.DoAction(eAction_Hajime_Mate);
    REQUIRE(controller.GetCurrentState() == eState_TimerRunning);

    controller.DoAction(eAction_Shido, ContestSide::SideB, true);
    REQUIRE(controller.GetScore(ContestSide::SideB, Score::Point::Shido) == 1);
    REQUIRE(controller.GetCurrentState() == eState_TimerRunning);
}

TEST_CASE("[Controller] Hold scoring awards points over time")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    controller.SetRoundTime(QTime(0, 3, 0));

    controller.DoAction(eAction_Hajime_Mate);
    REQUIRE(controller.GetCurrentState() == eState_TimerRunning);

    controller.DoAction(eAction_OsaeKomi_Toketa, ContestSide::SideA);
    REQUIRE(controller.GetCurrentState() == eState_Holding);

    controller.AdvanceTimerTicks(eTimer_Hold, 5);
    REQUIRE(controller.GetScore(ContestSide::SideA, Score::Point::Yuko) == 1);
    REQUIRE(controller.GetCurrentState() == eState_Holding);

    controller.AdvanceTimerTicks(eTimer_Hold, 5);
    REQUIRE(controller.GetScore(ContestSide::SideA, Score::Point::Wazaari) == 1);
    REQUIRE(controller.GetCurrentState() == eState_Holding);

    controller.AdvanceTimerTicks(eTimer_Hold, 10);
    REQUIRE(controller.GetScore(ContestSide::SideA, Score::Point::Ippon) == 1);
    REQUIRE(controller.GetCurrentState() == eState_TimerStopped);
}

TEST_CASE("[Controller] Auto adjust hold respects active rule thresholds")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    auto rules = std::make_shared<Ipponboard::Rules2025>();
    controller.SetRules(rules);
    controller.SetAutoAdjustPoints(true);

    fixture.startFight();
    fixture.beginHold(ContestSide::SideA);

    const auto yukoAt = rules->GetOsaekomiValue(Score::Point::Yuko);
    const auto wazaariAt = rules->GetOsaekomiValue(Score::Point::Wazaari);
    const auto ipponAt = rules->GetOsaekomiValue(Score::Point::Ippon);

    REQUIRE(yukoAt > 0);
    REQUIRE(wazaariAt > yukoAt);
    REQUIRE(ipponAt > wazaariAt);

    fixture.advanceHoldTime(yukoAt - 1);
    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Yuko) == 0);

    fixture.advanceHoldTime(1);
    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Yuko) == 1);
    CHECK(controller.GetCurrentState() == eState_Holding);

    fixture.advanceHoldTime(wazaariAt - yukoAt);
    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Yuko) == 0);
    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Wazaari) == 1);
    CHECK(controller.GetCurrentState() == eState_Holding);

    fixture.advanceHoldTime(ipponAt - wazaariAt);
    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Wazaari) == 0);
    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Ippon) == 1);
    CHECK(controller.GetCurrentState() == eState_TimerStopped);
}

TEST_CASE("[Controller] Auto adjust second hold delivers awasete ippon")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    auto rules = std::make_shared<Ipponboard::Rules2025>();
    controller.SetRules(rules);
    controller.SetAutoAdjustPoints(true);

    fixture.startFight();
    fixture.beginHold(ContestSide::SideA);

    const auto wazaariAt = rules->GetOsaekomiValue(Score::Point::Wazaari);
    REQUIRE(wazaariAt > 0);

    fixture.advanceHoldTime(wazaariAt);
    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Wazaari) == 1);
    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Ippon) == 0);
    CHECK(controller.GetCurrentState() == eState_Holding);

    controller.DoAction(eAction_OsaeKomi_Toketa, ContestSide::SideA);
    CHECK(controller.GetCurrentState() == eState_TimerRunning);
    controller.DoAction(eAction_ResetOsaeKomi, ContestSide::SideA, true);

    fixture.beginHold(ContestSide::SideA);
    fixture.advanceHoldTime(wazaariAt);

    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Wazaari) == 2);
    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Ippon) == 1);
    CHECK(controller.GetCurrentState() == eState_TimerStopped);
}

TEST_CASE("[Controller] Hold owner follows engine state across contest changes")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initCompetition(1, { QStringLiteral("-60"), QStringLiteral("-66") });

    fixture.startFight();
    fixture.beginHold(ContestSide::SideB);

    REQUIRE(controller.GetCurrentHoldSide() == ContestSide::SideB);
    REQUIRE(controller.GetLeadingSide() == ContestSide::SideB);

    controller.NextContest();

    CHECK(controller.GetCurrentHoldSide() == ContestSide::None);
    CHECK(controller.GetCurrentState() == eState_TimerStopped);
}

TEST_CASE("[Controller] NextContest advances across rounds")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initCompetition(2, { QStringLiteral("-60"), QStringLiteral("-66") });

    REQUIRE(controller.GetRoundCount() == 2);
    REQUIRE(controller.GetContestCount() == 2);
    REQUIRE(controller.GetCurrentRound() == 0);
    REQUIRE(controller.GetCurrentContest() == 0);

    controller.NextContest();
    CHECK(controller.GetCurrentRound() == 0);
    CHECK(controller.GetCurrentContest() == 1);

    controller.NextContest();
    CHECK(controller.GetCurrentRound() == 1);
    CHECK(controller.GetCurrentContest() == 0);
}

TEST_CASE("[Controller] PrevContest wraps to previous round")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initCompetition(2, { QStringLiteral("-60"), QStringLiteral("-66") });

    controller.SetCurrentRound(1);
    controller.SetCurrentContest(0);

    REQUIRE(controller.GetCurrentRound() == 1);
    REQUIRE(controller.GetCurrentContest() == 0);

    controller.PrevContest();
    CHECK(controller.GetCurrentRound() == 0);
    CHECK(controller.GetCurrentContest() == 1);

    controller.PrevContest();
    CHECK(controller.GetCurrentRound() == 0);
    CHECK(controller.GetCurrentContest() == 0);
}

TEST_CASE("[Controller] Save contest persists elapsed time and saved flag")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initCompetition(1, { QStringLiteral("-60") });

    controller.SetRoundTime(QTime(0, 0, 3));
    controller.DoAction(eAction_Hajime_Mate);
    controller.AdvanceTimerTicks(eTimer_Main, 2);

    controller.NextContest();

    const auto& contest = controller.GetContest(0, 0);
    CHECK(contest.is_saved);
    CHECK(contest.GetSecondsElapsed() == 2);
}

TEST_CASE("[Controller] Reset main timer restores clock only")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initCompetition(1, { QStringLiteral("-60") });

    controller.DoAction(eAction_Wazaari, ContestSide::SideA);
    controller.DoAction(eAction_Shido, ContestSide::SideB);
    controller.SetRoundTime(QTime(0, 1, 0));

    controller.DoAction(eAction_Hajime_Mate);
    controller.AdvanceTimerTicks(eTimer_Main, 5);
    controller.DoAction(eAction_Hajime_Mate);
    controller.DoAction(eAction_ResetMainTimer, ContestSide::None, true);

    const auto mainTime = controller.GetTimeText(eTimer_Main);
    const auto holdTime = controller.GetTimeText(eTimer_Hold);

    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Wazaari) == 1);
    CHECK(controller.GetScore(ContestSide::SideB, Score::Point::Shido) == 1);
    CHECK(mainTime == QStringLiteral("1:00"));
    CHECK(holdTime == QStringLiteral("00"));
}

TEST_CASE("[Controller] Reset contest clears scores and timers")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initCompetition(1, { QStringLiteral("-60") });

    controller.DoAction(eAction_Wazaari, ContestSide::SideA);
    controller.DoAction(eAction_Shido, ContestSide::SideB);
    controller.SetRoundTime(QTime(0, 1, 0));

    controller.DoAction(eAction_ResetAll);

    const auto resetMain = controller.GetTimeText(eTimer_Main);
    const auto resetHold = controller.GetTimeText(eTimer_Hold);

    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Wazaari) == 0);
    CHECK(controller.GetScore(ContestSide::SideB, Score::Point::Shido) == 0);
    CHECK(resetMain == QStringLiteral("0:30"));
    CHECK(resetHold == QStringLiteral("00"));
}

TEST_CASE("[Controller] Open ended golden score increments main timer")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initCompetition(1, { QStringLiteral("-60") });

    controller.SetGoldenScore(true);
    controller.DoAction(eAction_Hajime_Mate);

    controller.AdvanceTimerTicks(eTimer_Main, 3);

    CHECK(controller.GetTimeText(eTimer_Main) == QStringLiteral("0:03"));
    CHECK(controller.GetCurrentState() == eState_TimerRunning);
}

TEST_CASE("[Controller] Golden score hold scoring keeps fight active until decisive score")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    auto rules = std::make_shared<Ipponboard::ClassicRules>();
    controller.SetRules(rules);
    controller.SetAutoAdjustPoints(true);
    controller.SetGoldenScore(true);

    fixture.startFight();
    fixture.beginHold(ContestSide::SideA);

    const auto yukoAt = rules->GetOsaekomiValue(Score::Point::Yuko);
    const auto wazaariAt = rules->GetOsaekomiValue(Score::Point::Wazaari);

    fixture.advanceHoldTime(yukoAt);
    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Yuko) == 1);
    CHECK(controller.GetCurrentState() == eState_Holding);

    fixture.advanceHoldTime(wazaariAt - yukoAt);
    CHECK(controller.GetScore(ContestSide::SideA, Score::Point::Wazaari) == 1);
    CHECK(controller.GetCurrentState() == eState_Holding);
}

TEST_CASE("[Controller] Auto adjust points option toggles")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;

    CHECK(controller.IsAutoAdjustPoints());
    controller.SetAutoAdjustPoints(false);
    CHECK_FALSE(controller.IsAutoAdjustPoints());
    controller.SetAutoAdjustPoints(true);
    CHECK(controller.IsAutoAdjustPoints());
}
