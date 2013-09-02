#ifndef BASE__CONTROLLER_H_
#define BASE__CONTROLLER_H_

#include <QObject>
#include <QTime>
#include <utility>
#include <set>
#include <vector>
#include <bitset>

#include "score.h"
#include "tournament.h"
#include "icontroller.h"
#include "icontrollercore.h"
#include "tournamentmodel.h"
#include "statemachine.h"

// forwards
class QTimer;
class QSound;
//class TournamentModel;


namespace Ipponboard
{
// forwards
class IView;

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
	void RegisterView(IView* pView);
	int GetScore(Ipponboard::EFighter whos, Ipponboard::EPoint point) const;
	void DoAction(Ipponboard::EAction action, Ipponboard::EFighter who = Ipponboard::eFighter1, bool doRevoke = false);
	Ipponboard::EState GetCurrentState() const	{ return m_State; }
	Ipponboard::EFighter GetLead() const;
	Ipponboard::EFighter GetLastHolder() const;
	const QString GetTimeText(Ipponboard::ETimer timer) const;
	const QString GetFighterName(Ipponboard::EFighter who) const;
	const QString GetFighterLastName(Ipponboard::EFighter) const;
	const QString GetFighterFirstName(Ipponboard::EFighter) const;
	const QString GetFighterClub(Ipponboard::EFighter who) const;
	const QString& GetWeight() const;
	const QString GetMessage() const;
	int GetTeamScore(Ipponboard::EFighter who) const;
	void SetTimerValue(Ipponboard::ETimer timer, const QString& value);
	void SetRoundTime(const QString& value);
	QString GetRoundTime() const;
	int GetRoundTimeSecs() const;
	void SetRoundTime(const QTime& time);
	int GetRound() const;
	void SetWeightClass(QString const& c);
	QString const& GetCategoryName() const { return m_weight_class; } //TODO: weight class should be part of tournament!
	void SetGoldenScore(bool isGS);
	bool IsGoldenScore() const { return is_golden_score(); }
    void SetOption(Ipponboard::EOption option, bool isSet);
    bool GetOption(Ipponboard::EOption option) const;
    QString GetHomeLabel() const { return m_labelHome; }
    QString GetGuestLabel() const { return m_labelGuest; }
    void SetLabels(QString const& home, QString const& guest);

	void Gong() const;

	// --- IControllerCore ---
private:
	void start_timer(ETimer t);
	void stop_timer(ETimer t);
	void reset_fight();
	void reset_timer(ETimer);
	Score& get_score(Ipponboard::EFighter who);
	const Score& get_score(Ipponboard::EFighter who) const;
	int get_time(ETimer) const;
	bool is_sonomama() const;
	bool is_golden_score() const;
    bool is_option(Ipponboard::EOption option) const { return GetOption(option); } // TODO: use GetOption!

public:
	// --- other functions ---
	int GetFightCount() const
	{
		Q_ASSERT(m_TournamentModelsPtrs[0]);
		return m_TournamentModelsPtrs[0]->GetNumRows();
	}

	void SetCurrentFight(unsigned int index);

	int GetCurrentFightIndex() const
	{ return m_currentFight; }

	void SetCurrentTournament(unsigned int index)
	{ m_currentTournament = index; update_views(); }

	int GetCurrentTournamentIndex() const
	{ return m_currentTournament; }

	void ClearFights();
	void SetClub(Ipponboard::EFighter whos, const QString& clubName);
	void SetFight(unsigned int tournament_index, unsigned int fight_index,
				  const QString& weight, const QString& first_player_name,
				  const QString& first_player_club, const QString& second_player_name,
				  const QString& second_player_club, int yuko1 = -1,
				  int wazaari1 = -1, int ippon1 = -1, int shido1 = -1,
				  int hansokumake1 = -1, int yuko2 = -1, int wazaari2 = -1,
				  int ippon2 = -1, int shido2 = -1, int hansokumake2 = -1);
	const Ipponboard::Fight& GetFight(unsigned int tournament_index, unsigned int fight_index) const;
	void SetFighterName(Ipponboard::EFighter whos, const QString& name);

	void SetWeights(QStringList const& weights);
	void CopyAndSwitchGuestFighters();
	TournamentModel* GetTournamentScoreModel(int which = 0);

	void SetGongFile(const QString&);
	const QString& GetGongFile() const;


private slots:
	void update_main_time();
	void update_hold_time();

private:
//	void AddPoint_( Ipponboard::EFighter whos, Ipponboard::EPoint point );
//	void RemovePoint_( Ipponboard::EFighter whos, Ipponboard::EPoint point );
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
	{ return m_TournamentScores[m_currentTournament].at(m_currentFight); }

	Ipponboard::Tournament m_TournamentScores[2];
	TournamentModel* m_TournamentModelsPtrs[2];
	int m_currentFight;
	int m_currentTournament;

	Ipponboard::IpponboardSM* m_pSM;
	Ipponboard::EState m_State;
	QTimer* m_pTimerMain;
	QTimer* m_pTimerHold;
	QTime* m_pTimeMain;
	QTime* m_pTimeHold; // needed when side is not chosen yet
	Ipponboard::EFighter m_Tori;
	typedef std::set<IView*> ViewList;	// TODO: protect pointer
	ViewList m_Views;
	QString m_Message;
	bool setPointsInOsaekomi;
	QString m_gongFile;
	bool m_isSonoMama;
	bool m_isGoldenScore;
	QTime m_roundTime;
	QString m_weight_class;
    std::bitset<eOption_MAX> m_options;
    QString m_labelHome;
    QString m_labelGuest;
};

} // namespace Ipponboard

#endif  // BASE__CONTROLLER_H_
