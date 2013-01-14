#include "statemachine.h"
#include "controller.h"
#include <QTimer>

using namespace Ipponboard;

//---------------------------------------------------------
void IpponboardSM_::add_point(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
    if (m_pCore->is_option(Ipponboard::eOption_AutoIncrementPoints))
    {
        if (eOsaekomiVal_Yuko == evt.secs)
        {
            Score_(evt.tori).Add(ePoint_Yuko);
        }

        else if (eOsaekomiVal_Wazaari == evt.secs)
        {
            Score_(evt.tori).Remove(ePoint_Yuko);
            Score_(evt.tori).Add(ePoint_Wazaari);
        }

        else if (eOsaekomiVal_Ippon == evt.secs)
        {
            Score_(evt.tori).Remove(ePoint_Wazaari);
            Score_(evt.tori).Add(ePoint_Ippon);
        }
    }
}


//---------------------------------------------------------
bool IpponboardSM_::has_wazaari(Wazaari const& evt)
//---------------------------------------------------------
{
	if (0 != Score_(evt.tori).Wazaari())
	{
		return true;
	}

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_2wazaari(RevokeWazaari const& evt)
//---------------------------------------------------------
{
	if (2 == Score_(evt.tori).Wazaari())
	{
		return true;
	}

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_no_wazaari(Wazaari const& evt)
//---------------------------------------------------------
{
	return !has_wazaari(evt);
}

//---------------------------------------------------------
bool IpponboardSM_::has_25s(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if (eOsaekomiVal_Ippon == evt.secs)
		return true;

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_20s(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if (eOsaekomiVal_Wazaari == evt.secs)
		return true;

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_20s_and_wazaari(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if (eOsaekomiVal_Wazaari == evt.secs &&
			0 != Score_(evt.tori).Wazaari())
		return true;

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_20s_and_gs(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if (eOsaekomiVal_Wazaari == evt.secs &&
			0 != m_pCore->is_golden_score())
		return true;

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_15s(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if (eOsaekomiVal_Yuko == evt.secs)
		return true;

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_15s_and_gs(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if (eOsaekomiVal_Yuko == evt.secs &&
			0 != m_pCore->is_golden_score())
		return true;

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::is_sonomama(Osaekomi_Toketa const& /*evt*/)
//---------------------------------------------------------
{
	return m_pCore->is_sonomama();
}

//---------------------------------------------------------
bool IpponboardSM_::has_enough_shido(Shido const& evt)
//---------------------------------------------------------
{
	EFighter uke = GetUkeFromTori(evt.tori);

	if (Score_(evt.tori).Shido() == 3)
	{
		return true;
	}

	if (Score_(evt.tori).Shido() == 2 &&
			Score_(uke).Wazaari() == 1)
	{
		return true;
	}

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::can_take_shido(Shido const& evt)
//---------------------------------------------------------
{
	return !has_enough_shido(evt);
}

void IpponboardSM_::reset(Reset const& /*evt*/)
{
	m_pCore->reset_fight();
}

void IpponboardSM_::stop_timer(Osaekomi_Toketa const& /*evt*/)
{
	m_pCore->stop_timer(eTimer_Hold);
}

void IpponboardSM_::stop_timer(TimeEndedEvent const& /*evt*/)
{
	m_pCore->stop_timer(eTimer_Main);
}

void IpponboardSM_::stop_timer(Finish const& /*evt*/)
{
	// Finish will be created if current fight should be saved
	// --> stop timers
	m_pCore->stop_timer(eTimer_Hold);
	m_pCore->stop_timer(eTimer_Main);   // will save main time
}
void IpponboardSM_::stop_timer(Hajime_Mate const& /*evt*/)
{
	m_pCore->stop_timer(ETimer(Hajime_Mate::type));
}

void IpponboardSM_::start_timer(Hajime_Mate const& /*evt*/)
{
	m_pCore->reset_timer(eTimer_Hold);
	m_pCore->start_timer(eTimer_Main);
}

void IpponboardSM_::start_timer(Osaekomi_Toketa const& /*evt*/)
{
	//m_pCore->reset_timer( eTimer_Hold );
	m_pCore->start_timer(eTimer_Hold);
}

void IpponboardSM_::add_point(PointEvent<ippon_type> const& evt)
{
	Score_(evt.tori).Add(ePoint_Ippon);
	m_pCore->stop_timer(eTimer_Main);
	m_pCore->stop_timer(eTimer_Hold);
}

void IpponboardSM_::add_point(PointEvent<shido_type> const& evt)
{
    if (m_pCore->is_option(eOption_AutoIncrementPoints))
    {
        EFighter uke = GetUkeFromTori(evt.tori);

        if (3 == Score_(evt.tori).Shido())
        {
            Score_(uke).Remove(ePoint_Wazaari);
            Score_(uke).Add(ePoint_Ippon);
        }

        else if (2 == Score_(evt.tori).Shido())
        {
            Score_(uke).Remove(ePoint_Yuko);
            Score_(uke).Add(ePoint_Wazaari);
        }

        else if (1 == Score_(evt.tori).Shido())
        {
            Score_(uke).Add(ePoint_Yuko);
        }
    }

	Score_(evt.tori).Add(ePoint_Shido);
}

void IpponboardSM_::add_point(PointEvent<revoke_shido_hm_type> const& evt)
{
    EFighter uke = GetUkeFromTori(evt.tori);

    if (Score_(evt.tori).Hansokumake())
    {
        Score_(uke).Remove(ePoint_Ippon);
        Score_(evt.tori).Remove(ePoint_Hansokumake);
    }
    else
    {
        if (m_pCore->is_option(eOption_AutoIncrementPoints))
        {
            if (4 == Score_(evt.tori).Shido())
            {
                Score_(uke).Remove(ePoint_Ippon);
                Score_(uke).Add(ePoint_Wazaari);
            }
            else if (3 == Score_(evt.tori).Shido())
            {
                Score_(uke).Remove(ePoint_Wazaari);
                Score_(uke).Add(ePoint_Yuko);
            }
            else if (2 == Score_(evt.tori).Shido())
            {
                Score_(uke).Remove(ePoint_Yuko);
            }
        }
	}

    Score_(evt.tori).Remove(ePoint_Shido);
}

void IpponboardSM_::add_point(PointEvent<hansokumake_type> const& evt)
{
	EFighter uke = GetUkeFromTori(evt.tori);
	Score_(uke).Add(ePoint_Ippon);
	Score_(evt.tori).Add(ePoint_Hansokumake);

	m_pCore->stop_timer(eTimer_Main);
	m_pCore->stop_timer(eTimer_Hold);
}

void IpponboardSM_::yoshi(Osaekomi_Toketa const& /*evt*/)
{
	m_pCore->start_timer(eTimer_Main);
	m_pCore->start_timer(eTimer_Hold);
}
