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
	int GetScore(Ipponboard::FighterEnum whos, Ipponboard::Score::Point point) const;
	void DoAction(Ipponboard::EAction action, Ipponboard::FighterEnum who = Ipponboard::FighterEnum::First, bool doRevoke = false);
	Ipponboard::EState GetCurrentState() const	{ return m_State; }
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
	void SetFightTime(const QString& value);
	QString GetFightTimeString() const;
	int GetFightDuration(QString const& weight) const;
	void SetFightTime(const QTime& time);
	//FIXME: int GetRound() const;
	void SetWeightClass(QString const& c);
	QString const& GetCategoryName() const { return m_weight_class; } //TODO: weight class should be part of tournament!
	void SetGoldenScore(bool isGS);
	bool IsGoldenScore() const { return is_golden_score(); }
    void SetRules(std::shared_ptr<AbstractRules> rules);
    virtual std::shared_ptr<AbstractRules> GetRules() const override;
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
	void start_timer(ETimer t);
	void stop_timer(ETimer t);
	void save_fight();
	void reset_fight();
	void reset_timer(ETimer);
	Score& get_score(Ipponboard::FighterEnum who) final;
	Score const& get_score(Ipponboard::FighterEnum who) const final;
	int get_time(ETimer) const;
	bool is_sonomama() const;
	bool is_golden_score() const;
	bool is_option(Ipponboard::EOption option) const { return GetOption(option); } // TODO: use GetOption!

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
	{ return m_currentFight; }

	void SetCurrentRound(unsigned int index);

	int GetCurrentRound() const
	{ return m_currentRound; }

    void ClearFightsAndResetTimers();
	void SetClub(Ipponboard::FighterEnum whos, const QString& clubName);
	void SetFight(unsigned int tournament_index, unsigned int fight_index,
				  const QString& weight, const QString& first_player_name,
				  const QString& first_player_club, const QString& second_player_name,
				  const QString& second_player_club, int yuko1 = -1,
				  int wazaari1 = -1, int ippon1 = -1, int shido1 = -1,
				  int hansokumake1 = -1, int yuko2 = -1, int wazaari2 = -1,
				  int ippon2 = -1, int shido2 = -1, int hansokumake2 = -1);
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
//	void AddPoint_( Ipponboard::FighterEnum whos, Ipponboard::Score::Point point );
//	void RemovePoint_( Ipponboard::FighterEnum whos, Ipponboard::Score::Point point );
//	void StartStopTimer_( Ipponboard::ETimer timer );
//	void UpdatePointsFromHoldTimer_();
//	void CorrectState_();
	void update_views() const;
//	bool IsTimeLeft_() const
//	{
//		return  *m_pTimeMain > QTime(0,0,1) &&
//				*m_pTimeMain < QTime(23,0,0);
//	}
	void reset();
	void reset_timer_value(Ipponboard::ETimer timer);

	Ipponboard::Fight& current_fight()
	{ return m_Tournament.at(m_currentRound)->at(m_currentFight); }

	Ipponboard::TournamentMode m_mode;
	Ipponboard::Tournament m_Tournament;
	std::vector<std::shared_ptr<TournamentModel> > m_TournamentModels;
	int m_currentRound;
	int m_currentFight;

	Ipponboard::IpponboardSM* m_pSM;
	Ipponboard::EState m_State;
	QTimer* m_pTimerMain;
	QTimer* m_pTimerHold;
	QTime* m_pTimeMain;
	QTime* m_pTimeHold; // needed when side is not chosen yet
	Ipponboard::FighterEnum m_Tori;
	typedef std::set<IView*> ViewList;	// TODO: protect pointer
	ViewList m_Views;
	QString m_Message;
	bool setPointsInOsaekomi;
	QString m_gongFile;
	bool m_isSonoMama;
	bool m_isGoldenScore;
	QTime m_fightTime;
	QString m_weight_class;
	std::bitset<eOption_MAX> m_options;
	QString m_labelHome;
	QString m_labelGuest;
    void reset_timers();
    std::shared_ptr<AbstractRules> m_rules;
};

} // namespace Ipponboard

#endif  // BASE__CONTROLLER_H_
