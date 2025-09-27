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
