#ifndef BASE__ENUMS_H_
#define BASE__ENUMS_H_

//
// definitions
//
namespace Ipponboard
{
enum EPoint
{
	ePoint_NONE,
	ePoint_Yuko,
	ePoint_Wazaari,
	ePoint_Ippon,
	ePoint_Shido,
	ePoint_Hansokumake,
	ePoint_MAX
};

enum EFighter
{
	eFighterNobody = -1,
	eFighter1 = 0,
	eFighter2 = 1,
	eFighter_MAX
};

inline EFighter GetUkeFromTori(EFighter tori)
{
	return (tori == eFighter1) ? eFighter2 : eFighter1;
}


enum EAction
{
	eAction_NONE,
	eAction_Hajime_Mate,
	eAction_OsaeKomi_Toketa,
	eAction_SonoMama_Yoshi,
	eAction_Yuko,
	eAction_Wazaari,
	eAction_Ippon,
	eAction_Shido,
	eAction_Hansokumake,
	eAction_SetOsaekomi,
	eAction_ResetAll,
	eAction_ResetOsaeKomi,
	eAction_ResetMainTimer
};

enum EState
{
	eState_UNDEFINED = -1,
	eState_TimerStopped = 0,
	eState_TimerRunning,
	eState_Holding,
	eState_SonoMama
};

enum EHoldState
{
	eHoldState_off = 0,
	eHoldState_on,
	eHoldState_pause
};

enum ETimer
{
	eTimer_Main = 0,
	eTimer_Hold
};

enum EOsaekomiVal
{
	eOsaekomiVal_Yuko = 15,
	eOsaekomiVal_Wazaari = 20,
	eOsaekomiVal_Ippon = 25,

	// new values for 2013 IJF rules
	eOsaekomiVal2013_Yuko = 10,
	eOsaekomiVal2013_Wazaari = 15,
	eOsaekomiVal2013_Ippon = 20
};

enum EOption
{
    eOption_Invalid = -1,
    eOption_AutoIncrementPoints = 0,
	eOption_Use2013Rules = 1,
    eOption_AllSubscoresCount = 2,
	eOption_MAX
};

}

#endif  // BASE__ENUMS_H_
