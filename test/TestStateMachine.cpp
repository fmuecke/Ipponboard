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

    fixture.toggleMainTimer();

    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Reset, eTimer_Hold));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Start, eTimer_Main));
}

TEST_CASE("[StateMachine] Ippon stops both timers and ends the fight")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 90);

    fixture.toggleMainTimer();
    fixture.core.clear_timer_events();

    fixture.awardPoint(Score::Point::Ippon, FighterEnum::First);

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Ippon());
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Wazaari below match point keeps timers running")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 90);

    fixture.toggleMainTimer();
    fixture.core.clear_timer_events();

    fixture.awardPoint(Score::Point::Wazaari, FighterEnum::First);

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 1);
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerRunning);
}

TEST_CASE("[StateMachine] Wazaari match point stops the fight")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 90);

    fixture.toggleMainTimer();
    fixture.core.mutable_score(FighterEnum::First)
        .SetValue(Score::Point::Wazaari, fixture.core.GetRules()->GetMaxWazaariCount() - 1);
    fixture.core.clear_timer_events();

    fixture.awardPoint(Score::Point::Wazaari, FighterEnum::First);

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

    fixture.toggleMainTimer();
    fixture.core.mutable_score(FighterEnum::First).SetValue(Score::Point::Wazaari, 2);
    fixture.core.clear_timer_events();

    fixture.awardPoint(Score::Point::Wazaari, FighterEnum::First);

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 2);
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerRunning);
}

TEST_CASE("[StateMachine] Shido match point awards opponent and stops timers")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 120);

    fixture.toggleMainTimer();
    fixture.core.mutable_score(FighterEnum::Second)
        .SetValue(Score::Point::Shido, fixture.core.GetRules()->GetMaxShidoCount());
    fixture.core.clear_timer_events();

    fixture.awardShido(FighterEnum::Second);

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

    fixture.revokePoint(Score::Point::Wazaari, FighterEnum::First);

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 0);
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Revoke shido removes automatic opponent points")
{
    StateMachineFixture fixture;
    fixture.core.set_rules(std::make_shared<ClassicRules>());
    fixture.core.set_auto_adjust(true);

    fixture.awardShido(FighterEnum::Second);
    REQUIRE(fixture.core.mutable_score(FighterEnum::Second).Shido() == 1);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 0);

    fixture.awardShido(FighterEnum::Second);
    REQUIRE(fixture.core.mutable_score(FighterEnum::Second).Shido() == 2);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 1);

    fixture.revokeShido(FighterEnum::Second);

    REQUIRE(fixture.core.mutable_score(FighterEnum::Second).Shido() == 1);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 0);
}

TEST_CASE("[StateMachine] Golden score shido revocation keeps the main timer running")
{
    StateMachineFixture fixture;
    fixture.core.set_golden_score(true);
    fixture.core.set_time(eTimer_Main, 45);
    fixture.core.set_rules(std::make_shared<ClassicRules>());
    fixture.core.mutable_score(FighterEnum::First).SetValue(Score::Point::Wazaari, 1);
    fixture.core.mutable_score(FighterEnum::Second).SetValue(Score::Point::Shido, 2);

    fixture.toggleMainTimer();
    fixture.core.clear_observations();

    fixture.revokeShido(FighterEnum::Second);

    REQUIRE(fixture.core.mutable_score(FighterEnum::Second).Shido() == 1);
    REQUIRE(fixture.machine.CurrentState() == eState_TimerRunning);
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
}

TEST_CASE("[StateMachine] Finish while running stops timers and saves the fight")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 90);

    fixture.toggleMainTimer();
    fixture.core.clear_observations();

    fixture.finishFight();

    REQUIRE(fixture.core.fight_saved);
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Reset while running resets fight and returns to stopped")
{
    StateMachineFixture fixture;

    fixture.toggleMainTimer();
    fixture.core.clear_observations();

    fixture.resetFight();

    REQUIRE(fixture.core.fight_reset);
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Hold release resumes running when main time is left")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 15);

    fixture.toggleMainTimer();
    fixture.beginHold(FighterEnum::First);
    fixture.core.clear_observations();

    fixture.endHold();

    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerRunning);
}

TEST_CASE("[StateMachine] Hold release stops the fight when main time is up")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 0);

    fixture.toggleMainTimer();
    fixture.beginHold(FighterEnum::First);
    fixture.core.clear_observations();

    fixture.endHold();

    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
    REQUIRE_FALSE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Hold owner is tracked inside the engine")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 30);

    REQUIRE(fixture.machine.CurrentHolder() == FighterEnum::Nobody);

    fixture.toggleMainTimer();
    fixture.beginHold(FighterEnum::First);
    REQUIRE(fixture.machine.CurrentHolder() == FighterEnum::First);

    fixture.setHoldOwner(FighterEnum::Second);
    REQUIRE(fixture.machine.CurrentHolder() == FighterEnum::Second);

    fixture.clearHoldOwner();
    REQUIRE(fixture.machine.CurrentHolder() == FighterEnum::Nobody);
}

TEST_CASE("[StateMachine] Hold tick uses the tracked hold owner")
{
    StateMachineFixture fixture;
    fixture.core.set_rules(std::make_shared<Rules2025>());
    fixture.core.set_auto_adjust(true);

    fixture.toggleMainTimer();
    fixture.beginHold(FighterEnum::Second);

    fixture.onHoldTimerTick(5);

    REQUIRE(fixture.core.mutable_score(FighterEnum::Second).Yuko() == 1);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 0);
}

TEST_CASE("[StateMachine] Golden score decisive score stops the main timer")
{
    StateMachineFixture fixture;
    fixture.core.set_golden_score(true);
    fixture.core.set_time(eTimer_Main, 45);

    fixture.toggleMainTimer();
    fixture.core.clear_observations();

    fixture.awardPoint(Score::Point::Wazaari, FighterEnum::First);

    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 1);
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.machine.CurrentState() == eState_TimerStopped);
}

TEST_CASE("[StateMachine] Hansokumake awards opponent ippon and stops timers")
{
    StateMachineFixture fixture;
    fixture.core.set_time(eTimer_Main, 60);

    fixture.toggleMainTimer();
    fixture.core.clear_observations();

    fixture.awardHansokumake(FighterEnum::Second);

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

    fixture.toggleMainTimer();
    fixture.beginHold(FighterEnum::First);

    fixture.core.clear_timer_events();

    fixture.onHoldTimerTick(15);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 1);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 0);

    fixture.onHoldTimerTick(20);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Yuko() == 0);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Wazaari() == 1);

    fixture.onHoldTimerTick(25);
    REQUIRE(fixture.core.mutable_score(FighterEnum::First).Ippon() == true);
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Main));
    REQUIRE(fixture.core.timer_event_occurred(TimerEventType::Stop, eTimer_Hold));
}
