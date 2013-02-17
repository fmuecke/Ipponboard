#include "controller.h"
#include "iview.h"
#include "score.h"
#include "enums.h"
#include "tournamentmodel.h"

#include <QTimer>
#include <QSound>
#include <QFileInfo>
#include <QMessageBox>
#include <algorithm>
#include <functional>
#include "statemachine.h"
#include <boost/foreach.hpp>

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
    , m_Tori(eFighterNobody)
    , setPointsInOsaekomi(false)
    , m_isSonoMama(false)
    , m_isGoldenScore(false)
    , m_roundTime(0, 5, 0, 0)
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

    m_TournamentModelsPtrs[0] = new TournamentModel(&m_TournamentScores[0]);
    m_TournamentModelsPtrs[1] = new TournamentModel(&m_TournamentScores[1]);

    reset();
    m_pSM->start();

    connect(m_pTimerMain, SIGNAL(timeout()), this, SLOT(update_main_time()));
    connect(m_pTimerHold, SIGNAL(timeout()), this, SLOT(update_hold_time()));
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
int Controller::GetScore(EFighter whos, EPoint point) const
//=========================================================
{
    int value(0);
    const Score& score = get_score(whos);

    switch (point)
    {
    case ePoint_Yuko:
        value = score.Yuko();
        break;

    case ePoint_Wazaari:
        value = /*score.IsAwaseteIppon() ? 0 : */score.Wazaari();
        break;

    case ePoint_Ippon:
        if (score.Ippon() || score.IsAwaseteIppon())
            value =  1;
        else
            value = 0;

        break;

    case ePoint_Shido:
        value = score.Shido();
        break;

    case ePoint_Hansokumake:
        value = score.Hansokumake() ? 1 : 0;
        break;

    default:
        //Q_ASSERT(!"Unknown case in switch!");
        break;
    }

    return value;
}


//=========================================================
void Controller::DoAction(EAction action, EFighter whos, bool doRevoke)
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
    // TODO: use state machine!
    if (m_isGoldenScore
            && eState_TimerStopped != EState(m_pSM->current_state()[0]))
    {
        // Note: In golden score the hold should not end after the first
        //       scored point!
        if (eState_Holding != EState(m_pSM->current_state()[0]))
        {
            if (get_score(eFighter1) < get_score(eFighter2) ||
                get_score(eFighter2) < get_score(eFighter1))
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
EFighter Controller::GetLead() const
//=========================================================
{
    EFighter winner(eFighterNobody);

    switch (m_State)
    {
    case eState_TimerRunning:
    case eState_TimerStopped:
        {
            // determine who has the lead
            if (get_score(eFighter1).Wazaari() > get_score(eFighter2).Wazaari())
            {
                winner = eFighter1;
            }
            else if (get_score(eFighter1).Wazaari() < get_score(eFighter2).Wazaari())
            {
                winner = eFighter2;
            }
            else  // GetScore_(eFirst).Wazaari() == GetScore_(eSecond).Wazaari()
            {
                if (get_score(eFighter1).Yuko() >
                        get_score(eFighter2).Yuko())
                {
                    winner = eFighter1;
                }
                else if (get_score(eFighter1).Yuko() < get_score(eFighter2).Yuko())
                {
                    winner = eFighter2;
                }
                else  // GetScore_(eFirst).Yuko() == GetScore_(eSecond).Yuko()
                {
                    if (get_score(eFighter1).Shido() < get_score(eFighter2).Shido() &&
                            get_score(eFighter2).Shido() > 1)  // no "koka"!
                        winner = eFighter1;
                    else if (get_score(eFighter1).Shido() > get_score(eFighter2).Shido() &&
                             get_score(eFighter1).Shido() > 1)
                        winner = eFighter2;
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
Ipponboard::EFighter Controller::GetLastHolder() const
//=========================================================
{
    return m_Tori;
}


//=========================================================
void Controller::reset()
//=========================================================
{
    m_State = eState_TimerStopped;

    Q_ASSERT(m_pTimeMain);
    Q_ASSERT(m_pTimeHold);

    m_pTimerMain->stop();
    reset_timer_value(eTimer_Main);

    m_pTimerHold->stop();
    reset_timer_value(eTimer_Hold);
    m_Tori = eFighterNobody;

    m_isSonoMama = false;
    m_isGoldenScore = false;

    ClearFights();
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
        *m_pTimeMain = m_roundTime;
    }
    else if (eTimer_Hold == timer)
    {
        m_pTimeHold->setHMS(0, 0, 0, 0);

        if (!m_pTimerHold->isActive())
            m_Tori = eFighterNobody;
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
QString Controller::GetFighterName(EFighter who) const
//=========================================================
{
    Q_ASSERT(who == eFighter1 || who == eFighter2);

    QString name = m_TournamentScores[m_currentTournament]
                   .at(m_currentFight).fighters[who].name;

    // shorten name
    const int pos = name.indexOf(' ');

    if (pos != -1)
    {
        name = name.left(1) + ". " + name.mid(pos, name.length() - pos);
    }

    return name;
}

//=========================================================
QString Controller::GetFighterLastName(Ipponboard::EFighter who) const
//=========================================================
{
    Q_ASSERT(who == eFighter1 || who == eFighter2);

    QString name = m_TournamentScores[m_currentTournament]
                   .at(m_currentFight).fighters[who].name;

    // get last name
    const int pos = name.indexOf(' ');

    if (pos != -1)
    {
        name = name.mid(pos + 1, name.length() - pos);
    }

    return name;
}

//=========================================================
QString Controller::GetFighterFirstName(Ipponboard::EFighter who) const
//=========================================================
{
    Q_ASSERT(who == eFighter1 || who == eFighter2);

    QString name = m_TournamentScores[m_currentTournament].
                   at(m_currentFight).fighters[who].name;

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
QString Controller::GetFighterClub(EFighter who) const
//=========================================================
{
    Q_ASSERT(who == eFighter1 || who == eFighter2);

    return m_TournamentScores[m_currentTournament].
           at(m_currentFight).fighters[who].club;
}

//=========================================================
QString const& Controller::GetWeight() const
//=========================================================
{
    return m_TournamentScores[m_currentTournament]
           .at(m_currentFight).weight;
}

//=========================================================
QString Controller::GetMessage() const
//=========================================================
{
    return m_Message;
}

//=========================================================
int Controller::GetTeamScore(Ipponboard::EFighter who) const
//=========================================================
{
    int score(0);

    for (int i(0); i < eTournament_FightCount; ++i)
    {
        if (m_TournamentScores[0].at(i).is_saved)
            score += m_TournamentScores[0].at(i).HasWon(who);

        if (m_TournamentScores[1].at(i).is_saved)
            score += m_TournamentScores[1].at(i).HasWon(who);
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
    *m_pTimeMain = m_roundTime;

    update_views();
}

//=========================================================
QString Controller::GetRoundTime() const
//=========================================================
{
    return m_roundTime.toString("m:ss");
}

//=========================================================
int Controller::GetRound() const
//=========================================================
{
    return m_currentTournament * 10 + m_currentFight + 1;
}

//=========================================================
int Controller::GetRoundTimeSecs() const
//=========================================================
{
    return QTime(0, 0, 0, 0).secsTo(m_roundTime);
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
    m_isGoldenScore = isGS;
}

//=========================================================
void Controller::SetOption(EOption option, bool isSet)
//=========================================================
{
    m_options.set(option, isSet);
}

//=========================================================
bool Controller::GetOption(EOption option) const
//=========================================================
{
    return m_options.test(option);
}

//=========================================================
void Controller::SetLabels(const QString &home, const QString &guest)
//=========================================================
{
    m_labelHome = home;
    m_labelGuest = guest;
}

//=========================================================
void Controller::Gong() const
//=========================================================
{
    QSound::play(m_gongFile);
}

//=========================================================
void Controller::RegisterView(IView* pView)
//=========================================================
{
    m_Views.insert(pView);
    pView->SetController(this);

    // do not call UpdateViews here as views may not have been fully created
}

//=========================================================
void Controller::start_timer(ETimer t)
//=========================================================
{
    if (eTimer_Main == t)
        m_pTimerMain->start(1000);
    else
        m_pTimerHold->start(1000);
}

//=========================================================
void Controller::stop_timer(ETimer t)
//=========================================================
{
    m_pTimerHold->stop();

    if (eTimer_Main == t)
    {
        m_pTimerMain->stop();
        current_fight().time_in_seconds = m_pTimeMain->secsTo(m_roundTime);
    }
}

//=========================================================
void Controller::reset_fight()
//=========================================================
{
    m_pTimerHold->stop();
    m_pTimerMain->stop();
    m_pTimeHold->setHMS(0, 0, 0, 0);
    *m_pTimeMain = m_roundTime;
    m_Tori = eFighterNobody;

    // just clear the score, not the names
    get_score(eFighter1) = Score();
    get_score(eFighter2) = Score();
    Fight& fight = m_TournamentScores[m_currentTournament].at(m_currentFight);
    fight.time_in_seconds = 0;
    fight.is_saved = false;

    std::for_each(m_Views.begin(), m_Views.end(), std::mem_fun(&IView::Reset));
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
Score& Controller::get_score(EFighter who)
//=========================================================
{
    Q_ASSERT(who == eFighter1 || who == eFighter2);

    return m_TournamentScores[m_currentTournament]
           .at(m_currentFight).scores[who];
}

//=========================================================
Score const& Controller::get_score(EFighter who) const
//=========================================================
{
    Q_ASSERT(who == eFighter1 || who == eFighter2);

    return m_TournamentScores[m_currentTournament]
           .at(m_currentFight).scores[who];
}

//=========================================================
int Controller::get_time(ETimer t) const
//=========================================================
{
    if (eTimer_Hold == t)
        return -m_pTimeHold->secsTo(QTime(0, 0, 0, 0));
    else
        return m_pTimeMain->secsTo(QTime(0, 0, 0, 0));
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
    return m_isGoldenScore;
}

//=========================================================
void Controller::SetCurrentFight(unsigned int index)
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
    *m_pTimeMain = m_pTimeMain->addSecs(-current_fight().time_in_seconds);
    *m_pTimeHold = QTime();

    // update state
    m_State = EState(m_pSM->current_state()[0]);
    Q_ASSERT(eState_TimerStopped == m_State);

    update_views();
}

//=========================================================
void Controller::ClearFights()
//=========================================================
{
    for (unsigned int i(0); i < m_TournamentScores[0].size(); ++i)
    {
        SetFight(0, i, "", "", "", "", "");
        SetFight(1, i, "", "", "", "", "");
        m_TournamentScores[0].at(i).time_in_seconds = 0;
        m_TournamentScores[1].at(i).time_in_seconds = 0;
    }

    m_currentTournament = 0;
    m_currentFight = 0;

    update_views();
}


//=========================================================
void Controller::SetClub(Ipponboard::EFighter whos, const QString& clubName)
//=========================================================
{
    Q_ASSERT(whos == Ipponboard::eFighter1 ||
             whos == Ipponboard::eFighter2);

    for (unsigned int i(0); i < m_TournamentScores[0].size(); ++i)
    {
        m_TournamentScores[0].at(i).fighters[whos].club = clubName;
        m_TournamentScores[1].at(i).fighters[whos].club = clubName;
    }

    update_views();
}

//=========================================================
void Controller::SetFight(
    unsigned int tournament_index, unsigned int fight_index,
    const QString& weight,
    const QString& first_player_name, const QString& first_player_club,
    const QString& second_player_name, const QString& second_player_club,
    int yuko1, int wazaari1, int ippon1, int shido1, int hansokumake1,
    int yuko2, int wazaari2, int ippon2, int shido2, int hansokumake2)
//=========================================================
{
    Ipponboard::Fight fight;
    fight.weight = weight;

    fight.fighters[Ipponboard::eFighter1].name = first_player_name;
    fight.fighters[Ipponboard::eFighter1].club = first_player_club;
    fight.scores[Ipponboard::eFighter1].Clear();

    while (yuko1 != -1 && yuko1 > 0)
    {
        fight.scores[Ipponboard::eFighter1].Add(Ipponboard::ePoint_Yuko);
        --yuko1;
    }

    while (wazaari1 != -1 && wazaari1 > 0)
    {
        fight.scores[Ipponboard::eFighter1].Add(Ipponboard::ePoint_Wazaari);
        --wazaari1;
    }

    if (ippon1 > 0)
        fight.scores[Ipponboard::eFighter1].Add(Ipponboard::ePoint_Ippon);

    while (shido1 != -1 && shido1 > 0)
    {
        fight.scores[Ipponboard::eFighter1].Add(Ipponboard::ePoint_Shido);
        --shido1;
    }

    if (hansokumake1 > 0)
        fight.scores[Ipponboard::eFighter1].Add(Ipponboard::ePoint_Hansokumake);

    fight.fighters[Ipponboard::eFighter2].name = second_player_name;
    fight.fighters[Ipponboard::eFighter2].club = second_player_club;
    fight.scores[Ipponboard::eFighter2].Clear();

    while (yuko2 != -1 && yuko2 > 0)
    {
        fight.scores[Ipponboard::eFighter2].Add(Ipponboard::ePoint_Yuko);
        --yuko2;
    }

    while (wazaari2 != -1 && wazaari2 > 0)
    {
        fight.scores[Ipponboard::eFighter2].Add(Ipponboard::ePoint_Wazaari);
        --wazaari2;
    }

    if (ippon2 > 0)
        fight.scores[Ipponboard::eFighter2].Add(Ipponboard::ePoint_Ippon);

    while (shido2 != -1 && shido2 > 0)
    {
        fight.scores[Ipponboard::eFighter2].Add(Ipponboard::ePoint_Shido);
        --shido2;
    }

    if (hansokumake2 > 0)
        fight.scores[Ipponboard::eFighter1].Add(Ipponboard::ePoint_Hansokumake);

    fight.time_in_seconds = 0;

    m_TournamentScores[tournament_index].at(fight_index) = fight;

    update_views();
}

//=========================================================
Ipponboard::Fight const& Controller::GetFight(
    unsigned int tournament_index,
    unsigned int fight_index) const
//=========================================================
{
    return m_TournamentScores[tournament_index].at(fight_index);
}

//=========================================================
void Controller::SetFighterName(Ipponboard::EFighter whos,
                                const QString& name)
//=========================================================
{
    m_TournamentScores[m_currentTournament].
    at(m_currentFight).fighters[whos].name = name;
    update_views();
}

//=========================================================
void Controller::SetWeights(QStringList const& weights)
//=========================================================
{
    if (weights.count() == GetFightCount())
    {
        for (int i(0); i < GetFightCount(); ++i)
        {
            m_TournamentScores[0].at(i).weight = weights.at(i);
            m_TournamentScores[1].at(i).weight = weights.at(i);
        }
    }
    else
    {
        // duplicate each entry
        for (int i(0); i < GetFightCount(); ++i)
        {
            m_TournamentScores[0].at(i).weight = weights.at(i / 2);
            m_TournamentScores[0].at(i + 1).weight = weights.at(i / 2);
            ++i;
        }

        for (int i(0); i < GetFightCount(); ++i)
        {
            m_TournamentScores[1].at(i).weight = weights.at(i / 2);
            m_TournamentScores[1].at(i + 1).weight = weights.at(i / 2);
            ++i;
        }
    }

    BOOST_FOREACH(TournamentModel* pModel, m_TournamentModelsPtrs)
    {
        pModel->SetDataChanged();
    }

}

//=========================================================
void Controller::CopyAndSwitchGuestFighters()
//=========================================================
{
    for (int i(0); i < GetFightCount()-1; ++i)
    {
        m_TournamentScores[1].at(i).fighters[eFighter1] =
            m_TournamentScores[0].at(i).fighters[eFighter1];

        m_TournamentScores[1].at(i + 1).fighters[eFighter1] =
            m_TournamentScores[0].at(i + 1).fighters[eFighter1];

        m_TournamentScores[1].at(i + 1).fighters[eFighter2] =
            m_TournamentScores[0].at(i).fighters[eFighter2];

        m_TournamentScores[1].at(i).fighters[eFighter2] =
            m_TournamentScores[0].at(i + 1).fighters[eFighter2];

        ++i;
    }

    m_TournamentModelsPtrs[1]->SetDataChanged();
}

//=========================================================
TournamentModel* Controller::GetTournamentScoreModel(int which)
//=========================================================
{
    if (1 == which)
        return m_TournamentModelsPtrs[1];

    return m_TournamentModelsPtrs[0];
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
    if (is_option(Ipponboard::eOption_Use2013Rules)
            && is_golden_score())
    {
        *m_pTimeMain = m_pTimeMain->addSecs(1);
    }
    else
    {
        *m_pTimeMain = m_pTimeMain->addSecs(-1);

        const bool isTimeUp = QTime(0, 0, 0, 0).secsTo(*m_pTimeMain) <= 0;

        // correct time again
        const int secsTo(QTime(0, 0, 0, 0).secsTo(*m_pTimeMain));

        if (secsTo < 0 || *m_pTimeMain > m_roundTime)
            m_pTimeMain->setHMS(0, 0, 0, 0);

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

    if ((is_option(eOption_Use2013Rules)
         && (eOsaekomiVal2013_Yuko == secs
             || eOsaekomiVal2013_Wazaari == secs
             || eOsaekomiVal2013_Ippon == secs))
        || (eOsaekomiVal_Yuko == secs
            || eOsaekomiVal_Wazaari == secs
            || eOsaekomiVal_Ippon == secs))
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
    std::for_each(m_Views.begin(), m_Views.end(),
                  std::mem_fun(&IView::UpdateView));
}
