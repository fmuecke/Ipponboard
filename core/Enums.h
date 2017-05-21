#ifndef BASE__ENUMS_H_
#define BASE__ENUMS_H_

//
// definitions
//
namespace Ipponboard
{

enum  FighterEnum
{
	None = -1,
	First = 0,
	Second = 1,
	_MAX
};

inline FighterEnum GetUkeFromTori(FighterEnum tori)
{
	return (tori == FighterEnum::First) ? FighterEnum::Second : FighterEnum::First;
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

enum EOption
{
	eOption_Invalid = -1,
	//eOption_AutoAdjustPoints = 0,
	eOption_AllSubscoresCount = 2,
	eOption_MAX
};

}

#endif  // BASE__ENUMS_H_
