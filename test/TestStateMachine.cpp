#include "ControllerTestHelpers.h"

#include <catch2/catch_test_macros.hpp>

using namespace Ipponboard;
using namespace TestSupport;

namespace
{
using TimerEventType = RecordingControllerCore::TimerEventType;

// Minimal ruleset to disable awasete-ippon so can_add_wazaari guard flips false.
class NoAwaseteRules final : public AbstractRules
{
  public:
    const char* Name() const override { return "NoAwasete"; }
    bool IsOption_AwaseteIppon() const override { return false; }
    bool IsOption_ShidoScoreCounts() const override { return false; }
    int GetMaxWazaariCount() const override { return 2; }

    int GetOsaekomiValue(Score::Point p) const override
    {
        switch (p)
        {
        case Score::Point::Ippon:
            return 20;
        case Score::Point::Wazaari:
            return 10;
        case Score::Point::Yuko:
            return 5;
        default:
            return -1;
        }
    }
};
} // namespace

TEST_CASE("[StateMachine] Hajime starts main timer and resets hold timer")
{
    StateMachineFixture fixture;

    fixture.action(eAction_Hajime_Mate);

    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Reset, eTimer_Hold));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Start, eTimer_Main));
}

TEST_CASE("[StateMachine] Ippon stops both timers and ends the fight")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 90);

    fixture.action(eAction_Hajime_Mate);
    fixture.core.clear_timer_events();

    fixture.action(eAction_Ippon, FighterEnum::First);

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Ippon());
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Wazaari below match point keeps timers running")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 90);

    fixture.action(eAction_Hajime_Mate);
    fixture.core.clear_timer_events();

    fixture.action(eAction_Wazaari, FighterEnum::First);

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 1);
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerRunning);
}

TEST_CASE("[StateMachine] Wazaari match point stops the fight")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 90);

    fixture.action(eAction_Hajime_Mate);
    fixture.core.mutable_score(FighterEnum::First)
        .SetValue(Score::Point::Wazaari, fixture.core.GetRules()->GetMaxWazaariCount() - 1);
    fixture.core.clear_timer_events();

    fixture.action(eAction_Wazaari, FighterEnum::First);

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() ==
            fixture.core.GetRules()->GetMaxWazaariCount());
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Wazaari blocked when awasete is disabled and max reached")
{
    StateMachineFixture fixture;
    fixture.core.set_rules(std::make_shared<NoAwaseteRules>());
    fixture.core.set_auto_adjust(false);

    fixture.action(eAction_Hajime_Mate);
    fixture.core.mutable_score(FighterEnum::First).SetValue(Score::Point::Wazaari, 2);
    fixture.core.clear_timer_events();

    fixture.action(eAction_Wazaari, FighterEnum::First);

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 2);
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerRunning);
}

TEST_CASE("[StateMachine] Shido match point awards opponent and stops timers")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 120);

    fixture.action(eAction_Hajime_Mate);
    fixture.core.mutable_score(FighterEnum::Second)
        .SetValue(Score::Point::Shido, fixture.core.GetRules()->GetMaxShidoCount());
    fixture.core.clear_timer_events();

    fixture.action(eAction_Shido, FighterEnum::Second);

    REQUIRE(fixture.core.mutable_score(FighterEnum::Second).Shido() ==
            fixture.core.GetRules()->GetMaxShidoCount() + 1);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Ippon());
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Revoke wazaari restores score without side effects")
{
    StateMachineFixture fixture;

    fixture.core.mutable_score(FighterEnum::First).SetValue(Score::Point::Wazaari, 1);
    fixture.core.clear_timer_events();

    fixture.action(eAction_Wazaari, FighterEnum::First, true);

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 0);
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Revoke shido removes automatic opponent points")
{
    StateMachineFixture fixture;
    fixture.core.set_rules(std::make_shared<ClassicRules>());
    fixture.core.set_auto_adjust(true);

    fixture.action(eAction_Shido, FighterEnum::Second);
    REQUIRE(fixture.core.mutable_score(FighterEnum::Second).Shido() == 1);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 0);

    fixture.action(eAction_Shido, FighterEnum::Second);
    REQUIRE(fixture.core.mutable_score(FighterEnum::Second).Shido() == 2);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 1);

    fixture.action(eAction_Shido, FighterEnum::Second, true);

    REQUIRE(fixture.core.mutable_score(FighterEnum::Second).Shido() == 1);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 0);
}

TEST_CASE("[StateMachine] Finish while running stops timers and saves the fight")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 90);

    fixture.action(eAction_Hajime_Mate);
    fixture.core.clear_observations();

    fixture.finish();

    REQUIRE(fixture.core.fight_saved);
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Reset while running resets fight and returns to stopped")
{
    StateMachineFixture fixture;

    fixture.action(eAction_Hajime_Mate);
    fixture.core.clear_observations();

    fixture.action(eAction_ResetAll);

    REQUIRE(fixture.core.fight_reset);
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Hold release resumes running when main time is left")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 15);

    fixture.action(eAction_Hajime_Mate);
    fixture.action(eAction_OsaeKomi_Toketa);
    fixture.core.clear_observations();

    fixture.action(eAction_OsaeKomi_Toketa);

    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerRunning);
}

TEST_CASE("[StateMachine] Hold release stops the fight when main time is up")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 0);

    fixture.action(eAction_Hajime_Mate);
    fixture.action(eAction_OsaeKomi_Toketa);
    fixture.core.clear_observations();

    fixture.action(eAction_OsaeKomi_Toketa);

    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Golden score decisive score stops the main timer")
{
    StateMachineFixture fixture;
    fixture.core.set_golden_score(true);
    fixture.core.set_time(eTimer_Main, 45);

    fixture.action(eAction_Hajime_Mate);
    fixture.core.clear_observations();

    fixture.action(eAction_Wazaari, FighterEnum::First);

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 1);
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Hansokumake awards opponent ippon and stops timers")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 60);

    fixture.action(eAction_Hajime_Mate);
    fixture.core.clear_observations();

    fixture.action(eAction_Hansokumake, FighterEnum::Second);

    REQUIRE(fixture.core.mutable_score(FighterEnum::Second).Hansokumake());
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Ippon());
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Hold time auto-adjust awards progressive scores")
{
    StateMachineFixture fixture;
    fixture.core.set_rules(std::make_shared<ClassicRules>());
    fixture.core.set_auto_adjust(true);

    fixture.action(eAction_Hajime_Mate);
    fixture.action(eAction_OsaeKomi_Toketa);

    fixture.core.clear_timer_events();

    fixture.onHoldTimerTick(15, FighterEnum::First);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 1);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 0);

    fixture.onHoldTimerTick(20, FighterEnum::First);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 0);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 1);

    fixture.onHoldTimerTick(25, FighterEnum::First);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Ippon() == true);
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
}
