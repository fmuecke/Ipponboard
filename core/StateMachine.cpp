// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "StateMachine.h"

#include "Rules.h"

using namespace Ipponboard;
using Point = Score::Point;

void IpponboardSM::ToggleMainTimer()
{
    switch (m_state)
    {
    case eState_TimerStopped:
        startMainTimer();
        m_state = eState_TimerRunning;
        break;

    case eState_TimerRunning:
    case eState_Holding:
        stopMainTimer();
        m_state = eState_TimerStopped;
        break;

    default:
        break;
    }
}

void IpponboardSM::BeginHold(ContestSide who)
{
    m_holder = who;

    switch (m_state)
    {
    case eState_TimerStopped:
        m_core.start_timer(eTimer_Main);
        startHoldTimer();
        m_state = eState_Holding;
        break;

    case eState_TimerRunning:
        startHoldTimer();
        m_state = eState_Holding;
        break;

    default:
        break;
    }
}

void IpponboardSM::EndHold()
{
    if (m_state != eState_Holding)
    {
        return;
    }

    stopHoldTimer();
    m_state = mainTimeIsUp() ? eState_TimerStopped : eState_TimerRunning;
}

void IpponboardSM::SetHoldOwner(ContestSide who) { m_holder = who; }

void IpponboardSM::ClearHoldOwner() { m_holder = ContestSide::None; }

void IpponboardSM::AwardPoint(Score::Point point, ContestSide who)
{
    switch (point)
    {
    case Point::Yuko:
        if (m_state == eState_TimerStopped || m_state == eState_TimerRunning ||
            m_state == eState_Holding)
        {
            awardPoint(point, who);
            maybeStopForGoldenScore(point);
        }
        break;

    case Point::Wazaari:
        switch (m_state)
        {
        case eState_TimerStopped:
            if (canAddWazaari(who))
            {
                awardPoint(point, who);
            }
            break;

        case eState_TimerRunning:
            handleRunningWazaari(who);
            break;

        case eState_Holding:
            awardPoint(point, who);
            break;

        default:
            break;
        }
        maybeStopForGoldenScore(point);
        break;

    case Point::Ippon:
        if (m_state == eState_TimerStopped || m_state == eState_TimerRunning ||
            m_state == eState_Holding)
        {
            awardIppon(who);
            m_state = eState_TimerStopped;
        }
        break;

    case Point::Shido:
        AwardShido(who);
        break;

    case Point::Hansokumake:
        AwardHansokumake(who);
        break;

    default:
        break;
    }
}

void IpponboardSM::RevokePoint(Score::Point point, ContestSide who)
{
    switch (point)
    {
    case Point::Yuko:
    case Point::Wazaari:
        if (m_state == eState_TimerStopped || m_state == eState_TimerRunning ||
            m_state == eState_Holding)
        {
            revokePoint(point, who);
            maybeStopForGoldenScore(point);
        }
        break;

    case Point::Ippon:
        if (m_state == eState_TimerStopped)
        {
            revokePoint(point, who);
            maybeStopForGoldenScore(point);
        }
        break;

    default:
        break;
    }
}

void IpponboardSM::AwardShido(ContestSide who)
{
    switch (m_state)
    {
    case eState_TimerStopped:
    case eState_Holding:
        if (canTakeShido(who))
        {
            awardShido(who);
        }
        break;

    case eState_TimerRunning:
        handleRunningShido(who);
        break;

    default:
        break;
    }
}

void IpponboardSM::RevokeShido(ContestSide who)
{
    if (m_state == eState_TimerStopped || m_state == eState_TimerRunning ||
        m_state == eState_Holding)
    {
        revokeShidoOrHansokumake(who);
    }
}

void IpponboardSM::RevokeHansokumake(ContestSide who)
{
    if (m_state == eState_TimerStopped || m_state == eState_TimerRunning ||
        m_state == eState_Holding)
    {
        revokeShidoOrHansokumake(who);
        maybeStopForGoldenScoreAfterPenalty();
    }
}

void IpponboardSM::AwardHansokumake(ContestSide who)
{
    if (m_state == eState_TimerStopped || m_state == eState_TimerRunning ||
        m_state == eState_Holding)
    {
        awardHansokumake(who);
        m_state = eState_TimerStopped;
    }
}

void IpponboardSM::ResetContest()
{
    if (m_state == eState_TimerStopped || m_state == eState_TimerRunning ||
        m_state == eState_Holding)
    {
        resetContest();
        m_state = eState_TimerStopped;
        ClearHoldOwner();
    }
}

void IpponboardSM::FinishContest()
{
    switch (m_state)
    {
    case eState_TimerStopped:
        saveContest();
        break;

    case eState_TimerRunning:
    case eState_Holding:
        stopAllTimers();
        saveContest();
        m_state = eState_TimerStopped;
        break;

    default:
        break;
    }
}

void IpponboardSM::OnMainTimerElapsed()
{
    if (m_state == eState_TimerRunning)
    {
        stopMainTimer();
        m_state = eState_TimerStopped;
    }
}

void IpponboardSM::OnHoldTimerTick(int seconds)
{
    if (m_state != eState_Holding || m_holder == ContestSide::None)
    {
        return;
    }

    if (hasIpponTime(seconds))
    {
        applyHoldScore(seconds, m_holder);
        stopContest();
    }
    else if (hasAwaseteTime(seconds))
    {
        applyHoldScore(seconds, m_holder);
        stopContest();
    }
    else if (hasWazaariTime(seconds))
    {
        applyHoldScore(seconds, m_holder);
    }
    else if (hasYukoTime(seconds))
    {
        applyHoldScore(seconds, m_holder);
    }
}

void IpponboardSM::handleRunningWazaari(ContestSide who)
{
    if (isWazaariMatchPoint(who))
    {
        awardPoint(Point::Wazaari, who);
        stopContest();
    }
    else if (canAddWazaari(who))
    {
        awardPoint(Point::Wazaari, who);
    }
}

void IpponboardSM::handleRunningShido(ContestSide who)
{
    if (isShidoMatchPoint(who))
    {
        awardShido(who);
        stopContest();
    }
    else if (canTakeShido(who))
    {
        awardShido(who);
    }
}

void IpponboardSM::maybeStopForGoldenScore(Score::Point point)
{
    if (!m_core.is_golden_score())
    {
        return;
    }

    if (point == Point::Shido)
    {
        return;
    }

    if (m_state != eState_TimerRunning)
    {
        return;
    }

    if (compareScore() != 0)
    {
        stopMainTimer();
        m_state = eState_TimerStopped;
    }
}

void IpponboardSM::maybeStopForGoldenScoreAfterPenalty()
{
    if (!m_core.is_golden_score())
    {
        return;
    }

    if (m_state != eState_TimerRunning)
    {
        return;
    }

    if (compareScore() != 0)
    {
        stopMainTimer();
        m_state = eState_TimerStopped;
    }
}

void IpponboardSM::stopContest()
{
    stopAllTimers();
    m_state = eState_TimerStopped;
}

void IpponboardSM::resetContest() { m_core.reset_contest(); }

void IpponboardSM::saveContest() { m_core.save_contest(); }

void IpponboardSM::startMainTimer()
{
    m_core.reset_timer(eTimer_Hold);
    m_core.start_timer(eTimer_Main);
}

void IpponboardSM::stopMainTimer() { m_core.stop_timer(eTimer_Main); }

void IpponboardSM::startHoldTimer() { m_core.start_timer(eTimer_Hold); }

void IpponboardSM::stopHoldTimer() { m_core.stop_timer(eTimer_Hold); }

void IpponboardSM::stopAllTimers()
{
    m_core.stop_timer(eTimer_Hold);
    m_core.stop_timer(eTimer_Main);
}

void IpponboardSM::awardPoint(Score::Point point, ContestSide who) { score(who).Add(point); }

void IpponboardSM::revokePoint(Score::Point point, ContestSide who) { score(who).Remove(point); }

void IpponboardSM::awardIppon(ContestSide who)
{
    awardPoint(Point::Ippon, who);
    stopAllTimers();
}

void IpponboardSM::awardShido(ContestSide who)
{
    auto rules = m_core.GetRules();

    if (m_core.is_auto_adjust())
    {
        ContestSide uke = OpposingSide(who);
        auto maxShidoCount = rules->GetMaxShidoCount();

        if (maxShidoCount == score(who).Shido())
        {
            score(uke).Add(Point::Ippon);
        }
        else if (rules->IsOption_ShidoAddsPoint())
        {
            if (maxShidoCount > 2 && score(who).Shido() == 3)
            {
                score(uke).Remove(Point::Wazaari);
                score(uke).Add(Point::Ippon);
            }
            else if (maxShidoCount > 1 && score(who).Shido() == 2)
            {
                score(uke).Remove(Point::Yuko);
                score(uke).Add(Point::Wazaari);
            }
            else if (maxShidoCount > 0 && score(who).Shido() == 1)
            {
                score(uke).Add(Point::Yuko);
            }
        }
    }

    score(who).Add(Point::Shido);
}

void IpponboardSM::revokeShidoOrHansokumake(ContestSide who)
{
    ContestSide uke = OpposingSide(who);

    if (score(who).Hansokumake())
    {
        score(uke).Remove(Point::Ippon);
        score(who).Remove(Point::Hansokumake);
        return;
    }

    auto rules = m_core.GetRules();

    if (m_core.is_auto_adjust())
    {
        auto maxShidoCount = rules->GetMaxShidoCount();

        if (maxShidoCount + 1 == score(who).Shido())
        {
            score(uke).Remove(Point::Ippon);
        }
        else
        {
            if (maxShidoCount > 2 && score(who).Shido() == 4)
            {
                score(uke).Remove(Point::Ippon);
                score(uke).Add(Point::Wazaari);
            }
            else if (maxShidoCount > 1 && score(who).Shido() == 3)
            {
                score(uke).Remove(Point::Wazaari);
                score(uke).Add(Point::Yuko);
            }
            else if (maxShidoCount > 0 && score(who).Shido() == 2)
            {
                score(uke).Remove(Point::Yuko);
            }
        }
    }

    score(who).Remove(Point::Shido);
}

void IpponboardSM::awardHansokumake(ContestSide who)
{
    ContestSide uke = OpposingSide(who);
    score(uke).Add(Point::Ippon);
    score(who).Add(Point::Hansokumake);
    stopAllTimers();
}

void IpponboardSM::applyHoldScore(int seconds, ContestSide who)
{
    auto rules = m_core.GetRules();

    if (!m_core.is_auto_adjust())
    {
        return;
    }

    if (rules->GetOsaekomiValue(Point::Yuko) == seconds)
    {
        score(who).Add(Point::Yuko);
    }
    else if (rules->GetOsaekomiValue(Point::Wazaari) == seconds)
    {
        score(who).Remove(Point::Yuko);
        score(who).Add(Point::Wazaari);
    }
    else if (rules->GetOsaekomiValue(Point::Ippon) == seconds)
    {
        score(who).Remove(Point::Wazaari);
        score(who).Add(Point::Ippon);
    }
}

bool IpponboardSM::canAddWazaari(ContestSide who) const
{
    auto rules = m_core.GetRules();
    return rules->IsOption_AwaseteIppon() || score(who).Wazaari() < rules->GetMaxWazaariCount();
}

bool IpponboardSM::isWazaariMatchPoint(ContestSide who) const
{
    auto rules = m_core.GetRules();
    return rules->IsOption_AwaseteIppon() &&
           score(who).Wazaari() == rules->GetMaxWazaariCount() - 1;
}

bool IpponboardSM::canTakeShido(ContestSide who) const
{
    return score(who).Shido() <= m_core.GetRules()->GetMaxShidoCount();
}

bool IpponboardSM::isShidoMatchPoint(ContestSide who) const
{
    return score(who).Shido() == m_core.GetRules()->GetMaxShidoCount();
}

bool IpponboardSM::hasIpponTime(int seconds) const
{
    return m_core.GetRules()->GetOsaekomiValue(Point::Ippon) == seconds;
}

bool IpponboardSM::hasWazaariTime(int seconds) const
{
    return m_core.GetRules()->GetOsaekomiValue(Point::Wazaari) == seconds;
}

bool IpponboardSM::hasAwaseteTime(int seconds) const
{
    auto rules = m_core.GetRules();
    if (rules->IsOption_AwaseteIppon() && m_holder != ContestSide::None &&
        score(m_holder).Wazaari() != 0)
    {
        return rules->GetOsaekomiValue(Point::Wazaari) == seconds;
    }

    return false;
}

bool IpponboardSM::hasYukoTime(int seconds) const
{
    return m_core.GetRules()->GetOsaekomiValue(Point::Yuko) == seconds;
}

bool IpponboardSM::mainTimeIsUp() const { return m_core.get_time(eTimer_Main) == 0; }

int IpponboardSM::compareScore() const
{
    return m_core.GetRules()->CompareScore(
        score(ContestSide::SideA), score(ContestSide::SideB), m_core.is_golden_score());
}
