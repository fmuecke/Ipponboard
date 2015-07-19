#include "StateMachine.h"
#include "Controller.h"

#include <QTimer>

using namespace Ipponboard;
using Point = Score::Point;

//---------------------------------------------------------
void IpponboardSM_::add_point(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if (m_pCore->is_option(Ipponboard::eOption_AutoIncrementPoints))
	{
		if (m_pCore->is_option(Ipponboard::eOption_Use2013Rules))
		{
			if (OsaekomiValue::Yuko == evt.secs)
			{
				Score_(evt.tori).Add(Point::Yuko);
			}
			else if (OsaekomiValue::Wazaari == evt.secs)
			{
				Score_(evt.tori).Remove(Point::Yuko);
				Score_(evt.tori).Add(Point::Wazaari);
			}
			else if (OsaekomiValue::Ippon == evt.secs)
			{
				Score_(evt.tori).Remove(Point::Wazaari);
				Score_(evt.tori).Add(Point::Ippon);
			}
		}
		else
		{
			if (OsaekomiValue::YukoOld == evt.secs)
			{
				Score_(evt.tori).Add(Point::Yuko);
			}
			else if (OsaekomiValue::WazaariOld == evt.secs)
			{
				Score_(evt.tori).Remove(Point::Yuko);
				Score_(evt.tori).Add(Point::Wazaari);
			}
			else if (OsaekomiValue::IpponOld == evt.secs)
			{
				Score_(evt.tori).Remove(Point::Wazaari);
				Score_(evt.tori).Add(Point::Ippon);
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
		OsaekomiValue::Ippon :
		OsaekomiValue::IpponOld;

	return checkVal == evt.secs;
}

//---------------------------------------------------------
bool IpponboardSM_::has_WazaariTime(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	auto checkVal =
		m_pCore->is_option(Ipponboard::eOption_Use2013Rules) ?
		OsaekomiValue::Wazaari :
		OsaekomiValue::WazaariOld;

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
			OsaekomiValue::Wazaari :
			OsaekomiValue::WazaariOld;

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
		OsaekomiValue::Yuko :
		OsaekomiValue::YukoOld;

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
		FighterEnum uke = GetUkeFromTori(evt.tori);

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
	Score_(evt.tori).Add(Point::Ippon);
	m_pCore->stop_timer(eTimer_Main);
	m_pCore->stop_timer(eTimer_Hold);
}

void IpponboardSM_::add_point(PointEvent<shido_type> const& evt)
{
	if (m_pCore->is_option(eOption_AutoIncrementPoints))
	{
		FighterEnum uke = GetUkeFromTori(evt.tori);

		if (m_pCore->is_option(eOption_Use2013Rules))
		{
			if (3 == Score_(evt.tori).Shido())
			{
				Score_(uke).Add(Point::Ippon);
			}
		}
		else
		{
			if (3 == Score_(evt.tori).Shido())
			{
				Score_(uke).Remove(Point::Wazaari);
				Score_(uke).Add(Point::Ippon);
			}
			else if (2 == Score_(evt.tori).Shido())
			{
				Score_(uke).Remove(Point::Yuko);
				Score_(uke).Add(Point::Wazaari);
			}
			else if (1 == Score_(evt.tori).Shido())
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
		if (m_pCore->is_option(eOption_AutoIncrementPoints))
		{
			if (m_pCore->is_option(eOption_Use2013Rules))
			{
				if (4 == Score_(evt.tori).Shido())
				{
					Score_(uke).Remove(Point::Ippon);
				}
			}
			else
			{
				if (4 == Score_(evt.tori).Shido())
				{
					Score_(uke).Remove(Point::Ippon);
					Score_(uke).Add(Point::Wazaari);
				}
				else if (3 == Score_(evt.tori).Shido())
				{
					Score_(uke).Remove(Point::Wazaari);
					Score_(uke).Add(Point::Yuko);
				}
				else if (2 == Score_(evt.tori).Shido())
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

void IpponboardSM_::yoshi(Osaekomi_Toketa const& /*evt*/)
{
	m_pCore->start_timer(eTimer_Main);
	m_pCore->start_timer(eTimer_Hold);
}
