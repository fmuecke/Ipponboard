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

static EFighter GetUkeFromTori(EFighter tori)
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
	eState_SonoMama,
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
	eTimer_Hold,
};

enum EOsaekomiVal
{
	eOsaekomiVal_Yuko = 15,
	eOsaekomiVal_Wazaari = 20,
	eOsaekomiVal_Ippon = 25
};
}

#endif  // BASE__ENUMS_H_
