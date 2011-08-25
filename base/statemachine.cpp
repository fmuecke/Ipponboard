#include "statemachine.h"
#include "controller.h"
#include <QTimer>

using namespace Ipponboard;

//---------------------------------------------------------
void IpponboardSM_::add_point(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if( eOsaekomiVal_Yuko == evt.secs )
	{
		Score_(evt.tori).Add(ePoint_Yuko);
	}
	else if( eOsaekomiVal_Wazaari == evt.secs )
	{
		Score_(evt.tori).Remove(ePoint_Yuko);
		Score_(evt.tori).Add(ePoint_Wazaari);
	}
	else if( eOsaekomiVal_Ippon == evt.secs )
	{
		Score_(evt.tori).Remove(ePoint_Wazaari);
		Score_(evt.tori).Add(ePoint_Ippon);
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
	if( eOsaekomiVal_Ippon == evt.secs )
		return true;
	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_20s(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if( eOsaekomiVal_Wazaari == evt.secs )
		return true;
	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_20s_and_wazaari(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if( eOsaekomiVal_Wazaari == evt.secs &&
		0 != Score_(evt.tori).Wazaari() )
		return true;

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_20s_and_gs(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if( eOsaekomiVal_Wazaari == evt.secs &&
		0 != m_pCore->is_golden_score() )
		return true;

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_15s(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if( eOsaekomiVal_Yuko == evt.secs )
		return true;

	return false;
}

//---------------------------------------------------------
bool IpponboardSM_::has_15s_and_gs(HoldTimeEvent const& evt)
//---------------------------------------------------------
{
	if( eOsaekomiVal_Yuko == evt.secs &&
		0 != m_pCore->is_golden_score() )
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
	if( Score_(evt.tori).Shido() == 3 )
	{
		return true;
	}
	if( Score_(evt.tori).Shido() == 2 &&
		Score_(uke).Wazaari() == 1 )
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
