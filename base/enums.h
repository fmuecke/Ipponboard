#ifndef ENUMS_HEADER_INCLUDED
#define ENUMS_HEADER_INCLUDED

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
		eFighter_Nobody = -1,
		eFighter_Blue = 0,
		eFighter_White = 1,
		eFighter_MAX
	};

	static EFighter GetUkeFromTori( EFighter tori )
	{
		return (tori==eFighter_Blue)? eFighter_White : eFighter_Blue;
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
		eAction_Reset,
		eAction_ResetOsaeKomi
	};

	enum EState
	{
		eState_UNDEFINED = -1,
		eState_TimerStopped,
		eState_TimerRunning,
		eState_Holding,
		eState_SonoMama,
	};

	enum ETimer
	{
		eTimer_Main = 0,
		eTimer_Hold,
	};
}

#endif // ENUMS_HEADER_INCLUDED
