// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__ENUMS_H_
#define BASE__ENUMS_H_

//
// definitions
//
namespace Ipponboard
{

enum class ContestSide
{
    None = -1,
    SideA = 0,
    SideB = 1,
    Count
};

constexpr bool IsContestSide(ContestSide side)
{
    return side == ContestSide::SideA || side == ContestSide::SideB;
}

constexpr int ToIndex(ContestSide side) { return static_cast<int>(side); }

inline ContestSide OpposingSide(ContestSide side)
{
    if (side == ContestSide::SideA)
    {
        return ContestSide::SideB;
    }

    if (side == ContestSide::SideB)
    {
        return ContestSide::SideA;
    }

    return side;
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

} // namespace Ipponboard

#endif // BASE__ENUMS_H_
