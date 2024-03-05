// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "Controller.h"
#include "iView.h"
#include "iGoldenScoreView.h"
#include "Score.h"
#include "Enums.h"
#include "TournamentMode.h"
#include "TournamentModel.h"
#include "StateMachine.h"
#include "Rules.h"

#include <QTimer>
#include <QFileInfo>
#include <QMessageBox>
#include <algorithm>
#include <functional>

#ifdef _WIN32
#include <QSound>
#endif
#ifdef __linux__
#ifdef __QT4__
#include <QProcess>
#else
#include <QSound>
#endif
#endif

using namespace Ipponboard;
using Point = Score::Point;

const char* const Controller::msg_Ippon = "Ippon";
const char* const Controller::msg_WazariAwaseteIppon = "Wazaari awasete ippon";
const char* const Controller::msg_Wazaari = "Wazaari";
const char* const Controller::msg_Yuko = "Yuko";
const char* const Controller::msg_Shido = "Shido";
const char* const Controller::msg_Hansokumake = "Hansokumake";
const char* const Controller::msg_Osaekomi = "Osae Komi";
const char* const Controller::msg_SonoMama = "Sono Mama";
//const char* const Controller::msg_Hantei = "Hantei";
const char* const Controller::msg_Hikiwaki = "Hikiwaki";
const char* const Controller::msg_Winner = "Winner";
const char* const emptyFighterName = "--";

//=========================================================
Controller::Controller()
	: m_mode()
	, m_Tournament()
	, m_TournamentModels()
	, m_currentRound(0)
	, m_currentFight(0)
	, m_pSM(0)
	, m_State(eState_TimerStopped)
	, m_pTimerMain(0)
	, m_pTimerHold(0)
	, m_pTimeMain(0)
	, m_pTimeHold(0)
    , m_Tori(FighterEnum::Nobody)
	, m_isSonoMama(false)
	, m_roundTime(0, 0, 0, 0)
	, m_options(0)
	, m_labelHome("HOME")
	, m_labelGuest("GUEST")
//=========================================================
{
	m_pSM = new IpponboardSM();
	m_pSM->SetCore(this);
	m_pTimerMain = new QTimer(this);
	m_pTimerHold = new QTimer(this);
	m_pTimeMain = new QTime();
	m_pTimeHold = new QTime();

	InitTournament(m_mode);

	reset();
	m_pSM->start();

	connect(m_pTimerMain, SIGNAL(timeout()), this, SLOT(update_main_time()));
	connect(m_pTimerHold, SIGNAL(timeout()), this, SLOT(update_hold_time()));
}

//=========================================================
Controller::~Controller()
//=========================================================
{
	m_views.clear();
	m_goldenScoreViews.clear();

    if (m_pTimeHold)
        delete m_pTimeHold;

    if (m_pTimeMain)
        delete m_pTimeMain;

    if (m_pSM)
        delete m_pSM;
}

//=========================================================
void Controller::InitTournament(TournamentMode const& mode)
//=========================================================
{
	m_TournamentModels.clear();
	m_Tournament.clear();

	m_mode = mode;
	m_rules = RulesFactory::Create(m_mode.rules);
	m_rules->SetCountSubscores(m_mode.IsOptionSet(TournamentMode::str_Option_AllSubscoresCount));

	QStringList actualWeights = m_mode.weights.split(';');

	for (int round = 0; round < m_mode.nRounds; ++round)
	{
		PTournamentRound pRound(new TournamentRound());

		for (int fightNo = 0; fightNo < m_mode.FightsPerRound(); ++fightNo)
		{
			QString weight = m_mode.weightsAreDoubled ?
							 actualWeights[fightNo / 2] :
							 actualWeights[fightNo];

			Fight fight;
			fight.weight = weight;
			fight.SetRoundTime(m_mode.GetFightDuration(weight));
			fight.rules = m_rules;
			fight.rules->SetCountSubscores(m_mode.IsOptionSet(TournamentMode::str_Option_AllSubscoresCount));

			SimpleFighter emptyFighter;
			emptyFighter.name = emptyFighterName;
			fight.fighters[0] = emptyFighter;
			fight.fighters[1] = emptyFighter;

			pRound->emplace_back(fight);
		}

		m_Tournament.push_back(pRound);

		PTournamentModel pModel(new TournamentModel(pRound));
		pModel->SetNumRows(m_mode.FightsPerRound());

		m_TournamentModels.push_back(pModel);
	}

	// set options AFTER configuring fights

	m_currentRound = 0;
	m_currentFight = 0;

	// set time and update views
    SetRoundTime(QTime(0,0,0,0).addSecs(m_mode.GetFightDuration(current_fight().weight)));
}

//=========================================================
int Controller::GetScore(FighterEnum whos, Score::Point point) const
//=========================================================
{
	int value(0);
	const Score& score = get_score(whos);

	switch (point)
	{
	case Point::Yuko:
		value = score.Yuko();
		break;

	case Point::Wazaari:
		value = /*score.IsAwaseteIppon() ? 0 : */score.Wazaari();
		break;

	case Point::Ippon:
		if (score.Ippon() || m_rules->IsAwaseteIppon(score))
			value = 1;
		else
			value = 0;

		break;

	case Point::Shido:
		value = score.Shido();
		break;

	case Point::Hansokumake:
		value = score.Hansokumake() ? 1 : 0;
		break;

	default:
		//Q_ASSERT(!"Unknown case in switch!");
		break;
	}

	return value;
}

//=========================================================
void Controller::DoAction(EAction action, FighterEnum whos, bool doRevoke)
//=========================================================
{
	if (doRevoke)
	{
		switch (action)
		{
		case eAction_Yuko:
			m_pSM->process_event(IpponboardSM_::RevokeYuko(whos));
			break;

		case eAction_Wazaari:
			m_pSM->process_event(IpponboardSM_::RevokeWazaari(whos));
			break;

		case eAction_Ippon:
			m_pSM->process_event(IpponboardSM_::RevokeIppon(whos));
			break;

		case eAction_Shido:
		case eAction_Hansokumake:
			m_pSM->process_event(IpponboardSM_::RevokeShidoHM(whos));
			break;

		case eAction_ResetOsaeKomi:
			reset_timer_value(eTimer_Hold);
			break;

		case eAction_ResetMainTimer:
			reset_timer_value(eTimer_Main);
			break;

		default:
			return;
		}
	}
	else
	{
		switch (action)
		{
		case eAction_Hajime_Mate:
			if (eState_Holding == m_State)
				m_isSonoMama = true;
			else
				m_isSonoMama = false;

			m_pSM->process_event(IpponboardSM_::Hajime_Mate());
			break;

		case eAction_OsaeKomi_Toketa:
			m_pSM->process_event(IpponboardSM_::Osaekomi_Toketa());
			m_Tori = whos;
			m_isSonoMama = false;
			break;

		case eAction_Yuko:
			m_pSM->process_event(IpponboardSM_::Yuko(whos));
			break;

		case eAction_Wazaari:
			m_pSM->process_event(IpponboardSM_::Wazaari(whos));
			break;

		case eAction_Ippon:
			m_pSM->process_event(IpponboardSM_::Ippon(whos));
			break;

		case eAction_Shido:
			m_pSM->process_event(IpponboardSM_::Shido(whos));
			break;

		case eAction_Hansokumake:
			m_pSM->process_event(IpponboardSM_::Hansokumake(whos));
			break;

		case eAction_SetOsaekomi:
			m_Tori = whos;
			break;

		case eAction_ResetAll:
			m_pSM->process_event(IpponboardSM_::Reset());
			break;

		default:
			//Q_ASSERT(!"wrong action/action not handled!");
			break;
		}
	}

	//	if( eState_SonoMama == m_State && action != eAction_SonoMama_Yoshi )
	//		return;
	//

	//
	// handle golden score
	//
	// TODO: use state machine
	//
	// test cases:
	// (1) when golden score is active, wazaari or any other (except shido) will stop main timer
	// (2) main timer can be started, even if points are unequal (e.g. 1 shido) - no matter if GS or not
	// (3) main timer will not be stopped after the first achieved point in osaekomi in GS
	//
	// Do stop main timer if
	// - golden score is active
	// - we were not trying to start the timer itself
	// - if point <> shidos are changeed
	// - and the timer is not stopped
	//
	if (current_fight().IsGoldenScore()
			&& action != eAction_Hajime_Mate
			&& action != eAction_Shido
			&& eState_TimerStopped != EState(m_pSM->current_state()[0]))
	{
		// Note: In golden score the hold should not end after the first scored point!
		if (eState_Holding != EState(m_pSM->current_state()[0]))
		{
			auto ruleSet = GetRules();

			if (ruleSet->CompareScore(current_fight()) != 0)
			{
				m_pSM->process_event(IpponboardSM_::Hajime_Mate());
			}
		}
	}

	// set current state
	m_State = EState(m_pSM->current_state()[0]);

	update_views();
}

//=========================================================
FighterEnum Controller::GetLead() const
//=========================================================
{
    FighterEnum winner(FighterEnum::Nobody);

	switch (m_State)
	{
	case eState_TimerRunning:
	case eState_TimerStopped:
		{
			// determine who has the lead
			if (get_score(FighterEnum::First).Wazaari() > get_score(FighterEnum::Second).Wazaari())
			{
				winner = FighterEnum::First;
			}
			else if (get_score(FighterEnum::First).Wazaari() < get_score(FighterEnum::Second).Wazaari())
			{
				winner = FighterEnum::Second;
			}
			else  // GetScore_(eFirst).Wazaari() == GetScore_(eSecond).Wazaari()
			{
				if (get_score(FighterEnum::First).Yuko() >
						get_score(FighterEnum::Second).Yuko())
				{
					winner = FighterEnum::First;
				}
				else if (get_score(FighterEnum::First).Yuko() < get_score(FighterEnum::Second).Yuko())
				{
					winner = FighterEnum::Second;
				}
				else  // GetScore_(eFirst).Yuko() == GetScore_(eSecond).Yuko()
				{
					if (get_score(FighterEnum::First).Shido() < get_score(FighterEnum::Second).Shido() &&
							get_score(FighterEnum::Second).Shido() > 1)  // no "koka"!
					{
						winner = FighterEnum::First;
					}
					else if (get_score(FighterEnum::First).Shido() > get_score(FighterEnum::Second).Shido() &&
							 get_score(FighterEnum::First).Shido() > 1)
					{
						winner = FighterEnum::Second;
					}
					else
					{
						// equal ==> golden score in single tournament (Hantai is no more)
					}
				}
			}

			break;
		}

	case eState_SonoMama:
	case eState_Holding:
		winner = m_Tori;
		break;

	default:
		break;
	}

	return winner;
}

//=========================================================
Ipponboard::FighterEnum Controller::GetLastHolder() const
//=========================================================
{
	return m_Tori;
}

//=========================================================
void Controller::reset_timers()
{
	m_State = eState_TimerStopped;

	Q_ASSERT(m_pTimeMain);
	Q_ASSERT(m_pTimeHold);

	m_pTimerMain->stop();
	reset_timer_value(eTimer_Main);

	m_pTimerHold->stop();
	reset_timer_value(eTimer_Hold);
    m_Tori = FighterEnum::Nobody;

	m_isSonoMama = false;
}

void Controller::reset()
//=========================================================
{
	ClearFightsAndResetTimers();
	SetFight(0, 0, "-XX", tr("First"), "", tr("Second"), "");

	update_views();
}

//=========================================================
void Controller::reset_timer_value(Ipponboard::ETimer timer)
//=========================================================
{
	// Note:
	//  just reset values - nothing more, nothing less

	if (eTimer_Main == timer)
	{
		*m_pTimeMain = m_roundTime;  // FIXME: set to 0 for golden score?
	}
	else if (eTimer_Hold == timer)
	{
		m_pTimeHold->setHMS(0, 0, 0, 0);

		if (!m_pTimerHold->isActive())
            m_Tori = FighterEnum::Nobody;
	}
}

//=========================================================
QString Controller::GetTimeText(ETimer timer) const
//=========================================================
{
	QString ret;

	switch (timer)
	{
	case eTimer_Main:
		ret = m_pTimeMain->toString("m:ss");
		ret = ret.isEmpty() ? "0:00" : ret;
		break;

	case eTimer_Hold:
		ret = m_pTimeHold->toString("ss");
		ret = ret.isEmpty() ? "00" : ret;
		break;

	default:
		break;
	}

	return ret;
}

//=========================================================
QString Controller::GetFighterName(FighterEnum who) const
//=========================================================
{
	Q_ASSERT(who == FighterEnum::First || who == FighterEnum::Second);

	QString name = m_Tournament[m_currentRound]
				   ->at(m_currentFight).GetFighter(who).name;

	// shorten name
	const int pos = name.indexOf(' ');

	if (pos != -1)
	{
		name = name.left(1) + ". " + name.mid(pos, name.length() - pos);
	}

	return name;
}

//=========================================================
QString Controller::GetFighterLastName(Ipponboard::FighterEnum who) const
//=========================================================
{
	Q_ASSERT(who == FighterEnum::First || who == FighterEnum::Second);

	QString name = m_Tournament[m_currentRound]
				   ->at(m_currentFight).GetFighter(who).name;

	// get last name
	const int pos = name.indexOf(' ');

	if (pos != -1)
	{
		name = name.mid(pos + 1, name.length() - pos);
	}

	return name;
}

//=========================================================
QString Controller::GetFighterFirstName(Ipponboard::FighterEnum who) const
//=========================================================
{
	Q_ASSERT(who == FighterEnum::First || who == FighterEnum::Second);

	QString name = m_Tournament[m_currentRound]
				   ->at(m_currentFight).GetFighter(who).name;

	// get first name
	const int pos = name.indexOf(' ');

	if (pos != -1)
	{
		name = name.left(pos);
	}
	else
	{
		name = QString();
	}

	return name;
}

//=========================================================
QString Controller::GetFighterClub(FighterEnum who) const
//=========================================================
{
	Q_ASSERT(who == FighterEnum::First || who == FighterEnum::Second);

	return m_Tournament[m_currentRound]
		   ->at(m_currentFight).GetFighter(who).club;
}

//=========================================================
QString const& Controller::GetWeight() const
//=========================================================
{
	return m_Tournament[m_currentRound]
		   ->at(m_currentFight).weight;
}

//=========================================================
QString Controller::GetMessage() const
//=========================================================
{
	return m_Message;
}

//=========================================================
int Controller::GetTeamScore(Ipponboard::FighterEnum who) const
//=========================================================
{
	int score(0);

	for (size_t round(0); round < m_Tournament.size(); ++round)
	{
		for (size_t fight(0); fight < m_Tournament[0]->size(); ++fight)
		{
			if (m_Tournament[round]->at(fight).is_saved)
			{
				score += m_Tournament[round]->at(fight).HasWon(who);
			}
		}
	}

	return score;
}

//=========================================================
void Controller::SetTimerValue(Ipponboard::ETimer timer, const QString& value)
//=========================================================
{
	if (eState_TimerStopped == m_State ||
			eState_SonoMama == m_State)
	{
		if (eTimer_Main == timer)
		{
			QTime newTime = QTime::fromString(value, "m:ss");

			// don't allow invalid times like "-1:22"
			if (newTime.isValid())
			{
				*m_pTimeMain = newTime;
			}
		}
		else if (eTimer_Hold == timer)
		{
			m_pTimeHold->setHMS(0, 0, value.toInt());
		}

		update_views();
	}
}

//=========================================================
void Controller::SetRoundTime(const QString& value)
//=========================================================
{
	SetRoundTime(QTime::fromString(value, "m:ss"));
}

//=========================================================
void Controller::SetRoundTime(QTime const& time)
//=========================================================
{
	m_roundTime = time;
	*m_pTimeMain = time;

	update_views();
}

void Controller::OverrideRoundTimeOfFightMode(int fightTimeSecs)
{
	m_mode.fightTimeInSeconds = fightTimeSecs;
}

//=========================================================
QString Controller::GetFightTimeString() const
//=========================================================
{
	return m_roundTime.toString("m:ss");  //FIXME: use main time value instead
}

//=========================================================
//int Controller::GetRound() const
//=========================================================
//{
//    return m_currentRound * 10 + m_currentFight + 1;
//}

//=========================================================
int Controller::GetFightDuration(QString const& weight) const
//=========================================================
{
	return m_mode.GetFightDuration(weight);
}

//=========================================================
void Controller::SetWeightClass(QString const& c)
//=========================================================
{
	m_weight_class = c;
}

//=========================================================
void Controller::SetGoldenScore(bool isGS)
//=========================================================
{
	current_fight().SetGoldenScore(isGS);

	if (isGS && GetRules()->IsOption_OpenEndGoldenScore())
	{
        *m_pTimeMain = QTime(0,0,0,0).addSecs(current_fight().GetGoldenScoreTime());
	}
	else
	{
        *m_pTimeMain = QTime(0,0,0,0).addSecs(current_fight().GetRemainingTime());
	}

	update_views();
}

std::shared_ptr<AbstractRules> Controller::GetRules() const
{
	return m_rules;
}

void Controller::SetRules(std::shared_ptr<AbstractRules> rules)
{
	m_rules = rules;

	for (auto const & pRound : m_Tournament)
	{
		for (auto & fight : *pRound)
		{
			fight.rules = rules;
		}
	}
}

bool Controller::IsAutoAdjustPoints() const
{
	return m_isAutoAdjustPoints;
}

void Controller::SetAutoAdjustPoints(bool isActive)
{
	m_isAutoAdjustPoints = isActive;
}

//=========================================================
void Controller::SetOption(EOption option, bool isSet)
//=========================================================
{
	m_options.set(option, isSet);

	// TODO: remove maybe
	if (eOption_AllSubscoresCount == option)
	{
		m_rules->SetCountSubscores(isSet);
	}
}

//=========================================================
bool Controller::GetOption(EOption option) const
//=========================================================
{
	return m_options.test(option);
}

//=========================================================
void Controller::SetLabels(const QString& home, const QString& guest)
//=========================================================
{
	m_labelHome = home;
	m_labelGuest = guest;
}

//=========================================================
void Controller::Gong() const
//=========================================================
{
#ifdef _WIN32
    if(QSound::isAvailable())
        QSound::play(m_gongFile);
    else
        QMessageBox::information(0, tr("Error"), tr("No sound device found"));
#endif
#ifdef __linux__
#ifdef __QT4__
    QProcess::startDetached("/usr/bin/aplay", QStringList() << m_gongFile);
#else
    QSound::play(m_gongFile);
#endif
#endif
}

//=========================================================
void Controller::RegisterView(IView* pView)
//=========================================================
{
	m_views.insert(pView);

	// do not call UpdateViews here as views may not have been fully created
}

void Controller::RegisterView(IGoldenScoreView* pView)
{
	m_goldenScoreViews.insert(pView);
}

//=========================================================
void Controller::start_timer(ETimer t)
//=========================================================
{
	if (eTimer_Main == t)
	{
		m_pTimerMain->start(1000);
	}
	else
	{
		m_pTimerHold->start(1000);
	}
}

//=========================================================
void Controller::stop_timer(ETimer t)
//=========================================================
{
	m_pTimerHold->stop();

	if (eTimer_Main == t)
	{
		m_pTimerMain->stop();
	}
}

//=========================================================
void Controller::save_fight()
//=========================================================
{
	auto elapsed = is_golden_score() ? m_pTimeMain->secsTo(QTime()) : m_pTimeMain->secsTo(m_roundTime);
	current_fight().SetSecondsElapsed(elapsed);
	current_fight().is_saved = true;
}

//=========================================================
void Controller::reset_fight()
//=========================================================
{
	m_pTimerHold->stop();
	m_pTimerMain->stop();
	m_pTimeHold->setHMS(0, 0, 0, 0);
    m_Tori = FighterEnum::Nobody;

	// just clear the score, not the names
	get_score(FighterEnum::First) = Score();
	get_score(FighterEnum::Second) = Score();

	//FIXME: make fight resetting more robust
	Fight& fight = m_Tournament[m_currentRound]->at(m_currentFight);
	fight.SetSecondsElapsed(0);
	fight.SetGoldenScore(false);
	fight.is_saved = false;
	fight.rules = m_rules;
    m_roundTime = QTime(0,0,0,0).addSecs(m_mode.GetFightDuration(current_fight().weight));
	*m_pTimeMain = m_roundTime;

	update_views();
}

//=========================================================
void Controller::reset_timer(ETimer t)
//=========================================================
{
	// called by statemachine

	reset_timer_value(t);
	update_views();
}

//=========================================================
Score& Controller::get_score(FighterEnum who)
//=========================================================
{
	Q_ASSERT(who == FighterEnum::First || who == FighterEnum::Second);

	return m_Tournament[m_currentRound]
		   ->at(m_currentFight).GetScore(who);
}

//=========================================================
Score const& Controller::get_score(FighterEnum who) const
//=========================================================
{
	Q_ASSERT(who == FighterEnum::First || who == FighterEnum::Second);

	return m_Tournament[m_currentRound]
		   ->at(m_currentFight).GetScore(who);
}

//=========================================================
int Controller::get_time(ETimer t) const
//=========================================================
{
	if (eTimer_Hold == t)
	{
        return -m_pTimeHold->secsTo(QTime(0,0,0,0));
	}
	else
	{
        return m_pTimeMain->secsTo(QTime(0,0,0,0));
	}
}

//=========================================================
bool Controller::is_sonomama() const
//=========================================================
{
	return m_isSonoMama;
}

//=========================================================
bool Controller::is_golden_score() const
//=========================================================
{
	return current_fight().IsGoldenScore();
}

//=========================================================
void Controller::NextFight()
//=========================================================
{
	// move to Stopped state
	// (will stop all timers and thus save the current fight)
	m_pSM->process_event(IpponboardSM_::Finish());

	auto currentFight = GetCurrentFight();
	auto currentRound = GetCurrentRound();

	if (currentFight == GetFightCount() - 1)
	{
		if (currentRound < GetRoundCount() - 1)
		{
			SetCurrentRound(currentRound + 1);
			SetCurrentFight(0);
		}
	}
	else
	{
		SetCurrentFight(currentFight + 1);
	}
}

//=========================================================
void Controller::PrevFight()
//=========================================================
{
	// move to Stopped state
	// (will stop all timers and thus save the current fight)
	m_pSM->process_event(IpponboardSM_::Finish());

	auto currentFight = GetCurrentFight();
	auto currentRound = GetCurrentRound();

	if (currentFight == 0)
	{
		if (currentRound > 0)
		{
			SetCurrentRound(currentRound - 1);
			SetCurrentFight(GetFightCount() - 1);
		}
	}
	else
	{
		SetCurrentFight(currentFight - 1);
	}
}

//=========================================================
void Controller::SetCurrentFight(unsigned int index)
//=========================================================
{
	// now set pointer to next fight
	m_currentFight = index;
    *m_pTimeHold = QTime(0,0,0,0);
    m_roundTime = QTime(0,0,0,0).addSecs(m_mode.GetFightDuration(current_fight().weight));

	//FIXME: check this block
	if (current_fight().IsGoldenScore())
	{
        *m_pTimeMain = QTime(0,0,0,0).addSecs(current_fight().GetGoldenScoreTime());
	}
	else
	{
        *m_pTimeMain = QTime(0,0,0,0).addSecs(current_fight().GetRemainingTime());
	}

	// update state
	m_State = EState(m_pSM->current_state()[0]);
	Q_ASSERT(eState_TimerStopped == m_State);

	update_views();
}

//=========================================================
void Controller::SetCurrentRound(unsigned int index)
//=========================================================
{
	if (index < static_cast<unsigned int>(GetRoundCount()))
	{
		m_currentRound = index;
		update_views();
	}
}

//=========================================================
void Controller::ClearFightsAndResetTimers()
//=========================================================
{
	reset_timers();

	for (unsigned int round(0); round < m_Tournament.size(); ++round)
	{
		for (size_t fight(0); fight < m_Tournament[0]->size(); ++fight)
		{
			SetFight(round, fight, "", "", "", "", "");
			m_Tournament[round]->at(fight).SetSecondsElapsed(0);
		}
	}

	m_currentRound = 0;
	m_currentFight = 0;

	update_views();
}

//=========================================================
void Controller::SetClub(Ipponboard::FighterEnum who, const QString& clubName)
//=========================================================
{
	Q_ASSERT(who == Ipponboard::FighterEnum::First ||
			 who == Ipponboard::FighterEnum::Second);

	for (unsigned int round(0); round < m_Tournament.size(); ++round)
	{
		for (size_t fight(0); fight < m_Tournament[0]->size(); ++fight)
		{
			m_Tournament[round]->at(fight).GetFighter(who).club = clubName;
		}
	}

	update_views();
}

//=========================================================
void Controller::SetFight(
	unsigned int round_index, size_t fight_index,
	const QString& weight,
	const QString& first_player_name, const QString& first_player_club,
	const QString& second_player_name, const QString& second_player_club,
	int yuko1, int wazaari1, int ippon1, int shido1, int hansokumake1,
	int yuko2, int wazaari2, int ippon2, int shido2, int hansokumake2)
//=========================================================
{
	Ipponboard::Fight fight;
	fight.weight = weight;
	fight.SetSecondsElapsed(0);
	fight.rules = m_rules;

	// TODO: set fight.max_time_in_seconds
	// TODO: set fight.allSubscoresCount
	// (not set as setting fights is currently not used...): 130512
	auto First = FighterEnum::First;
	auto Second = FighterEnum::Second;
	auto Yuko = Score::Point::Yuko;
	auto Wazaari = Score::Point::Wazaari;
	auto Ippon = Score::Point::Ippon;
	auto Shido = Score::Point::Shido;
	auto Hansokumake = Score::Point::Hansokumake;

	fight.fighters[First].name = first_player_name.isEmpty() ? emptyFighterName : first_player_name;
	fight.fighters[First].club = first_player_club;
	fight.GetScore1().Clear();

	while (yuko1 != -1 && yuko1 > 0)
	{
		fight.GetScore1().Add(Yuko);
		--yuko1;
	}

	while (wazaari1 != -1 && wazaari1 > 0)
	{
		fight.GetScore1().Add(Wazaari);
		--wazaari1;
	}

	if (ippon1 > 0)
	{
		fight.GetScore1().Add(Ippon);
	}

	while (shido1 != -1 && shido1 > 0)
	{
		fight.GetScore1().Add(Shido);
		--shido1;
	}

	if (hansokumake1 > 0)
	{
		fight.GetScore1().Add(Hansokumake);
	}

	fight.fighters[Second].name = second_player_name.isEmpty() ? emptyFighterName : second_player_name;
	fight.fighters[Second].club = second_player_club;
	fight.GetScore2().Clear();

	while (yuko2 != -1 && yuko2 > 0)
	{
		fight.GetScore2().Add(Yuko);
		--yuko2;
	}

	while (wazaari2 != -1 && wazaari2 > 0)
	{
		fight.GetScore2().Add(Wazaari);
		--wazaari2;
	}

	if (ippon2 > 0)
	{
		fight.GetScore2().Add(Ippon);
	}

	while (shido2 != -1 && shido2 > 0)
	{
		fight.GetScore2().Add(Shido);
		--shido2;
	}

	if (hansokumake2 > 0)
	{
		fight.GetScore1().Add(Hansokumake);
	}

	m_Tournament[round_index]->at(fight_index) = fight;

	update_views();
}

//=========================================================
Ipponboard::Fight const& Controller::GetFight(
	unsigned int round_index,
	unsigned int fight_index) const
//=========================================================
{
	return m_Tournament[round_index]->at(fight_index);
}

//=========================================================
void Controller::SetFighterName(Ipponboard::FighterEnum whos,
								const QString& name)
//=========================================================
{
	m_Tournament[m_currentRound]
	->at(m_currentFight).fighters[whos].name = name;

	update_views();
}

//=========================================================
void Controller::SetWeights(QStringList const& weights)
//=========================================================
{
	if (weights.count() == GetFightCount())
	{
		for (int round(0); round < GetRoundCount(); ++round)
		{
			for (int fight(0); fight < GetFightCount(); ++fight)
			{
				Fight& f = m_Tournament.at(round)->at(fight);
				f.weight = weights.at(fight);
				f.SetRoundTime(m_mode.GetFightDuration(f.weight));
			}
		}
	}
	else
	{
		// duplicate each entry
		for (int round(0); round < GetRoundCount(); ++round)
		{
			for (int fight(0); fight < GetFightCount() - 1; ++fight)
			{
				Fight& f1 = m_Tournament.at(round)->at(fight);
				f1.weight = weights.at(fight / 2);
				f1.SetRoundTime(m_mode.GetFightDuration(f1.weight));

				Fight& f2 = m_Tournament.at(round)->at(fight + 1);
				f2.weight = weights.at(fight / 2);
				f2.SetRoundTime(m_mode.GetFightDuration(f2.weight));

				++fight;
			}
		}
	}

	for (auto const & pModel : m_TournamentModels)
	{
		pModel->SetDataChanged();
	}
}

//=========================================================
void Controller::CopyAndSwitchGuestFighters()
//=========================================================
{
	if (m_Tournament.size() != 2)
	{
		throw std::exception(); // FIXME: use correct exception!
	}

	for (int fight(0); fight < GetFightCount() - 1; ++fight)
	{
		m_Tournament[1]->at(fight).fighters[FighterEnum::First] =
			m_Tournament[0]->at(fight).fighters[FighterEnum::First];

		m_Tournament[1]->at(fight + 1).fighters[FighterEnum::First] =
			m_Tournament[0]->at(fight + 1).fighters[FighterEnum::First];

		m_Tournament[1]->at(fight + 1).fighters[FighterEnum::Second] =
			m_Tournament[0]->at(fight).fighters[FighterEnum::Second];

		m_Tournament[1]->at(fight).fighters[FighterEnum::Second] =
			m_Tournament[0]->at(fight + 1).fighters[FighterEnum::Second];

		++fight;
	}

	m_TournamentModels[1]->SetDataChanged();
}

//=========================================================
PTournamentModel Controller::GetTournamentScoreModel(int which)
//=========================================================
{
	if ((size_t)which < m_TournamentModels.size() && which > 0)
		return m_TournamentModels[which];

	return m_TournamentModels[0];
}

//=========================================================
void Controller::SetGongFile(const QString& s)
//=========================================================
{
	m_gongFile = s;
}

//=========================================================
QString const& Controller::GetGongFile() const
//=========================================================
{
	return m_gongFile;
}

//=========================================================
void Controller::update_main_time()
//=========================================================
{
	if (m_rules->IsOption_OpenEndGoldenScore() && is_golden_score())
	{
		*m_pTimeMain = m_pTimeMain->addSecs(1);
	}
	else
	{
		*m_pTimeMain = m_pTimeMain->addSecs(-1);

        const bool isTimeUp = QTime(0,0,0,0).secsTo(*m_pTimeMain) <= 0;

		// correct time again
        const int secsTo(QTime(0,0,0,0).secsTo(*m_pTimeMain));

		if (secsTo < 0 || *m_pTimeMain > m_roundTime)
            m_pTimeMain->setHMS(0,0,0,0);

		if (eState_TimerRunning == m_State)
		{
			if (isTimeUp)
			{
				m_pSM->process_event(IpponboardSM_::TimeEndedEvent());
				m_State = EState(m_pSM->current_state()[0]);
				Gong();
			}
		}

		// else (stopped or ended) --> do nothing
	}

	update_views();
}

//=========================================================
void Controller::update_hold_time()
//=========================================================
{
	if (eState_Holding != m_State)
		return;

	*m_pTimeHold = m_pTimeHold->addSecs(1);
	const int secs = m_pTimeHold->second();

	if (secs > 0 &&
			(m_rules->GetOsaekomiValue(Score::Point::Yuko) == secs
			 || m_rules->GetOsaekomiValue(Score::Point::Wazaari) == secs
			 || m_rules->GetOsaekomiValue(Score::Point::Ippon) == secs))
	{
		m_pSM->process_event(IpponboardSM_::HoldTimeEvent(secs, m_Tori));
		m_State = EState(m_pSM->current_state()[0]);

		if (eState_TimerStopped == m_State)
			Gong();
	}

	update_views();
}

//=========================================================
void Controller::update_views() const
//=========================================================
{
	for (auto const & pView : m_views)
	{
		pView->UpdateView();
	}

	for (auto const & pView : m_goldenScoreViews)
	{
		pView->UpdateGoldenScoreView();
	}
}
