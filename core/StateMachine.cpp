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

void IpponboardSM::BeginHold(FighterEnum who)
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

void IpponboardSM::SetHoldOwner(FighterEnum who) { m_holder = who; }

void IpponboardSM::ClearHoldOwner() { m_holder = FighterEnum::Nobody; }

void IpponboardSM::AwardPoint(Score::Point point, FighterEnum who)
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

void IpponboardSM::RevokePoint(Score::Point point, FighterEnum who)
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

void IpponboardSM::AwardShido(FighterEnum who)
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

void IpponboardSM::RevokeShido(FighterEnum who)
{
    if (m_state == eState_TimerStopped || m_state == eState_TimerRunning ||
        m_state == eState_Holding)
    {
        revokeShidoOrHansokumake(who);
    }
}

void IpponboardSM::RevokeHansokumake(FighterEnum who)
{
    if (m_state == eState_TimerStopped || m_state == eState_TimerRunning ||
        m_state == eState_Holding)
    {
        revokeShidoOrHansokumake(who);
        maybeStopForGoldenScoreAfterPenalty();
    }
}

void IpponboardSM::AwardHansokumake(FighterEnum who)
{
    if (m_state == eState_TimerStopped || m_state == eState_TimerRunning ||
        m_state == eState_Holding)
    {
        awardHansokumake(who);
        m_state = eState_TimerStopped;
    }
}

void IpponboardSM::ResetFight()
{
    if (m_state == eState_TimerStopped || m_state == eState_TimerRunning ||
        m_state == eState_Holding)
    {
        resetFight();
        m_state = eState_TimerStopped;
        ClearHoldOwner();
    }
}

void IpponboardSM::FinishFight()
{
    switch (m_state)
    {
    case eState_TimerStopped:
        saveFight();
        break;

    case eState_TimerRunning:
    case eState_Holding:
        stopAllTimers();
        saveFight();
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
    if (m_state != eState_Holding || m_holder == FighterEnum::Nobody)
    {
        return;
    }

    if (hasIpponTime(seconds))
    {
        applyHoldScore(seconds, m_holder);
        stopFight();
    }
    else if (hasAwaseteTime(seconds))
    {
        applyHoldScore(seconds, m_holder);
        stopFight();
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

void IpponboardSM::handleRunningWazaari(FighterEnum who)
{
    if (isWazaariMatchPoint(who))
    {
        awardPoint(Point::Wazaari, who);
        stopFight();
    }
    else if (canAddWazaari(who))
    {
        awardPoint(Point::Wazaari, who);
    }
}

void IpponboardSM::handleRunningShido(FighterEnum who)
{
    if (isShidoMatchPoint(who))
    {
        awardShido(who);
        stopFight();
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

void IpponboardSM::stopFight()
{
    stopAllTimers();
    m_state = eState_TimerStopped;
}

void IpponboardSM::resetFight() { m_core.reset_fight(); }

void IpponboardSM::saveFight() { m_core.save_fight(); }

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

void IpponboardSM::awardPoint(Score::Point point, FighterEnum who) { score(who).Add(point); }

void IpponboardSM::revokePoint(Score::Point point, FighterEnum who) { score(who).Remove(point); }

void IpponboardSM::awardIppon(FighterEnum who)
{
    awardPoint(Point::Ippon, who);
    stopAllTimers();
}

void IpponboardSM::awardShido(FighterEnum who)
{
    auto rules = m_core.GetRules();

    if (m_core.is_auto_adjust())
    {
        FighterEnum uke = GetUkeFromTori(who);
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

void IpponboardSM::revokeShidoOrHansokumake(FighterEnum who)
{
    FighterEnum uke = GetUkeFromTori(who);

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

void IpponboardSM::awardHansokumake(FighterEnum who)
{
    FighterEnum uke = GetUkeFromTori(who);
    score(uke).Add(Point::Ippon);
    score(who).Add(Point::Hansokumake);
    stopAllTimers();
}

void IpponboardSM::applyHoldScore(int seconds, FighterEnum who)
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

bool IpponboardSM::canAddWazaari(FighterEnum who) const
{
    auto rules = m_core.GetRules();
    return rules->IsOption_AwaseteIppon() || score(who).Wazaari() < rules->GetMaxWazaariCount();
}

bool IpponboardSM::isWazaariMatchPoint(FighterEnum who) const
{
    auto rules = m_core.GetRules();
    return rules->IsOption_AwaseteIppon() &&
           score(who).Wazaari() == rules->GetMaxWazaariCount() - 1;
}

bool IpponboardSM::canTakeShido(FighterEnum who) const
{
    return score(who).Shido() <= m_core.GetRules()->GetMaxShidoCount();
}

bool IpponboardSM::isShidoMatchPoint(FighterEnum who) const
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
    if (rules->IsOption_AwaseteIppon() && m_holder != FighterEnum::Nobody &&
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
        score(FighterEnum::First), score(FighterEnum::Second), m_core.is_golden_score());
}
