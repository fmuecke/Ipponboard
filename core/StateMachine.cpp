#include "StateMachine.h"
#include "Controller.h"

#include <QTimer>

using namespace Ipponboard;

//---------------------------------------------------------
void IpponboardSM_::add_point(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if (m_pCore->is_option(Ipponboard::eOption_AutoIncrementPoints))
	{
		if (m_pCore->is_option(Ipponboard::eOption_Use2013Rules))
		{
			if (eOsaekomiVal2013_Yuko == evt.secs)
			{
				Score_(evt.tori).Add(ePoint_Yuko);
			}
			else if (eOsaekomiVal2013_Wazaari == evt.secs)
			{
				Score_(evt.tori).Remove(ePoint_Yuko);
				Score_(evt.tori).Add(ePoint_Wazaari);
			}
			else if (eOsaekomiVal2013_Ippon == evt.secs)
			{
				Score_(evt.tori).Remove(ePoint_Wazaari);
				Score_(evt.tori).Add(ePoint_Ippon);
			}
		}
		else
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
bool IpponboardSM_::has_IpponTime(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	auto checkVal =
		m_pCore->is_option(Ipponboard::eOption_Use2013Rules) ?
		eOsaekomiVal2013_Ippon :
		eOsaekomiVal_Ippon;

	return checkVal == evt.secs;
}

//---------------------------------------------------------
bool IpponboardSM_::has_WazaariTime(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	auto checkVal =
		m_pCore->is_option(Ipponboard::eOption_Use2013Rules) ?
		eOsaekomiVal2013_Wazaari :
		eOsaekomiVal_Wazaari;

	return checkVal == evt.secs;
}

//---------------------------------------------------------
bool IpponboardSM_::has_AwaseteTime(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if (0 != Score_(evt.tori).Wazaari())
	{
		auto checkVal =
			m_pCore->is_option(Ipponboard::eOption_Use2013Rules) ?
			eOsaekomiVal2013_Wazaari :
			eOsaekomiVal_Wazaari;

		return checkVal == evt.secs;
	}

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_YukoTime(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	auto checkVal =
		m_pCore->is_option(Ipponboard::eOption_Use2013Rules) ?
		eOsaekomiVal2013_Yuko :
		eOsaekomiVal_Yuko;

	return checkVal == evt.secs;
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


	if (Score_(evt.tori).Shido() == 3)
	{
		return true;
	}

	// Note: new 2013 IJF rule: no points for first three shido
	if (!m_pCore->is_option(eOption_Use2013Rules))
	{
		EFighter uke = GetUkeFromTori(evt.tori);

		if (Score_(evt.tori).Shido() == 2 &&
				Score_(uke).Wazaari() == 1)
		{
			return true;
		}
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

void IpponboardSM_::save(Finish const& /*evt*/)
{
	m_pCore->save_fight();
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
	m_pCore->save_fight();
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

		if (m_pCore->is_option(eOption_Use2013Rules))
		{
			if (3 == Score_(evt.tori).Shido())
			{
				Score_(uke).Add(ePoint_Ippon);
			}
		}
		else
		{
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
			if (m_pCore->is_option(eOption_Use2013Rules))
			{
				if (4 == Score_(evt.tori).Shido())
				{
					Score_(uke).Remove(ePoint_Ippon);
				}
			}
			else
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
