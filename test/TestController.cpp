#include "../core/Enums.h"
#include "../util/catch2/catch.hpp"
#include "ControllerTestHelpers.h"

#include <QStringList>
#include <QTime>

using namespace Ipponboard;
using namespace TestSupport;

TEST_CASE("[Controller] Main timer stops when fight time elapses")
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

    controller.DoAction(eAction_Wazaari, FighterEnum::First);
    REQUIRE(controller.GetScore(FighterEnum::First, Score::Point::Wazaari) == 1);
    REQUIRE(controller.GetCurrentState() == eState_TimerStopped);

    controller.DoAction(eAction_Wazaari, FighterEnum::First, true);
    REQUIRE(controller.GetScore(FighterEnum::First, Score::Point::Wazaari) == 0);
    REQUIRE(controller.GetCurrentState() == eState_TimerStopped);

    controller.DoAction(eAction_Hajime_Mate);
    REQUIRE(controller.GetCurrentState() == eState_TimerRunning);

    controller.AdvanceTimerTicks(eTimer_Main, 1);
    REQUIRE(controller.GetCurrentState() == eState_TimerRunning);
}

TEST_CASE("[Controller] Hold scoring awards points over time")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    controller.SetRoundTime(QTime(0, 3, 0));

    controller.DoAction(eAction_Hajime_Mate);
    REQUIRE(controller.GetCurrentState() == eState_TimerRunning);

    controller.DoAction(eAction_OsaeKomi_Toketa, FighterEnum::First);
    REQUIRE(controller.GetCurrentState() == eState_Holding);

    controller.AdvanceTimerTicks(eTimer_Hold, 5);
    REQUIRE(controller.GetScore(FighterEnum::First, Score::Point::Yuko) == 1);
    REQUIRE(controller.GetCurrentState() == eState_Holding);

    controller.AdvanceTimerTicks(eTimer_Hold, 5);
    REQUIRE(controller.GetScore(FighterEnum::First, Score::Point::Wazaari) == 1);
    REQUIRE(controller.GetCurrentState() == eState_Holding);

    controller.AdvanceTimerTicks(eTimer_Hold, 10);
    REQUIRE(controller.GetScore(FighterEnum::First, Score::Point::Ippon) == 1);
    REQUIRE(controller.GetCurrentState() == eState_TimerStopped);
}

TEST_CASE("[Controller] NextFight advances across rounds")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initTournament(2, { QStringLiteral("-60"), QStringLiteral("-66") });

    REQUIRE(controller.GetRoundCount() == 2);
    REQUIRE(controller.GetFightCount() == 2);
    REQUIRE(controller.GetCurrentRound() == 0);
    REQUIRE(controller.GetCurrentFight() == 0);

    controller.NextFight();
    CHECK(controller.GetCurrentRound() == 0);
    CHECK(controller.GetCurrentFight() == 1);

    controller.NextFight();
    CHECK(controller.GetCurrentRound() == 1);
    CHECK(controller.GetCurrentFight() == 0);
}

TEST_CASE("[Controller] PrevFight wraps to previous round")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initTournament(2, { QStringLiteral("-60"), QStringLiteral("-66") });

    controller.SetCurrentRound(1);
    controller.SetCurrentFight(0);

    REQUIRE(controller.GetCurrentRound() == 1);
    REQUIRE(controller.GetCurrentFight() == 0);

    controller.PrevFight();
    CHECK(controller.GetCurrentRound() == 0);
    CHECK(controller.GetCurrentFight() == 1);

    controller.PrevFight();
    CHECK(controller.GetCurrentRound() == 0);
    CHECK(controller.GetCurrentFight() == 0);
}

TEST_CASE("[Controller] Save fight persists elapsed time and saved flag")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initTournament(1, { QStringLiteral("-60") });

    controller.SetRoundTime(QTime(0, 0, 3));
    controller.DoAction(eAction_Hajime_Mate);
    controller.AdvanceTimerTicks(eTimer_Main, 2);

    controller.DoAction(eAction_ResetMainTimer);

    const auto& fight = controller.GetFight(0, 0);
    CHECK(fight.is_saved);
    CHECK(fight.GetSecondsElapsed() == 2);
}

TEST_CASE("[Controller] Reset fight clears scores and timers")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initTournament(1, { QStringLiteral("-60") });

    controller.DoAction(eAction_Wazaari, FighterEnum::First);
    controller.DoAction(eAction_Shido, FighterEnum::Second);
    controller.SetRoundTime(QTime(0, 1, 0));

    controller.DoAction(eAction_Hajime_Mate);
    controller.AdvanceTimerTicks(eTimer_Main, 5);
    controller.DoAction(eAction_ResetMainTimer);
    controller.DoAction(eAction_ResetMainTimer);

    CHECK(controller.GetScore(FighterEnum::First, Score::Point::Wazaari) == 0);
    CHECK(controller.GetScore(FighterEnum::Second, Score::Point::Shido) == 0);
    CHECK(controller.GetTimeText(eTimer_Main) == QStringLiteral("1:00"));
    CHECK(controller.GetTimeText(eTimer_Hold) == QStringLiteral("00"));
}

TEST_CASE("[Controller] Open ended golden score increments main timer")
{
    ControllerFixture fixture;
    auto& controller = fixture.controller;
    fixture.initTournament(1, { QStringLiteral("-60") });

    controller.SetGoldenScore(true);
    controller.DoAction(eAction_Hajime_Mate);

    controller.AdvanceTimerTicks(eTimer_Main, 3);

    CHECK(controller.GetTimeText(eTimer_Main) == QStringLiteral("0:03"));
    CHECK(controller.GetCurrentState() == eState_TimerRunning);
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
