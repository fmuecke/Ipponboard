// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "StateMachine.h"

#include "Controller.h"

#include <QTimer>

using namespace Ipponboard;
using Point = Score::Point;

void IpponboardSM::start() noexcept { set_state(eState_TimerStopped); }

EState IpponboardSM::state() const noexcept { return m_currentState; }

void IpponboardSM::set_state(EState newState) noexcept { m_currentState = newState; }

void IpponboardSM::process_event(Reset const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
    case eState_TimerRunning:
    case eState_Holding:
        reset(evt);
        set_state(eState_TimerStopped);
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(Finish const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
        save(evt);
        break;

    case eState_TimerRunning:
    case eState_Holding:
        stop_timer(evt);
        set_state(eState_TimerStopped);
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(Hajime_Mate const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
        start_timer(evt);
        set_state(eState_TimerRunning);
        break;

    case eState_TimerRunning:
    case eState_Holding:
        stop_timer(evt);
        set_state(eState_TimerStopped);
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(Osaekomi_Toketa const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
        yoshi(evt);
        set_state(eState_Holding);
        break;

    case eState_TimerRunning:
        start_timer(evt);
        set_state(eState_Holding);
        break;

    case eState_Holding:
        stop_timer(evt);
        set_state(time_is_up(evt) ? eState_TimerStopped : eState_TimerRunning);
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(Ippon const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
    case eState_TimerRunning:
    case eState_Holding:
        add_point(evt);
        set_state(eState_TimerStopped);
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(Wazaari const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
        if (can_add_wazaari(evt))
        {
            add_point(evt);
        }
        break;

    case eState_TimerRunning:
        if (wazaari_is_match_point(evt))
        {
            add_point_stop_timer(evt);
            set_state(eState_TimerStopped);
        }
        else if (can_add_wazaari(evt))
        {
            add_point(evt);
        }
        break;

    case eState_Holding:
        add_point(evt);
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(Yuko const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
    case eState_TimerRunning:
    case eState_Holding:
        add_point(evt);
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(Shido const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
        if (can_take_shido(evt))
        {
            add_point(evt);
        }
        break;

    case eState_TimerRunning:
        if (shido_is_match_point(evt))
        {
            add_point_stop_timer(evt);
            set_state(eState_TimerStopped);
        }
        else if (can_take_shido(evt))
        {
            add_point(evt);
        }
        break;

    case eState_Holding:
        if (can_take_shido(evt))
        {
            add_point(evt);
        }
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(Hansokumake const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
    case eState_TimerRunning:
    case eState_Holding:
        add_point(evt);
        set_state(eState_TimerStopped);
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(RevokeIppon const& evt)
{
    if (state() == eState_TimerStopped)
    {
        add_point(evt);
    }
}

void IpponboardSM::process_event(RevokeWazaari const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
    case eState_TimerRunning:
    case eState_Holding:
        add_point(evt);
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(RevokeYuko const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
    case eState_TimerRunning:
    case eState_Holding:
        add_point(evt);
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(RevokeShidoHM const& evt)
{
    switch (state())
    {
    case eState_TimerStopped:
    case eState_TimerRunning:
    case eState_Holding:
        add_point(evt);
        break;

    default:
        break;
    }
}

void IpponboardSM::process_event(HoldTimeEvent const& evt)
{
    if (state() != eState_Holding)
    {
        return;
    }

    // Preserve the Boost.MSM transition priority: the table was processed bottom up.
    if (has_IpponTime(evt))
    {
        add_point_stop_timer(evt);
        set_state(eState_TimerStopped);
    }
    else if (has_AwaseteTime(evt))
    {
        add_point_stop_timer(evt);
        set_state(eState_TimerStopped);
    }
    else if (has_WazaariTime(evt))
    {
        add_point(evt);
    }
    else if (has_YukoTime(evt))
    {
        add_point(evt);
    }
}

void IpponboardSM::process_event(TimeEndedEvent const& evt)
{
    if (state() == eState_TimerRunning)
    {
        stop_timer(evt);
        set_state(eState_TimerStopped);
    }
}

//---------------------------------------------------------
void IpponboardSM_::add_point(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
    auto pRules = m_pCore->GetRules();

    if (m_pCore->is_auto_adjust())
    {
        if (pRules->GetOsaekomiValue(Point::Yuko) == evt.secs)
        {
            Score_(evt.tori).Add(Point::Yuko);
        }
        else if (pRules->GetOsaekomiValue(Point::Wazaari) == evt.secs)
        {
            Score_(evt.tori).Remove(Point::Yuko);
            Score_(evt.tori).Add(Point::Wazaari);
        }
        else if (pRules->GetOsaekomiValue(Point::Ippon) == evt.secs)
        {
            Score_(evt.tori).Remove(Point::Wazaari);
            Score_(evt.tori).Add(Point::Ippon);
        }
    }
}

//---------------------------------------------------------
bool IpponboardSM_::can_add_wazaari(Wazaari const& evt)
//---------------------------------------------------------
{
    return m_pCore->GetRules()->IsOption_AwaseteIppon() ||
           Score_(evt.tori).Wazaari() < m_pCore->GetRules()->GetMaxWazaariCount();
}

//---------------------------------------------------------
bool IpponboardSM_::wazaari_is_match_point(Wazaari const& evt)
//---------------------------------------------------------
{
    return m_pCore->GetRules()->IsOption_AwaseteIppon() &&
           Score_(evt.tori).Wazaari() == m_pCore->GetRules()->GetMaxWazaariCount() - 1;
}

//---------------------------------------------------------
bool IpponboardSM_::has_max_wazaari(RevokeWazaari const& evt)
//---------------------------------------------------------
{
    return m_pCore->GetRules()->IsAwaseteIppon(Score_(evt.tori));
}

//---------------------------------------------------------
bool IpponboardSM_::has_IpponTime(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
    return m_pCore->GetRules()->GetOsaekomiValue(Point::Ippon) == evt.secs;
}

//---------------------------------------------------------
bool IpponboardSM_::has_WazaariTime(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
    return m_pCore->GetRules()->GetOsaekomiValue(Point::Wazaari) == evt.secs;
}

//---------------------------------------------------------
bool IpponboardSM_::has_AwaseteTime(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
    if (m_pCore->GetRules()->IsOption_AwaseteIppon() && 0 != Score_(evt.tori).Wazaari())
    {
        return m_pCore->GetRules()->GetOsaekomiValue(Point::Wazaari) == evt.secs;
    }

    return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_YukoTime(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
    return m_pCore->GetRules()->GetOsaekomiValue(Point::Yuko) == evt.secs;
}

//---------------------------------------------------------
bool IpponboardSM_::is_sonomama(Osaekomi_Toketa const&)
//---------------------------------------------------------
{
    return m_pCore->is_sonomama();
}

//---------------------------------------------------------
bool IpponboardSM_::shido_is_match_point(Shido const& evt)
//---------------------------------------------------------
{
    if (Score_(evt.tori).Shido() == m_pCore->GetRules()->GetMaxShidoCount())
    {
        return true;
    }

    return false;
}

//---------------------------------------------------------
bool IpponboardSM_::can_take_shido(Shido const& evt)
//---------------------------------------------------------
{
    return Score_(evt.tori).Shido() <= m_pCore->GetRules()->GetMaxShidoCount();
}

void IpponboardSM_::reset(Reset const&) { m_pCore->reset_fight(); }

void IpponboardSM_::save(Finish const&) { m_pCore->save_fight(); }

void IpponboardSM_::stop_timer(Osaekomi_Toketa const&) { m_pCore->stop_timer(eTimer_Hold); }

void IpponboardSM_::stop_timer(TimeEndedEvent const&) { m_pCore->stop_timer(eTimer_Main); }

void IpponboardSM_::stop_timer(Finish const&)
{
    // Finish will be created if current fight should be saved.
    m_pCore->stop_timer(eTimer_Hold);
    m_pCore->stop_timer(eTimer_Main);
    m_pCore->save_fight();
}

void IpponboardSM_::stop_timer(Hajime_Mate const&)
{
    m_pCore->stop_timer(ETimer(Hajime_Mate::type));
}

void IpponboardSM_::start_timer(Hajime_Mate const&)
{
    m_pCore->reset_timer(eTimer_Hold);
    m_pCore->start_timer(eTimer_Main);
}

void IpponboardSM_::start_timer(Osaekomi_Toketa const&) { m_pCore->start_timer(eTimer_Hold); }

void IpponboardSM_::add_point(PointEvent<ippon_type> const& evt)
{
    Score_(evt.tori).Add(Point::Ippon);
    m_pCore->stop_timer(eTimer_Main);
    m_pCore->stop_timer(eTimer_Hold);
}

void IpponboardSM_::add_point(PointEvent<shido_type> const& evt)
{
    auto pRules = m_pCore->GetRules();

    if (m_pCore->is_auto_adjust())
    {
        FighterEnum uke = GetUkeFromTori(evt.tori);

        auto maxShidoCount = pRules->GetMaxShidoCount();

        if (maxShidoCount == Score_(evt.tori).Shido())
        {
            Score_(uke).Add(Point::Ippon);
        }
        else if (pRules->IsOption_ShidoAddsPoint())
        {
            if (maxShidoCount > 2 && 3 == Score_(evt.tori).Shido())
            {
                Score_(uke).Remove(Point::Wazaari);
                Score_(uke).Add(Point::Ippon);
            }
            else if (maxShidoCount > 1 && 2 == Score_(evt.tori).Shido())
            {
                Score_(uke).Remove(Point::Yuko);
                Score_(uke).Add(Point::Wazaari);
            }
            else if (maxShidoCount > 0 && 1 == Score_(evt.tori).Shido())
            {
                Score_(uke).Add(Point::Yuko);
            }
        }
    }

    Score_(evt.tori).Add(Point::Shido);
}

void IpponboardSM_::add_point(PointEvent<revoke_shido_hm_type> const& evt)
{
    FighterEnum uke = GetUkeFromTori(evt.tori);

    if (Score_(evt.tori).Hansokumake())
    {
        Score_(uke).Remove(Point::Ippon);
        Score_(evt.tori).Remove(Point::Hansokumake);
    }
    else
    {
        auto pRules = m_pCore->GetRules();

        if (m_pCore->is_auto_adjust())
        {
            auto maxShidoCount = pRules->GetMaxShidoCount();

            if (maxShidoCount + 1 == Score_(evt.tori).Shido())
            {
                Score_(uke).Remove(Point::Ippon);
            }
            else
            {
                if (maxShidoCount > 2 && 4 == Score_(evt.tori).Shido())
                {
                    Score_(uke).Remove(Point::Ippon);
                    Score_(uke).Add(Point::Wazaari);
                }
                else if (maxShidoCount > 1 && 3 == Score_(evt.tori).Shido())
                {
                    Score_(uke).Remove(Point::Wazaari);
                    Score_(uke).Add(Point::Yuko);
                }
                else if (maxShidoCount > 0 && 2 == Score_(evt.tori).Shido())
                {
                    Score_(uke).Remove(Point::Yuko);
                }
            }
        }

        Score_(evt.tori).Remove(Point::Shido);
    }
}

void IpponboardSM_::add_point(PointEvent<hansokumake_type> const& evt)
{
    FighterEnum uke = GetUkeFromTori(evt.tori);
    Score_(uke).Add(Point::Ippon);
    Score_(evt.tori).Add(Point::Hansokumake);

    m_pCore->stop_timer(eTimer_Main);
    m_pCore->stop_timer(eTimer_Hold);
}

void IpponboardSM_::yoshi(Osaekomi_Toketa const&)
{
    m_pCore->start_timer(eTimer_Main);
    m_pCore->start_timer(eTimer_Hold);
}
