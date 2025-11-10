// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "Controller.h"

#include "Enums.h"
#include "Rules.h"
#include "Score.h"
#include "StateMachine.h"
#include "TournamentMode.h"
#include "TournamentModel.h"
#include "iGoldenScoreView.h"
#include "iView.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QSoundEffect>
#include <QUrl>

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
    : m_mode(),
      m_Tournament(),
      m_TournamentModels(),
      m_navigator(m_Tournament),
      m_repository(m_Tournament, m_TournamentModels),
      m_pSM(nullptr),
      m_State(eState_TimerStopped),
      m_timerService(this),
      m_mainTime(0, 0, 0, 0),
      m_holdTime(0, 0, 0, 0),
      m_Tori(FighterEnum::Nobody),
      m_isSonoMama(false),
      m_roundTime(0, 0, 0, 0),
      m_options(0),
      m_labelHome("HOME"),
      m_labelGuest("GUEST")
//=========================================================
{
    m_pSM = std::make_unique<IpponboardSM>();
    m_pSM->SetCore(this);

    InitTournament(m_mode);

    reset();
    m_pSM->start();

    connect(&m_timerService, &TimerService::mainTimeout, this, &Controller::update_main_time);
    connect(&m_timerService, &TimerService::holdTimeout, this, &Controller::update_hold_time);

    if (auto* app = QCoreApplication::instance())
    {
        connect(app,
                &QCoreApplication::aboutToQuit,
                this,
                [this]()
                {
                    if (m_gongEffect)
                    {
                        m_gongEffect->stop();
                        m_gongEffect->setSource(QUrl());
                        m_gongEffect.reset();
                    }
                });
    }
}

//=========================================================
Controller::~Controller()
//=========================================================
{
    if (m_gongEffect)
    {
        m_gongEffect->stop();
        m_gongEffect->setSource(QUrl());
    }
    m_views.clear();
    m_goldenScoreViews.clear();
    //delete m_pSM; --> deleted via smart ptr
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
            QString weight =
                m_mode.weightsAreDoubled ? actualWeights[fightNo / 2] : actualWeights[fightNo];

            Fight fight;
            fight.weight = weight;
            fight.SetRoundTime(m_mode.GetFightDuration(weight));
            fight.rules = m_rules;
            fight.rules->SetCountSubscores(
                m_mode.IsOptionSet(TournamentMode::str_Option_AllSubscoresCount));

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

    m_navigator.reset();

    // set time and update views
    SetRoundTime(QTime(0, 0, 0, 0).addSecs(m_mode.GetFightDuration(current_fight().weight)));
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
        value = /*score.IsAwaseteIppon() ? 0 : */ score.Wazaari();
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
    if (current_fight().IsGoldenScore() && action != eAction_Hajime_Mate &&
        action != eAction_Shido && eState_TimerStopped != EState(m_pSM->current_state()[0]))
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
        else // GetScore_(eFirst).Wazaari() == GetScore_(eSecond).Wazaari()
        {
            if (get_score(FighterEnum::First).Yuko() > get_score(FighterEnum::Second).Yuko())
            {
                winner = FighterEnum::First;
            }
            else if (get_score(FighterEnum::First).Yuko() < get_score(FighterEnum::Second).Yuko())
            {
                winner = FighterEnum::Second;
            }
            else // GetScore_(eFirst).Yuko() == GetScore_(eSecond).Yuko()
            {
                if (get_score(FighterEnum::First).Shido() <
                        get_score(FighterEnum::Second).Shido() &&
                    get_score(FighterEnum::Second).Shido() > 1) // no "koka"!
                {
                    winner = FighterEnum::First;
                }
                else if (get_score(FighterEnum::First).Shido() >
                             get_score(FighterEnum::Second).Shido() &&
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

    m_timerService.stopTimer(eTimer_Main);
    reset_timer_value(eTimer_Main);

    m_timerService.stopTimer(eTimer_Hold);
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
        m_mainTime = m_roundTime; // FIXME: set to 0 for golden score?
    }
    else if (eTimer_Hold == timer)
    {
        m_holdTime.setHMS(0, 0, 0, 0);

        if (!m_timerService.isActive(eTimer_Hold))
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
        ret = m_mainTime.toString("m:ss");
        ret = ret.isEmpty() ? "0:00" : ret;
        break;

    case eTimer_Hold:
        ret = m_holdTime.toString("ss");
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

    QString name = current_fight().GetFighter(who).name;

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

    QString name = current_fight().GetFighter(who).name;

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

    QString name = current_fight().GetFighter(who).name;

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

    return current_fight().GetFighter(who).club;
}

//=========================================================
QString const& Controller::GetWeight() const
//=========================================================
{
    return current_fight().weight;
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
    if (eState_TimerStopped == m_State || eState_SonoMama == m_State)
    {
        if (eTimer_Main == timer)
        {
            QTime newTime = QTime::fromString(value, "m:ss");

            // don't allow invalid times like "-1:22"
            if (newTime.isValid())
            {
                m_mainTime = newTime;
            }
        }
        else if (eTimer_Hold == timer)
        {
            m_holdTime.setHMS(0, 0, value.toInt());
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
    m_mainTime = time;

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
    return m_roundTime.toString("m:ss"); //FIXME: use main time value instead
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
        m_mainTime = QTime(0, 0, 0, 0).addSecs(current_fight().GetGoldenScoreTime());
    }
    else
    {
        m_mainTime = QTime(0, 0, 0, 0).addSecs(current_fight().GetRemainingTime());
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

    for (auto const& pRound : m_Tournament)
    {
        for (auto& fight : *pRound)
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
    if (m_gongFile.isEmpty())
    {
        return;
    }

    if (!m_gongEffect)
    {
        m_gongEffect = std::make_unique<QSoundEffect>();
    }

    auto* effect = m_gongEffect.get();
    effect->stop();
    const auto source = QUrl::fromLocalFile(m_gongFile);
    if (effect->source() != source)
    {
        effect->setSource(source);
    }
    effect->setLoopCount(1);
    effect->setVolume(1.0f);
    effect->play();
}

//=========================================================
void Controller::AdvanceTimerTicks(Ipponboard::ETimer timer, int ticks)
//=========================================================
{
    m_timerService.advanceTicks(timer, ticks);
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
    m_timerService.startTimer(t);
}

//=========================================================
void Controller::stop_timer(ETimer t)
//=========================================================
{
    m_timerService.stopTimer(eTimer_Hold);

    if (eTimer_Main == t)
    {
        m_timerService.stopTimer(eTimer_Main);
    }
}

//=========================================================
void Controller::save_fight()
//=========================================================
{
    auto elapsed =
        is_golden_score() ? m_mainTime.secsTo(QTime(0, 0, 0, 0)) : m_mainTime.secsTo(m_roundTime);
    m_repository.saveFight(m_navigator.currentRound(), m_navigator.currentFight(), elapsed);
}

//=========================================================
void Controller::reset_fight()
//=========================================================
{
    m_timerService.stopTimer(eTimer_Hold);
    m_timerService.stopTimer(eTimer_Main);
    m_Tori = FighterEnum::Nobody;
    m_isSonoMama = false;

    m_repository.resetFightData(
        m_navigator.currentRound(), m_navigator.currentFight(), m_rules, m_mode);

    applyFightChange();
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

    return current_fight().GetScore(who);
}

//=========================================================
Score const& Controller::get_score(FighterEnum who) const
//=========================================================
{
    Q_ASSERT(who == FighterEnum::First || who == FighterEnum::Second);

    return current_fight().GetScore(who);
}

//=========================================================
int Controller::get_time(ETimer t) const
//=========================================================
{
    if (eTimer_Hold == t)
    {
        return -m_holdTime.secsTo(QTime(0, 0, 0, 0));
    }
    else
    {
        return m_mainTime.secsTo(QTime(0, 0, 0, 0));
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

    const auto previousRound = m_navigator.currentRound();
    const auto previousFight = m_navigator.currentFight();

    m_navigator.nextFight();

    if (m_navigator.currentRound() != previousRound || m_navigator.currentFight() != previousFight)
    {
        applyFightChange();
    }
}

//=========================================================
void Controller::PrevFight()
//=========================================================
{
    // move to Stopped state
    // (will stop all timers and thus save the current fight)
    m_pSM->process_event(IpponboardSM_::Finish());

    const auto previousRound = m_navigator.currentRound();
    const auto previousFight = m_navigator.currentFight();

    m_navigator.prevFight();

    if (m_navigator.currentRound() != previousRound || m_navigator.currentFight() != previousFight)
    {
        applyFightChange();
    }
}

//=========================================================
void Controller::SetCurrentFight(unsigned int index)
//=========================================================
{
    m_navigator.setCurrentFight(index);
    applyFightChange();
}

void Controller::applyFightChange()
{
    if (GetRoundCount() == 0 || GetFightCount() == 0)
    {
        update_views();
        return;
    }

    m_holdTime = QTime(0, 0, 0, 0);
    m_roundTime = QTime(0, 0, 0, 0).addSecs(m_mode.GetFightDuration(current_fight().weight));

    if (current_fight().IsGoldenScore())
    {
        m_mainTime = QTime(0, 0, 0, 0).addSecs(current_fight().GetGoldenScoreTime());
    }
    else
    {
        m_mainTime = QTime(0, 0, 0, 0).addSecs(current_fight().GetRemainingTime());
    }

    m_State = EState(m_pSM->current_state()[0]);
    Q_ASSERT(eState_TimerStopped == m_State);

    update_views();
}

//=========================================================
void Controller::SetCurrentRound(unsigned int index)
//=========================================================
{
    auto previousRound = m_navigator.currentRound();
    m_navigator.setCurrentRound(index);
    if (m_navigator.currentRound() != previousRound)
    {
        update_views();
    }
}

//=========================================================
void Controller::ClearFightsAndResetTimers()
//=========================================================
{
    reset_timers();

    m_repository.clearAllFights(m_rules, m_mode, emptyFighterName);

    m_navigator.reset();
    applyFightChange();
}

//=========================================================
void Controller::SetClub(Ipponboard::FighterEnum who, const QString& clubName)
//=========================================================
{
    Q_ASSERT(who == Ipponboard::FighterEnum::First || who == Ipponboard::FighterEnum::Second);

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
void Controller::SetFight(unsigned int round_index, unsigned int fight_index, const QString& weight,
                          const QString& first_player_name, const QString& first_player_club,
                          const QString& second_player_name, const QString& second_player_club,
                          int yuko1, int wazaari1, int ippon1, int shido1, int hansokumake1,
                          int yuko2, int wazaari2, int ippon2, int shido2, int hansokumake2)
//=========================================================
{
    m_repository.setFight(round_index,
                          fight_index,
                          weight,
                          first_player_name,
                          first_player_club,
                          second_player_name,
                          second_player_club,
                          yuko1,
                          wazaari1,
                          ippon1,
                          shido1,
                          hansokumake1,
                          yuko2,
                          wazaari2,
                          ippon2,
                          shido2,
                          hansokumake2,
                          m_rules,
                          emptyFighterName);
    update_views();
}

//=========================================================
void Controller::SetFight(
	unsigned int round_index, unsigned int fight_index,
	Fight fight)
//=========================================================
{
	m_Tournament[round_index]->at(fight_index) = fight;

	update_views();
}

//=========================================================
Ipponboard::Fight const& Controller::GetFight(unsigned int round_index,
                                              unsigned int fight_index) const
//=========================================================
{
    return m_repository.fight(round_index, fight_index);
}

//=========================================================
void Controller::SetFighterName(Ipponboard::FighterEnum whos, const QString& name)
//=========================================================
{
    current_fight().fighters[whos].name = name;

    update_views();
}

//=========================================================
void Controller::SetWeights(QStringList const& weights)
//=========================================================
{
    m_repository.setWeights(weights, m_mode);
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
        m_mainTime = m_mainTime.addSecs(1);
    }
    else
    {
        m_mainTime = m_mainTime.addSecs(-1);

        const bool isTimeUp = QTime(0, 0, 0, 0).secsTo(m_mainTime) <= 0;

        // correct time again
        const int secsTo(QTime(0, 0, 0, 0).secsTo(m_mainTime));

        if (secsTo < 0 || m_mainTime > m_roundTime)
            m_mainTime.setHMS(0, 0, 0, 0);

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

    m_holdTime = m_holdTime.addSecs(1);
    const int secs = m_holdTime.second();

    if (secs > 0 && (m_rules->GetOsaekomiValue(Score::Point::Yuko) == secs ||
                     m_rules->GetOsaekomiValue(Score::Point::Wazaari) == secs ||
                     m_rules->GetOsaekomiValue(Score::Point::Ippon) == secs))
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
    for (auto const& pView : m_views)
    {
        pView->UpdateView();
    }

    for (auto const& pView : m_goldenScoreViews)
    {
        pView->UpdateGoldenScoreView();
    }
}
