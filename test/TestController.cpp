#include "../util/catch2/catch.hpp"
#include "../core/Controller.h"
#include "../core/Enums.h"

#include <QTime>
#include "TestQtApp.h"

using namespace Ipponboard;

TEST_CASE("[Controller] Main timer stops when fight time elapses")
{
    ensure_qt_app();

    Controller controller;
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

TEST_CASE("[Controller] Hold scoring awards points over time")
{
    ensure_qt_app();

    Controller controller;
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



