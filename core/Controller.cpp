// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "Controller.h"

#include "CompetitionMode.h"
#include "CompetitionModel.h"
#include "Enums.h"
#include "Rules.h"
#include "Score.h"
#include "StateMachine.h"
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
const char* const emptyAthleteName = "--";

//=========================================================
Controller::Controller()
    : m_mode(),
      m_Competition(),
      m_CompetitionModels(),
      m_navigator(m_Competition),
      m_repository(m_Competition, m_CompetitionModels),
      m_pSM(nullptr),
      m_State(eState_TimerStopped),
      m_timerService(this),
      m_mainTime(0, 0, 0, 0),
      m_holdTime(0, 0, 0, 0),
      m_isSonoMama(false),
      m_roundTime(0, 0, 0, 0),
      m_options(0),
      m_labelHome("HOME"),
      m_labelGuest("GUEST")
//=========================================================
{
    m_pSM = std::make_unique<IpponboardSM>(*this);

    InitCompetition(m_mode);

    reset();

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
void Controller::InitCompetition(CompetitionMode const& mode)
//=========================================================
{
    m_CompetitionModels.clear();
    m_Competition.clear();

    m_mode = mode;
    m_rules = RulesFactory::Create(m_mode.rules);
    m_rules->SetCountSubscores(m_mode.IsOptionSet(CompetitionMode::str_Option_AllSubscoresCount));

    QStringList actualWeights = m_mode.weights.split(';');

    for (int round = 0; round < m_mode.nRounds; ++round)
    {
        PContestRound pRound(new ContestRound());

        for (int contestNo = 0; contestNo < m_mode.FightsPerRound(); ++contestNo)
        {
            QString weight =
                m_mode.weightsAreDoubled ? actualWeights[contestNo / 2] : actualWeights[contestNo];

            Contest contest;
            contest.weight = weight;
            contest.SetRoundTime(m_mode.GetFightDuration(weight));
            contest.rules = m_rules;
            contest.rules->SetCountSubscores(
                m_mode.IsOptionSet(CompetitionMode::str_Option_AllSubscoresCount));

            ContestAthlete emptyAthlete;
            emptyAthlete.name = emptyAthleteName;
            contest.GetAthlete(ContestSide::SideA) = emptyAthlete;
            contest.GetAthlete(ContestSide::SideB) = emptyAthlete;

            pRound->emplace_back(contest);
        }

        m_Competition.push_back(pRound);

        PCompetitionModel pModel(new CompetitionModel(pRound));
        pModel->SetNumRows(m_mode.FightsPerRound());

        m_CompetitionModels.push_back(pModel);
    }

    // set options AFTER configuring fights

    m_navigator.reset();

    // set time and update views
    SetRoundTime(QTime(0, 0, 0, 0).addSecs(m_mode.GetFightDuration(current_contest().weight)));
}

//=========================================================
int Controller::GetScore(ContestSide whos, Score::Point point) const
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
void Controller::DoAction(EAction action, ContestSide whos, bool doRevoke)
//=========================================================
{
    if (doRevoke)
    {
        switch (action)
        {
        case eAction_Yuko:
            m_pSM->RevokePoint(Score::Point::Yuko, whos);
            break;

        case eAction_Wazaari:
            m_pSM->RevokePoint(Score::Point::Wazaari, whos);
            break;

        case eAction_Ippon:
            m_pSM->RevokePoint(Score::Point::Ippon, whos);
            break;

        case eAction_Shido:
            m_pSM->RevokeShido(whos);
            break;

        case eAction_Hansokumake:
            m_pSM->RevokeHansokumake(whos);
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
            m_isSonoMama = (eState_Holding == m_State);
            m_pSM->ToggleMainTimer();
            break;

        case eAction_OsaeKomi_Toketa:
            if (eState_Holding == m_State)
                m_pSM->EndHold();
            else
                m_pSM->BeginHold(whos);
            m_isSonoMama = false;
            break;

        case eAction_Yuko:
            m_pSM->AwardPoint(Score::Point::Yuko, whos);
            break;

        case eAction_Wazaari:
            m_pSM->AwardPoint(Score::Point::Wazaari, whos);
            break;

        case eAction_Ippon:
            m_pSM->AwardPoint(Score::Point::Ippon, whos);
            break;

        case eAction_Shido:
            m_pSM->AwardShido(whos);
            break;

        case eAction_Hansokumake:
            m_pSM->AwardHansokumake(whos);
            break;

        case eAction_ResetAll:
            m_pSM->ResetContest();
            break;

        case eAction_SetOsaekomi:
            m_pSM->SetHoldOwner(whos);
            break;

        default:
            //Q_ASSERT(!"wrong action/action not handled!");
            break;
        }
    }

    // set current state
    m_State = m_pSM->CurrentState();

    update_views();
}

//=========================================================
ContestSide Controller::GetLeadingSide() const
//=========================================================
{
    ContestSide winner(ContestSide::None);

    switch (m_State)
    {
    case eState_TimerRunning:
    case eState_TimerStopped:
    {
        // determine who has the lead
        if (get_score(ContestSide::SideA).Wazaari() > get_score(ContestSide::SideB).Wazaari())
        {
            winner = ContestSide::SideA;
        }
        else if (get_score(ContestSide::SideA).Wazaari() < get_score(ContestSide::SideB).Wazaari())
        {
            winner = ContestSide::SideB;
        }
        else // GetScore_(eFirst).Wazaari() == GetScore_(eSecond).Wazaari()
        {
            if (get_score(ContestSide::SideA).Yuko() > get_score(ContestSide::SideB).Yuko())
            {
                winner = ContestSide::SideA;
            }
            else if (get_score(ContestSide::SideA).Yuko() < get_score(ContestSide::SideB).Yuko())
            {
                winner = ContestSide::SideB;
            }
            else // GetScore_(eFirst).Yuko() == GetScore_(eSecond).Yuko()
            {
                if (get_score(ContestSide::SideA).Shido() < get_score(ContestSide::SideB).Shido() &&
                    get_score(ContestSide::SideB).Shido() > 1)
                {
                    winner = ContestSide::SideA;
                }
                else if (get_score(ContestSide::SideA).Shido() >
                             get_score(ContestSide::SideB).Shido() &&
                         get_score(ContestSide::SideA).Shido() > 1)
                {
                    winner = ContestSide::SideB;
                }
                else
                {
                    // equal ==> golden score in single competition (Hantai is no more)
                }
            }
        }

        break;
    }

    case eState_SonoMama:
    case eState_Holding:
        winner = m_pSM->CurrentHoldSide();
        break;

    default:
        break;
    }

    return winner;
}

//=========================================================
Ipponboard::ContestSide Controller::GetCurrentHoldSide() const
//=========================================================
{
    return m_pSM->CurrentHoldSide();
}

//=========================================================
void Controller::reset_timers()
{
    m_State = eState_TimerStopped;

    m_timerService.stopTimer(eTimer_Main);
    reset_timer_value(eTimer_Main);

    m_timerService.stopTimer(eTimer_Hold);
    reset_timer_value(eTimer_Hold);
    m_pSM->ClearHoldOwner();

    m_isSonoMama = false;
}

void Controller::reset()
//=========================================================
{
    ClearFightsAndResetTimers();
    SetContest(0, 0, "-XX", tr("Side A"), "", tr("Side B"), "");

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
            m_pSM->ClearHoldOwner();
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
QString Controller::GetAthleteName(ContestSide who) const
//=========================================================
{
    Q_ASSERT(who == ContestSide::SideA || who == ContestSide::SideB);

    QString name = current_contest().GetAthlete(who).name;

    // shorten name
    const int pos = name.indexOf(' ');

    if (pos != -1)
    {
        name = name.left(1) + ". " + name.mid(pos, name.length() - pos);
    }

    return name;
}

//=========================================================
QString Controller::GetAthleteLastName(Ipponboard::ContestSide who) const
//=========================================================
{
    Q_ASSERT(who == ContestSide::SideA || who == ContestSide::SideB);

    QString name = current_contest().GetAthlete(who).name;

    // get last name
    const int pos = name.indexOf(' ');

    if (pos != -1)
    {
        name = name.mid(pos + 1, name.length() - pos);
    }

    return name;
}

//=========================================================
QString Controller::GetAthleteFirstName(Ipponboard::ContestSide who) const
//=========================================================
{
    Q_ASSERT(who == ContestSide::SideA || who == ContestSide::SideB);

    QString name = current_contest().GetAthlete(who).name;

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
QString Controller::GetAthleteClub(ContestSide who) const
//=========================================================
{
    Q_ASSERT(who == ContestSide::SideA || who == ContestSide::SideB);

    return current_contest().GetAthlete(who).club;
}

//=========================================================
QString const& Controller::GetWeight() const
//=========================================================
{
    return current_contest().weight;
}

//=========================================================
QString Controller::GetMessage() const
//=========================================================
{
    return m_Message;
}

//=========================================================
int Controller::GetTeamScore(Ipponboard::ContestSide who) const
//=========================================================
{
    int score(0);

    for (size_t round(0); round < m_Competition.size(); ++round)
    {
        for (size_t fight(0); fight < m_Competition[0]->size(); ++fight)
        {
            if (m_Competition[round]->at(fight).is_saved)
            {
                score += m_Competition[round]->at(fight).HasWon(who);
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
    m_mode.timeInSeconds = fightTimeSecs;
}

//=========================================================
QString Controller::GetTimeOverridesString() const
//=========================================================
{
    return m_roundTime.toString("m:ss"); //FIXME: use main time value instead
}

//=========================================================
//int Controller::GetRound() const
//=========================================================
//{
//    return m_currentRound * 10 + m_currentContest + 1;
//}

//=========================================================
int Controller::GetContestDuration(QString const& weight) const
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
    current_contest().SetGoldenScore(isGS);

    if (isGS && GetRules()->IsOption_OpenEndGoldenScore())
    {
        m_mainTime = QTime(0, 0, 0, 0).addSecs(current_contest().GetGoldenScoreTime());
    }
    else
    {
        m_mainTime = QTime(0, 0, 0, 0).addSecs(current_contest().GetRemainingTime());
    }

    update_views();
}

std::shared_ptr<AbstractRules> Controller::GetRules() const { return m_rules; }

void Controller::SetRules(std::shared_ptr<AbstractRules> rules)
{
    m_rules = rules;

    for (auto const& pRound : m_Competition)
    {
        for (auto& contest : *pRound)
        {
            contest.rules = rules;
        }
    }
}

bool Controller::IsAutoAdjustPoints() const { return m_isAutoAdjustPoints; }

void Controller::SetAutoAdjustPoints(bool isActive) { m_isAutoAdjustPoints = isActive; }

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
    if (m_matSignal.isEmpty())
    {
        return;
    }

    if (!m_gongEffect)
    {
        m_gongEffect = std::make_unique<QSoundEffect>();
    }

    auto* effect = m_gongEffect.get();
    effect->stop();

    qDebug() << "Playing sound " << m_matSignal;
    const auto source = QUrl("qrc:/sounds/" + m_matSignal);
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

void Controller::RegisterView(IGoldenScoreView* pView) { m_goldenScoreViews.insert(pView); }

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
void Controller::save_contest()
//=========================================================
{
    auto elapsed =
        is_golden_score() ? m_mainTime.secsTo(QTime(0, 0, 0, 0)) : m_mainTime.secsTo(m_roundTime);
    m_repository.saveContest(m_navigator.currentRound(), m_navigator.currentContest(), elapsed);
}

//=========================================================
void Controller::reset_contest()
//=========================================================
{
    m_timerService.stopTimer(eTimer_Hold);
    m_timerService.stopTimer(eTimer_Main);
    m_isSonoMama = false;

    m_repository.resetContestData(
        m_navigator.currentRound(), m_navigator.currentContest(), m_rules, m_mode);

    applyContestChange();
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
Score& Controller::get_score(ContestSide who)
//=========================================================
{
    Q_ASSERT(who == ContestSide::SideA || who == ContestSide::SideB);

    return current_contest().GetScore(who);
}

//=========================================================
Score const& Controller::get_score(ContestSide who) const
//=========================================================
{
    Q_ASSERT(who == ContestSide::SideA || who == ContestSide::SideB);

    return current_contest().GetScore(who);
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
    return current_contest().IsGoldenScore();
}

//=========================================================
void Controller::NextContest()
//=========================================================
{
    // move to Stopped state
    // (will stop all timers and thus save the current contest)
    m_pSM->FinishContest();

    const auto previousRound = m_navigator.currentRound();
    const auto previousContest = m_navigator.currentContest();

    m_navigator.nextContest();

    if (m_navigator.currentRound() != previousRound ||
        m_navigator.currentContest() != previousContest)
    {
        applyContestChange();
    }
}

//=========================================================
void Controller::PrevContest()
//=========================================================
{
    // move to Stopped state
    // (will stop all timers and thus save the current contest)
    m_pSM->FinishContest();

    const auto previousRound = m_navigator.currentRound();
    const auto previousContest = m_navigator.currentContest();

    m_navigator.prevContest();

    if (m_navigator.currentRound() != previousRound ||
        m_navigator.currentContest() != previousContest)
    {
        applyContestChange();
    }
}

//=========================================================
void Controller::SetCurrentContest(unsigned int index)
//=========================================================
{
    m_navigator.setCurrentContest(index);
    applyContestChange();
}

void Controller::applyContestChange()
{
    if (GetRoundCount() == 0 || GetContestCount() == 0)
    {
        update_views();
        return;
    }

    m_holdTime = QTime(0, 0, 0, 0);
    m_roundTime = QTime(0, 0, 0, 0).addSecs(m_mode.GetFightDuration(current_contest().weight));

    if (current_contest().IsGoldenScore())
    {
        m_mainTime = QTime(0, 0, 0, 0).addSecs(current_contest().GetGoldenScoreTime());
    }
    else
    {
        m_mainTime = QTime(0, 0, 0, 0).addSecs(current_contest().GetRemainingTime());
    }

    m_pSM->ClearHoldOwner();
    m_State = m_pSM->CurrentState();
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

    m_repository.clearAllFights(m_rules, m_mode, emptyAthleteName);

    m_navigator.reset();
    applyContestChange();
}

//=========================================================
void Controller::SetClub(Ipponboard::ContestSide who, const QString& clubName)
//=========================================================
{
    Q_ASSERT(who == Ipponboard::ContestSide::SideA || who == Ipponboard::ContestSide::SideB);

    for (unsigned int round(0); round < m_Competition.size(); ++round)
    {
        for (size_t fight(0); fight < m_Competition[0]->size(); ++fight)
        {
            m_Competition[round]->at(fight).GetAthlete(who).club = clubName;
        }
    }

    update_views();
}

//=========================================================
void Controller::SetContest(unsigned int roundIndex, unsigned int contestIndex,
                            const QString& weight, const QString& sideAName,
                            const QString& sideAClub, const QString& sideBName,
                            const QString& sideBClub, int yukoSideA, int wazaariSideA,
                            int ipponSideA, int shidoSideA, int hansokumakeSideA, int yukoSideB,
                            int wazaariSideB, int ipponSideB, int shidoSideB, int hansokumakeSideB)
//=========================================================
{
    m_repository.setContest(roundIndex,
                            contestIndex,
                            weight,
                            sideAName,
                            sideAClub,
                            sideBName,
                            sideBClub,
                            yukoSideA,
                            wazaariSideA,
                            ipponSideA,
                            shidoSideA,
                            hansokumakeSideA,
                            yukoSideB,
                            wazaariSideB,
                            ipponSideB,
                            shidoSideB,
                            hansokumakeSideB,
                            m_rules,
                            emptyAthleteName);
    update_views();
}

//=========================================================
void Controller::SetContest(unsigned int roundIndex, unsigned int contestIndex, Contest contest)
//=========================================================
{
    m_Competition[roundIndex]->at(contestIndex) = contest;

    update_views();
}

//=========================================================
Ipponboard::Contest const& Controller::GetContest(unsigned int roundIndex,
                                                  unsigned int contestIndex) const
//=========================================================
{
    return m_repository.contest(roundIndex, contestIndex);
}

//=========================================================
void Controller::SetAthleteName(Ipponboard::ContestSide whos, const QString& name)
//=========================================================
{
    current_contest().GetAthlete(whos).name = name;

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
    if (m_Competition.size() != 2)
    {
        throw std::exception(); // FIXME: use correct exception!
    }

    for (int contestIndex(0); contestIndex < GetContestCount() - 1; ++contestIndex)
    {
        m_Competition[1]->at(contestIndex).GetAthlete(ContestSide::SideA) =
            m_Competition[0]->at(contestIndex).GetAthlete(ContestSide::SideA);

        m_Competition[1]->at(contestIndex + 1).GetAthlete(ContestSide::SideA) =
            m_Competition[0]->at(contestIndex + 1).GetAthlete(ContestSide::SideA);

        m_Competition[1]->at(contestIndex + 1).GetAthlete(ContestSide::SideB) =
            m_Competition[0]->at(contestIndex).GetAthlete(ContestSide::SideB);

        m_Competition[1]->at(contestIndex).GetAthlete(ContestSide::SideB) =
            m_Competition[0]->at(contestIndex + 1).GetAthlete(ContestSide::SideB);

        ++contestIndex;
    }

    m_CompetitionModels[1]->SetDataChanged();
}

//=========================================================
PCompetitionModel Controller::GetCompetitionScoreModel(int which)
//=========================================================
{
    if ((size_t)which < m_CompetitionModels.size() && which > 0)
        return m_CompetitionModels[which];

    return m_CompetitionModels[0];
}

//=========================================================
void Controller::SetMatSignal(const QString& s)
//=========================================================
{
    m_matSignal = s;
}

//=========================================================
QString const& Controller::GetMatSignal() const
//=========================================================
{
    return m_matSignal;
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
                m_pSM->OnMainTimerElapsed();
                m_State = m_pSM->CurrentState();
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
        m_pSM->OnHoldTimerTick(secs);
        m_State = m_pSM->CurrentState();

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
