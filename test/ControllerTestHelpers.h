#pragma once

#include "../core/Controller.h"
#include "../core/StateMachine.h"
#include "../core/Rules.h"
#include "../core/TournamentMode.h"
#include "../core/TournamentModel.h"
#include "../core/Fight.h"
#include "../core/Score.h"
#include "../core/Enums.h"
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
	ControllerFixture() : app(ensure_qt_app()), controller() { controller.SetRoundTime(QTime(0, 0, 30)); }

	QCoreApplication& app;
	Ipponboard::Controller controller;

	void startFight() { controller.DoAction(Ipponboard::eAction_Hajime_Mate); }

	void advanceMainTime(int ticks) { controller.AdvanceTimerTicks(Ipponboard::eTimer_Main, ticks); }

	void beginHold(Ipponboard::FighterEnum tori) { controller.DoAction(Ipponboard::eAction_OsaeKomi_Toketa, tori); }

	void advanceHoldTime(int ticks) { controller.AdvanceTimerTicks(Ipponboard::eTimer_Hold, ticks); }
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

	RecordingControllerCore() : time_values{ 0, 0 }, scores{}, rules(std::make_shared<Ipponboard::Rules2018>())
	{
		scores[0].Clear();
		scores[1].Clear();
	}

	void start_timer(Ipponboard::ETimer timer) override { timer_events.push_back({ TimerEventType::Start, timer }); }

	void stop_timer(Ipponboard::ETimer timer) override { timer_events.push_back({ TimerEventType::Stop, timer }); }

	void save_fight() override { fight_saved = true; }

	void reset_fight() override { fight_reset = true; }

	void reset_timer(Ipponboard::ETimer timer) override
	{
		timer_events.push_back({ TimerEventType::Reset, timer });
		time_values[static_cast<int>(timer)] = 0;
	}

	Ipponboard::Score& get_score(Ipponboard::FighterEnum who) override { return scores[static_cast<int>(who)]; }

	Ipponboard::Score const& get_score(Ipponboard::FighterEnum who) const override
	{
		return scores[static_cast<int>(who)];
	}

	int get_time(Ipponboard::ETimer timer) const override { return time_values[static_cast<int>(timer)]; }

	bool is_sonomama() const override { return sonomama; }

	bool is_golden_score() const override { return golden_score; }

	bool is_option(Ipponboard::EOption option) const override { return options.count(option) != 0; }

	bool is_auto_adjust() const override { return auto_adjust; }

	std::shared_ptr<Ipponboard::AbstractRules> GetRules() const override { return rules; }

	void set_time(Ipponboard::ETimer timer, int seconds) { time_values[static_cast<int>(timer)] = seconds; }

	void set_sonomama(bool value) { sonomama = value; }

	void set_golden_score(bool value) { golden_score = value; }

	void set_auto_adjust(bool value) { auto_adjust = value; }

	void set_rules(std::shared_ptr<Ipponboard::AbstractRules> new_rules) { rules = std::move(new_rules); }

	void set_options(std::initializer_list<Ipponboard::EOption> opts) { options = std::set<Ipponboard::EOption>(opts); }

	Ipponboard::Score& mutable_score(Ipponboard::FighterEnum who) { return scores[static_cast<int>(who)]; }

	void clear_timer_events() { timer_events.clear(); }

	void clear_observations()
	{
		clear_timer_events();
		fight_saved = false;
		fight_reset = false;
	}

	bool timer_event_occurred(TimerEventType type, Ipponboard::ETimer timer) const
	{
		for (const auto& evt : timer_events)
		{
			if (evt.type == type && evt.timer == timer) { return true; }
		}
		return false;
	}

	std::vector<TimerEvent> timer_events;
	bool fight_saved{ false };
	bool fight_reset{ false };

  private:
	int time_values[2];
	Ipponboard::Score scores[static_cast<int>(Ipponboard::FighterEnum::_MAX)];
	bool sonomama{ false };
	bool golden_score{ false };
	bool auto_adjust{ true };
	std::set<Ipponboard::EOption> options;
	std::shared_ptr<Ipponboard::AbstractRules> rules;
};

struct StateMachineFixture
{
	StateMachineFixture()
	{
		machine.SetCore(&core);
		machine.start();
	}

	template <typename Event> void process(Event const& event) { machine.process_event(event); }

	RecordingControllerCore core;
	Ipponboard::IpponboardSM machine;
};

} // namespace TestSupport
