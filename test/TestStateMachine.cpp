#include "../util/catch2/catch.hpp"

#include "ControllerTestHelpers.h"

using namespace Ipponboard;
using namespace TestSupport;

namespace
{
using TimerEventType = RecordingControllerCore::TimerEventType;
}

TEST_CASE("[StateMachine] Hajime starts main timer and resets hold timer")
{
    StateMachineFixture fixture;

    fixture.process(IpponboardSM_::Hajime_Mate{});

    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Reset, eTimer_Hold));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Start, eTimer_Main));
}

TEST_CASE("[StateMachine] Ippon stops both timers and ends the fight")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 90);

    fixture.process(IpponboardSM_::Hajime_Mate{});
    fixture.core.clear_timer_events();

    fixture.process(IpponboardSM_::Ippon(FighterEnum::First));

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Ippon());
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE(EState(fixture.machine.current_state()[0]) == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Hold time auto-adjust awards progressive scores")
{
    StateMachineFixture fixture;
    fixture.core.set_rules(std::make_shared<ClassicRules>());
    fixture.core.set_auto_adjust(true);

    fixture.process(IpponboardSM_::Hajime_Mate{});
    fixture.process(IpponboardSM_::Osaekomi_Toketa{});

    fixture.core.clear_timer_events();

    fixture.process(IpponboardSM_::HoldTimeEvent(15, FighterEnum::First));
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 1);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 0);

    fixture.process(IpponboardSM_::HoldTimeEvent(20, FighterEnum::First));
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 0);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 1);

    fixture.process(IpponboardSM_::HoldTimeEvent(25, FighterEnum::First));
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Ippon() == true);
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
}

