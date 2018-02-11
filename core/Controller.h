﻿#ifndef BASE__CONTROLLER_H_
#define BASE__CONTROLLER_H_

#include "Score.h"
#include "Tournament.h"
#include "iController.h"
#include "iControllerCore.h"
#include "TournamentMode.h"
#include "TournamentModel.h"
#include "StateMachine.h"
#include "../util/helpers.hpp"

#include <QObject>
#include <QTime>
#include <utility>
#include <set>
#include <vector>
#include <bitset>
#include <memory>

// forwards
class QTimer;
class QSound;
//class TournamentModel;

namespace Ipponboard
{
// forwards
class IView;
class IGoldenScoreView;
class AbstractRules;

class Controller : public QObject, public IController, public IControllerCore
{
	Q_OBJECT

public:
	static const char* const msg_Ippon;
	static const char* const msg_WazariAwaseteIppon;
	static const char* const msg_Wazaari;
	static const char* const msg_Yuko;
	static const char* const msg_Shido;
	static const char* const msg_Hansokumake;
	static const char* const msg_Osaekomi;
	static const char* const msg_SonoMama;
	//static const char* const msg_Hantei;
	static const char* const msg_Hikiwaki;
	static const char* const msg_Winner;

	Controller();
	virtual ~Controller();

	// --- IController ---
	//FIXME: use override/final
	void InitTournament(TournamentMode const& mode);
	void RegisterView(IView* pView);
	void RegisterView(IGoldenScoreView* pView);
	int GetScoreValue(Ipponboard::FighterEnum whos, Ipponboard::Point point) const;
	void DoAction(Ipponboard::EAction action, Ipponboard::FighterEnum who = Ipponboard::FighterEnum::First, bool doRevoke = false);
	Ipponboard::EState GetCurrentState() const { return m_State; }
	Ipponboard::FighterEnum GetLead() const;
	Ipponboard::FighterEnum GetLastHolder() const;
	QString GetTimeText(Ipponboard::ETimer timer) const;
	QString GetFighterName(Ipponboard::FighterEnum who) const;
	QString GetFighterLastName(Ipponboard::FighterEnum) const;
	QString GetFighterFirstName(Ipponboard::FighterEnum) const;
	QString GetFighterClub(Ipponboard::FighterEnum who) const;
	QString const& GetWeight() const;
	QString GetMessage() const;
	int GetTeamScore(Ipponboard::FighterEnum who) const;
	void SetTimerValue(Ipponboard::ETimer timer, const QString& value);
	void SetRoundTime(const QString& value);
	QString GetFightTimeString() const;
	int GetFightDuration(QString const& weight) const;
	void SetRoundTime(const QTime& time);
	void OverrideRoundTimeOfFightMode(int fightTimeSecs);
	//FIXME: int GetRound() const;
	void SetWeightClass(QString const& c);
	QString const& GetCategoryName() const { return m_weight_class; } //TODO: weight class should be part of tournament!
	void SetGoldenScore(bool isGS);
	bool IsGoldenScore() const { return is_golden_score(); }
	void SetRules(RuleSet rules);
	Fight& CurrentMatch() override;
	Fight const& CurrentMatch() const;
	bool IsAutoAdjustPoints() const final;
	void SetAutoAdjustPoints(bool isActive) final;
	void SetOption(Ipponboard::EOption option, bool isSet);
	bool GetOption(Ipponboard::EOption option) const;
	QString GetHomeLabel() const { return m_labelHome; }
	QString GetGuestLabel() const { return m_labelGuest; }
	void SetLabels(QString const& home, QString const& guest);

	void Gong() const;

	IController const* GetIController() const { return this; }
	IController* GetIController() { return this; }

	// --- IControllerCore ---
private:
	void start_timer(ETimer t) final;
	void stop_timer(ETimer t) final;
	void save_fight() final;
	void reset_fight() final;
	void reset_timer(ETimer) final;
	int get_time(ETimer) const final;
	bool is_sonomama() const final;
	bool is_golden_score() const final;
	bool is_option(Ipponboard::EOption option) const final { return GetOption(option); } // TODO: use GetOption!
	bool is_auto_adjust() const final { return IsAutoAdjustPoints(); }

public:
	// --- other functions ---
	int GetFightCount() const
	{
		return static_cast<int>(m_Tournament.at(0)->size());
	}

	int GetRoundCount() const
	{
		return static_cast<int>(m_Tournament.size());
	}

	void NextFight();
	void PrevFight();
	void SetCurrentFight(unsigned int index);

	int GetCurrentFight() const
	{
		return m_currentFightNo;
	}

	void SetCurrentRound(unsigned int index);

	int GetCurrentRound() const
	{
		return m_currentRound;
	}

	void ClearFightsAndResetTimers();
	void SetClub(Ipponboard::FighterEnum whos, const QString& clubName);
	void SetFight(unsigned int tournament_index, unsigned int fight_index,
				  const QString& weight, const QString& first_player_name,
				  const QString& first_player_club, const QString& second_player_name,
				  const QString& second_player_club, int yuko1 = 0,
				  int wazaari1 = 0, int ippon1 = 0, int shido1 = 0,
				  int hansokumake1 = 0, int yuko2 = 0, int wazaari2 = 0,
				  int ippon2 = 0, int shido2 = 0, int hansokumake2 = 0);

	Ipponboard::Fight const& GetFight(unsigned int tournament_index, unsigned int fight_index) const;
	void SetFighterName(Ipponboard::FighterEnum whos, const QString& name);

	void SetWeights(QStringList const& weights);
	void CopyAndSwitchGuestFighters();
	PTournamentModel GetTournamentScoreModel(int which = 0);

	void SetGongFile(const QString&);
	QString const& GetGongFile() const;

private slots:
	void update_main_time();
	void update_hold_time();

private:
	void update_views() const;
	void reset();
	void reset_timer_value(Ipponboard::ETimer timer);

	inline Ipponboard::Fight& current_fight()
	{
		return m_Tournament.at(m_currentRound)->at(m_currentFightNo);
	}

	inline Ipponboard::Fight const& current_fight() const
	{
		return m_Tournament.at(m_currentRound)->at(m_currentFightNo);
	}

	Ipponboard::TournamentMode m_mode;
	Ipponboard::Tournament m_Tournament;
	std::vector<std::shared_ptr<TournamentModel> > m_TournamentModels;
	int m_currentRound;
	int m_currentFightNo;

	Ipponboard::IpponboardSM* m_pSM;
	Ipponboard::EState m_State;
	QTimer* m_pTimerMain;
	QTimer* m_pTimerHold;
	QTime* m_pTimeMain;
	QTime* m_pTimeHold; // needed when side is not chosen yet
	Ipponboard::FighterEnum m_Tori;
	std::set<IView*> m_views;
	std::set<IGoldenScoreView*> m_goldenScoreViews;
	QString m_Message;
	QString m_gongFile;
	bool m_isSonoMama;
	QTime m_roundTime;
	QString m_weight_class;
	std::bitset<eOption_MAX> m_options;
	bool m_isAutoAdjustPoints { true };
	bool m_isCountSubscores { false };
	QString m_labelHome;
	QString m_labelGuest;
	void reset_timers();
	RuleSet m_rules;
};
} // namespace Ipponboard

#endif  // BASE__CONTROLLER_H_
