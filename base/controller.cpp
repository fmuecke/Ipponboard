#include "controller.h"
#include "iview.h"
#include "score.h"
#include "tournamentmodel.h"

#include <QTimer>
#include <QSound>
#include <QFileInfo>
#include <QMessageBox>
#include <algorithm>
#include <functional>
#include "statemachine.h"

using namespace Ipponboard;

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

//=========================================================
Controller::Controller()
	//: m_TournamentScoreModels[] = ...
	// m_TournamentModelsPtrs[] = ...
	: m_currentFight(0)
	, m_currentTournament(0)
	, m_pSM(0)
	, m_State(eState_TimerStopped)
	, m_pTimerMain(0)
	, m_pTimerHold(0)
	, m_pTimeMain(0)
	, m_pTimeHold(0)
	, m_Tori(eFighter_Nobody)
	, setPointsInOsaekomi(false)
	, m_isSonoMama(false)
	, m_roundTime(0,4,0,0)
//=========================================================
{
	m_pSM = new IpponboardSM();
	m_pSM->SetCore( this );
	m_pTimerMain = new QTimer(this);
	m_pTimerHold = new QTimer(this);
	m_pTimeMain = new QTime();
	m_pTimeHold = new QTime();

	m_TournamentModelsPtrs[0] = new TournamentModel(&m_TournamentScores[0]);
	m_TournamentModelsPtrs[1] = new TournamentModel(&m_TournamentScores[1]);

	Reset_();
	m_pSM->start();

	connect( m_pTimerMain, SIGNAL(timeout()), this, SLOT(UpdateTime_()) );
	connect( m_pTimerHold, SIGNAL(timeout()), this, SLOT(UpdateHoldTime_()) );
}

//=========================================================
Controller::~Controller()
//=========================================================
{
	m_Views.clear();
	delete m_TournamentModelsPtrs[0];
	delete m_TournamentModelsPtrs[1];
	//delete m_pTimeHold;
	//delete m_pTimeMain;
	delete m_pSM;
}

//=========================================================
int Controller::GetScore( EFighter whos, EPoint point ) const
//=========================================================
{
	int value(0);
	const Score& score = GetScore_(whos);
	switch(point)
	{
		case ePoint_Yuko:
			value = score.Yuko();
			break;

		case ePoint_Wazaari:
			value = /*score.IsAwaseteIppon() ? 0 : */score.Wazaari();
			break;

		case ePoint_Ippon:
			if( score.Ippon() || score.IsAwaseteIppon() )
				value =  1;
			else
				value = 0;
			break;

		case ePoint_Shido:
			value = score.Shido();
			break;

		case ePoint_Hansokumake:
			value = score.Hansokumake()? 1 : 0;
			break;

		default:
			//Q_ASSERT(!"Unknown case in switch!");
			break;
	}
	return value;
}


//=========================================================
void Controller::DoAction( EAction action, EFighter whos, bool doRevoke )
//=========================================================
{
	if( doRevoke )
	{
		switch( action )
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

		default:
			return;
		}
	}
	else
	{
		switch( action )
		{
		case eAction_Hajime_Mate:
			if( eState_Holding == m_State )
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

		case eAction_Reset:
			m_pSM->process_event(IpponboardSM_::Reset());
			break;

		case eAction_ResetOsaeKomi:
			m_pTimeHold->setHMS(0,0,0,0);
			break;

		default:
			//Q_ASSERT(!"wrong action/action not handled!");
			break;
		}
	}
//	if( eState_SonoMama == m_State && action != eAction_SonoMama_Yoshi )
//		return;
//

	m_State = EState(m_pSM->current_state()[0]);

	UpdateViews_();
}


//=========================================================
EFighter Controller::GetLead() const
//=========================================================
{
	EFighter winner(eFighter_Nobody);

	switch( m_State )
	{
		case eState_TimerRunning:
		case eState_TimerStopped:
		{
			// determine who has the lead
			if ( GetScore_(eFighter_Blue).Wazaari() > GetScore_(eFighter_White).Wazaari() )
			{
				winner = eFighter_Blue;
			}
			else if ( GetScore_(eFighter_Blue).Wazaari() < GetScore_(eFighter_White).Wazaari() )
			{
				winner = eFighter_White;
			}
			else  // GetScore_(eBlue).Wazaari() == GetScore_(eWhite).Wazaari()
			{
				if ( GetScore_(eFighter_Blue).Yuko() >
					 GetScore_(eFighter_White).Yuko() )
				{
					winner = eFighter_Blue;
				}
				else if ( GetScore_(eFighter_Blue).Yuko() < GetScore_(eFighter_White).Yuko() )
				{
					winner = eFighter_White;
				}
				else  // GetScore_(eBlue).Yuko() == GetScore_(eWhite).Yuko()
				{
					if ( GetScore_(eFighter_Blue).Shido() < GetScore_(eFighter_White).Shido() &&
						 GetScore_(eFighter_White).Shido() > 1 ) // no "koka"!
						winner = eFighter_Blue;
					else if ( GetScore_(eFighter_Blue).Shido() > GetScore_(eFighter_White).Shido() &&
							  GetScore_(eFighter_Blue).Shido() > 1)
						winner = eFighter_White;
					else
					{
						// equal ==> hantai or golden score
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
Ipponboard::EFighter Controller::GetLastHolder() const
//=========================================================
{
	return m_Tori;
}


//=========================================================
void Controller::Reset_()
//=========================================================
{
	m_State = eState_TimerStopped;

	Q_ASSERT(m_pTimeMain);
	Q_ASSERT(m_pTimeHold);

	m_pTimerMain->stop();
	m_pTimerHold->stop();

	ResetTimerValue(eTimer_Main);
	ResetTimerValue(eTimer_Hold);

	m_isSonoMama = false;

	ClearFights();
	SetFight( 0, 0, "-XX", tr("Blue"), "", tr("White"), "" );

	UpdateViews_();
}

//=========================================================
const QString Controller::GetTimeText( ETimer timer ) const
//=========================================================
{
	QString ret;
	switch(timer)
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
const QString Controller::GetFighterName( EFighter who ) const
//=========================================================
{
	Q_ASSERT( who == eFighter_Blue || who == eFighter_White );

	QString name = m_TournamentScores[m_currentTournament].at(m_currentFight).fighters[who].name;

	// shorten name
	const int pos = name.indexOf(' ');
	if( pos != -1 )
	{
		name = name.left(1) + ". " + name.mid(pos, name.length() - pos);
	}
	return name;
}

//=========================================================
const QString Controller::GetFighterLastName( Ipponboard::EFighter who ) const
//=========================================================
{
	Q_ASSERT( who == eFighter_Blue || who == eFighter_White );

	QString name = m_TournamentScores[m_currentTournament].at(m_currentFight).fighters[who].name;

	// get last name
	const int pos = name.indexOf(' ');
	if( pos != -1 )
	{
		name = name.mid(pos + 1, name.length() - pos);
	}
	return name;
}

//=========================================================
const QString Controller::GetFighterFirstName( Ipponboard::EFighter who ) const
//=========================================================
{
	Q_ASSERT( who == eFighter_Blue || who == eFighter_White );

	QString name = m_TournamentScores[m_currentTournament].
				   at(m_currentFight).fighters[who].name;

	// get first name
	const int pos = name.indexOf(' ');
	if( pos != -1 )
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
const QString Controller::GetFighterClub( EFighter who ) const
//=========================================================
{
	Q_ASSERT( who == eFighter_Blue || who == eFighter_White );

	return m_TournamentScores[m_currentTournament].
			at(m_currentFight).fighters[who].club;
}

//=========================================================
const QString& Controller::GetWeight() const
//=========================================================
{
	return m_TournamentScores[m_currentTournament].at(m_currentFight).weight;
}

//=========================================================
const QString Controller::GetMessage() const
//=========================================================
{
	return m_Message;
}

//=========================================================
const int Controller::GetTeamScore( Ipponboard::EFighter who ) const
//=========================================================
{
	int score(0);
	for( int i(0); i<eTournament_FightCount; ++i )
	{
		if( m_TournamentScores[0].at(i).is_saved )
			score += m_TournamentScores[0].at(i).HasWon(who);
		if( m_TournamentScores[1].at(i).is_saved )
			score += m_TournamentScores[1].at(i).HasWon(who);
	}

	return score;
}

//=========================================================
void Controller::SetTimerValue( Ipponboard::ETimer timer, const QString& value )
//=========================================================
{
	if( eState_TimerStopped == m_State ||
		eState_SonoMama == m_State)
	{
		if( eTimer_Main == timer )
		{
			*m_pTimeMain = QTime::fromString(value, "m:ss");
		}
		else if( eTimer_Hold == timer )
		{
			m_pTimeHold->setHMS(0,0,value.toInt());
		}
		UpdateViews_();
	}
}

//=========================================================
void Controller::ResetTimerValue( Ipponboard::ETimer timer )
//=========================================================
{
	if( eTimer_Main == timer )
	{
		*m_pTimeMain = m_roundTime;
	}
	else if( eTimer_Hold == timer )
	{
		m_pTimeHold->setHMS(0,0,0,0);
		m_Tori = eFighter_Nobody;
	}
	UpdateViews_();
}

//=========================================================
void Controller::SetRoundTime( const QString& value )
//=========================================================
{
	SetRoundTime(QTime::fromString(value, "m:ss"));
}

//=========================================================
void Controller::SetRoundTime( QTime const& time )
//=========================================================
{
	m_roundTime = time;
	*m_pTimeMain = m_roundTime;

	UpdateViews_();
}

//=========================================================
int Ipponboard::Controller::GetRound() const
//=========================================================
{
	return m_currentTournament * 10 + m_currentFight + 1;
}

//=========================================================
void Ipponboard::Controller::SetWeightClass(QString const& c)
//=========================================================
{
	m_weight_class = c;
}

//=========================================================
void Controller::Gong() const
//=========================================================
{
	QSound::play(m_gongFile);
}

//=========================================================
void Controller::RegisterView( IView* pView )
//=========================================================
{
	m_Views.insert( pView );
	pView->SetController( this );

	// do not call UpdateViews here as views may not have been fully created
}

//=========================================================
void Controller::StartTimer_( ETimer t )
//=========================================================
{
	if( eTimer_Main == t )
		m_pTimerMain->start(1000);
	else
		m_pTimerHold->start(1000);
}

//=========================================================
void Controller::StopTimer_( ETimer t )
//=========================================================
{
	if( eTimer_Main == t )
	{
		m_pTimerMain->stop();
		CurrentFight_().time_in_seconds = m_pTimeMain->secsTo(m_roundTime);
	}
	else
	{
		m_pTimerHold->stop();
	}
}

//=========================================================
void Controller::ResetFight_()
//=========================================================
{
	m_pTimerHold->stop();
	m_pTimerMain->stop();
	m_pTimeHold->setHMS(0,0,0,0);
	*m_pTimeMain = m_roundTime;
	m_Tori = eFighter_Nobody;

	// just clear the score, not the names
	GetScore_(eFighter_Blue) = Score();
	GetScore_(eFighter_White) = Score();
	Fight& fight = m_TournamentScores[m_currentTournament].at(m_currentFight);
	fight.time_in_seconds = 0;
	fight.is_saved = false;

	std::for_each( m_Views.begin(), m_Views.end(), std::mem_fun(&IView::Reset) );
}

//=========================================================
void Controller::ResetTimer_( ETimer t )
//=========================================================
{
	if( eTimer_Hold == t )
	{
		*m_pTimeHold = QTime();
		m_Tori = eFighter_Nobody;
	}
	else
	{
		*m_pTimeMain = m_roundTime;
	}

	UpdateViews_();
}

//=========================================================
Score& Controller::GetScore_( EFighter who )
//=========================================================
{
	Q_ASSERT( who == eFighter_Blue || who == eFighter_White );
	return m_TournamentScores[m_currentTournament].at(m_currentFight).scores[who];
}

//=========================================================
const Score& Controller::GetScore_( EFighter who ) const
//=========================================================
{
	Q_ASSERT( who == eFighter_Blue || who == eFighter_White );
	return m_TournamentScores[m_currentTournament].at(m_currentFight).scores[who];
}

//=========================================================
const int Controller::GetTime_( ETimer t ) const
//=========================================================
{
	if( eTimer_Hold == t )
		return -m_pTimeHold->secsTo(QTime(0,0,0,0));
	else
		return m_pTimeMain->secsTo(QTime(0,0,0,0));
}

//=========================================================
bool Controller::IsSonoMama_() const
//=========================================================
{
	return m_isSonoMama;
}

//=========================================================
void Controller::SetCurrentFight( unsigned int index )
//=========================================================
{
	// move to Stopped state
	// (will stop all timers and thus save the current fight time)
	m_pSM->process_event(IpponboardSM_::Finish());

	// set prev fight so saved
	m_TournamentScores[m_currentTournament]
		.at(m_currentFight).is_saved = true;

	// now set pointer to next fight
	m_currentFight = index;
	*m_pTimeMain = m_roundTime;
	*m_pTimeMain = m_pTimeMain->addSecs(-CurrentFight_().time_in_seconds);
	*m_pTimeHold = QTime();

	// update state
	m_State = EState(m_pSM->current_state()[0]);
	assert( eState_TimerStopped == m_State );

	UpdateViews_();
}

//=========================================================
void Controller::ClearFights()
//=========================================================
{
	for(unsigned int i(0); i < m_TournamentScores[0].size(); ++i )
	{
		SetFight(0, i, "", "", "", "", "");
		SetFight(1, i, "", "", "", "", "");
		m_TournamentScores[0].at(i).time_in_seconds = 0;
		m_TournamentScores[1].at(i).time_in_seconds = 0;
	}
	m_currentTournament = 0;
	m_currentFight = 0;

	UpdateViews_();
}


//=========================================================
void Controller::SetClub( Ipponboard::EFighter whos, const QString& clubName )
//=========================================================
{
	Q_ASSERT( whos == Ipponboard::eFighter_Blue || whos == Ipponboard::eFighter_White );
	for(unsigned int i(0); i < m_TournamentScores[0].size(); ++i )
	{
		m_TournamentScores[0].at(i).fighters[whos].club = clubName;
		m_TournamentScores[1].at(i).fighters[whos].club = clubName;
	}

	UpdateViews_();
}

//=========================================================
void Controller::SetFight(
		unsigned int tournament_index, unsigned int fight_index,
		const QString& weight,
		const QString& first_player_name, const QString& first_player_club,
		const QString& second_player_name, const QString& second_player_club,
		int yuko1, int wazaari1, int ippon1, int shido1, int hansokumake1,
		int yuko2, int wazaari2, int ippon2, int shido2, int hansokumake2 )
//=========================================================
{
	Ipponboard::Fight fight;
	fight.weight = weight;

	fight.fighters[Ipponboard::eFighter_Blue].name = first_player_name;
	fight.fighters[Ipponboard::eFighter_Blue].club = first_player_club;
	fight.scores[Ipponboard::eFighter_Blue].Clear();
	while( yuko1 != -1 && yuko1 > 0)
	{
		fight.scores[Ipponboard::eFighter_Blue].Add(Ipponboard::ePoint_Yuko);
		--yuko1;
	}
	while( wazaari1 != -1 && wazaari1 > 0 )
	{
		fight.scores[Ipponboard::eFighter_Blue].Add(Ipponboard::ePoint_Wazaari);
		--wazaari1;
	}
	if( ippon1 > 0 )
		fight.scores[Ipponboard::eFighter_Blue].Add(Ipponboard::ePoint_Ippon);
	while( shido1 != -1 && shido1 > 0 )
	{
		fight.scores[Ipponboard::eFighter_Blue].Add(Ipponboard::ePoint_Shido);
		--shido1;
	}
	if( hansokumake1 > 0 )
		fight.scores[Ipponboard::eFighter_Blue].Add(Ipponboard::ePoint_Hansokumake);

	fight.fighters[Ipponboard::eFighter_White].name = second_player_name;
	fight.fighters[Ipponboard::eFighter_White].club = second_player_club;
	fight.scores[Ipponboard::eFighter_White].Clear();
	while( yuko2 != -1 && yuko2 > 0)
	{
		fight.scores[Ipponboard::eFighter_White].Add(Ipponboard::ePoint_Yuko);
		--yuko2;
	}
	while( wazaari2 != -1 && wazaari2 > 0 )
	{
		fight.scores[Ipponboard::eFighter_White].Add(Ipponboard::ePoint_Wazaari);
		--wazaari2;
	}
	if( ippon2 > 0 )
		fight.scores[Ipponboard::eFighter_White].Add(Ipponboard::ePoint_Ippon);
	while( shido2 != -1 && shido2 > 0 )
	{
		fight.scores[Ipponboard::eFighter_White].Add(Ipponboard::ePoint_Shido);
		--shido2;
	}
	if( hansokumake2 > 0 )
		fight.scores[Ipponboard::eFighter_Blue].Add(Ipponboard::ePoint_Hansokumake);

	fight.time_in_seconds = 0;

	m_TournamentScores[tournament_index].at(fight_index) = fight;

	UpdateViews_();
}

//=========================================================
const Ipponboard::Fight& Controller::GetFight(
	unsigned int tournament_index,
	unsigned int fight_index ) const
//=========================================================
{
	 return m_TournamentScores[tournament_index].at(fight_index);
}

//=========================================================
void Controller::SetFighterName( Ipponboard::EFighter whos, const QString& name )
//=========================================================
{
	m_TournamentScores[m_currentTournament].
			at(m_currentFight).fighters[whos].name = name;
	UpdateViews_();
}

//=========================================================
TournamentModel* Controller::GetTournamentScoreModel( int which )
//=========================================================
{
	if (1 == which )
		return m_TournamentModelsPtrs[1];
	return m_TournamentModelsPtrs[0];
}

//=========================================================
void Controller::SetGongFile( const QString& s )
//=========================================================
{
	m_gongFile = s;
}

//=========================================================
const QString& Controller::GetGongFile() const
//=========================================================
{
	return m_gongFile;
}

//=========================================================
void Controller::UpdateTime_()
//=========================================================
{
	*m_pTimeMain = m_pTimeMain->addSecs(-1);
	const bool isTimeUp = QTime(0,0,0,0).secsTo(*m_pTimeMain) <= 0;

	// correct time again
	const int secsTo(QTime(0,0,0,0).secsTo(*m_pTimeMain));
	if( secsTo < 0 || *m_pTimeMain > m_roundTime )
		m_pTimeMain->setHMS(0,0,0,0);

	if( eState_Holding == m_State )
	{
		if( isTimeUp )
			// do stop timer, but do not process event
			StopTimer_( eTimer_Main );
	}
	else if( eState_TimerRunning == m_State )
	{
		if( isTimeUp )
		{
			m_pSM->process_event(IpponboardSM_::TimeEndedEvent());
			m_State = EState(m_pSM->current_state()[0]);
			Gong();
		}
	}
	// else (stopped or ended) --> do nothing

	UpdateViews_();
}

//=========================================================
void Controller::UpdateHoldTime_()
//=========================================================
{
	if( eState_Holding != m_State )
		return;

	*m_pTimeHold = m_pTimeHold->addSecs(1);
	const int secs = m_pTimeHold->second();
	if( eOsaekomiVal_Yuko == secs ||
		eOsaekomiVal_Wazaari == secs ||
		eOsaekomiVal_Ippon == secs )
	{
		m_pSM->process_event(IpponboardSM_::HoldTimeEvent( secs, m_Tori) );
		m_State = EState(m_pSM->current_state()[0]);
		if( eState_TimerStopped == m_State )
			Gong();
	}

	UpdateViews_();
}

//=========================================================
void Controller::UpdateViews_() const
//=========================================================
{
	std::for_each( m_Views.begin(), m_Views.end(), std::mem_fun(&IView::UpdateView) );
}
