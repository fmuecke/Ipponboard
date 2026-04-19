#pragma once

#include "../core/CompetitionMode.h"
#include "../core/CompetitionModel.h"
#include "../core/Contest.h"
#include "../core/Controller.h"
#include "../core/Enums.h"
#include "../core/Rules.h"
#include "../core/Score.h"
#include "../core/StateMachine.h"
#include "TestQtApp.h"

#include <QTime>
#include <initializer_list>
#include <memory>
#include <set>
#include <vector>

namespace TestSupport
{

struct ControllerFixture
{
    ControllerFixture() : app(ensure_qt_app()), controller()
    {
        controller.SetRoundTime(QTime(0, 0, 30));
    }

    QCoreApplication& app;
    Ipponboard::Controller controller;

    void initCompetition(int rounds, const QStringList& weights)
    {
        Ipponboard::CompetitionMode mode;
        mode.nRounds = rounds;
        mode.weights = weights.join(';');
        mode.timeInSeconds = 30;
        controller.InitCompetition(mode);
    }

    void startFight() { controller.DoAction(Ipponboard::eAction_Hajime_Mate); }

    void advanceMainTime(int ticks)
    {
        controller.AdvanceTimerTicks(Ipponboard::eTimer_Main, ticks);
    }

    void beginHold(Ipponboard::ContestSide tori)
    {
        controller.DoAction(Ipponboard::eAction_OsaeKomi_Toketa, tori);
    }

    void advanceHoldTime(int ticks)
    {
        controller.AdvanceTimerTicks(Ipponboard::eTimer_Hold, ticks);
    }
};

struct RecordingControllerCore : public Ipponboard::IControllerCore
{
    enum class TimerEventType
    {
        Start,
        Stop,
        Reset
    };

    struct TimerEvent
    {
        TimerEventType type;
        Ipponboard::ETimer timer;
    };

    RecordingControllerCore()
        : time_values{ 0, 0 }, scores{}, rules(std::make_shared<Ipponboard::Rules2018>())
    {
        scores[0].Clear();
        scores[1].Clear();
    }

    void start_timer(Ipponboard::ETimer timer) override
    {
        timer_events.push_back({ TimerEventType::Start, timer });
    }

    void stop_timer(Ipponboard::ETimer timer) override
    {
        timer_events.push_back({ TimerEventType::Stop, timer });
    }

    void save_contest() override { contest_saved = true; }

    void reset_contest() override { contest_reset = true; }

    void reset_timer(Ipponboard::ETimer timer) override
    {
        timer_events.push_back({ TimerEventType::Reset, timer });
        time_values[static_cast<int>(timer)] = 0;
    }

    Ipponboard::Score& get_score(Ipponboard::ContestSide who) override
    {
        return scores[static_cast<int>(who)];
    }

    Ipponboard::Score const& get_score(Ipponboard::ContestSide who) const override
    {
        return scores[static_cast<int>(who)];
    }

    int get_time(Ipponboard::ETimer timer) const override
    {
        return time_values[static_cast<int>(timer)];
    }

    bool is_sonomama() const override { return sonomama; }

    bool is_golden_score() const override { return golden_score; }

    bool is_option(Ipponboard::EOption option) const override { return options.count(option) != 0; }

    bool is_auto_adjust() const override { return auto_adjust; }

    std::shared_ptr<Ipponboard::AbstractRules> GetRules() const override { return rules; }

    void set_time(Ipponboard::ETimer timer, int seconds)
    {
        time_values[static_cast<int>(timer)] = seconds;
    }

    void set_sonomama(bool value) { sonomama = value; }

    void set_golden_score(bool value) { golden_score = value; }

    void set_auto_adjust(bool value) { auto_adjust = value; }

    void set_rules(std::shared_ptr<Ipponboard::AbstractRules> new_rules)
    {
        rules = std::move(new_rules);
    }

    void set_options(std::initializer_list<Ipponboard::EOption> opts)
    {
        options = std::set<Ipponboard::EOption>(opts);
    }

    Ipponboard::Score& mutable_score(Ipponboard::ContestSide who)
    {
        return scores[static_cast<int>(who)];
    }

    void clear_timer_events() { timer_events.clear(); }

    void clear_observations()
    {
        clear_timer_events();
        contest_saved = false;
        contest_reset = false;
    }

    bool timer_event_occurred(TimerEventType type, Ipponboard::ETimer timer) const
    {
        for (const auto& evt : timer_events)
        {
            if (evt.type == type && evt.timer == timer)
            {
                return true;
            }
        }
        return false;
    }

    std::vector<TimerEvent> timer_events;
    bool contest_saved{ false };
    bool contest_reset{ false };

  private:
    int time_values[2];
    Ipponboard::Score scores[static_cast<int>(Ipponboard::ContestSide::Count)];
    bool sonomama{ false };
    bool golden_score{ false };
    bool auto_adjust{ true };
    std::set<Ipponboard::EOption> options;
    std::shared_ptr<Ipponboard::AbstractRules> rules;
};

struct StateMachineFixture
{
    StateMachineFixture() : machine(core) {}

    void toggleMainTimer() { machine.ToggleMainTimer(); }

    void beginHold(Ipponboard::ContestSide who) { machine.BeginHold(who); }

    void endHold() { machine.EndHold(); }

    void setHoldOwner(Ipponboard::ContestSide who) { machine.SetHoldOwner(who); }

    void clearHoldOwner() { machine.ClearHoldOwner(); }

    void awardPoint(Ipponboard::Score::Point point, Ipponboard::ContestSide who)
    {
        machine.AwardPoint(point, who);
    }

    void revokePoint(Ipponboard::Score::Point point, Ipponboard::ContestSide who)
    {
        machine.RevokePoint(point, who);
    }

    void awardShido(Ipponboard::ContestSide who) { machine.AwardShido(who); }

    void revokeShido(Ipponboard::ContestSide who) { machine.RevokeShido(who); }

    void revokeHansokumake(Ipponboard::ContestSide who) { machine.RevokeHansokumake(who); }

    void awardHansokumake(Ipponboard::ContestSide who) { machine.AwardHansokumake(who); }

    void resetContest() { machine.ResetContest(); }

    void finishContest() { machine.FinishContest(); }

    void onMainTimerElapsed() { machine.OnMainTimerElapsed(); }

    void onHoldTimerTick(int seconds) { machine.OnHoldTimerTick(seconds); }

    RecordingControllerCore core;
    Ipponboard::IpponboardSM machine;
};

} // namespace TestSupport
