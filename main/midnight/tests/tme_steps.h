//
//  steps.h
//  midnight
//
//  Created by Chris Wild on 04/01/2021.
//
#pragma once


#include "catch2/catch.hpp"
#include "../src/system/moonring.h"
#include "../src/tme/tme_interface.h"
#include "../src/tme/baseinc/tme_internal.h"

#if defined(_LOM_)
    #include "midnight/common_steps.h"
#endif
#if defined(_DDR_)
    #include "revenge/common_steps.h"
#endif

#include <stdio.h>

using namespace tme;
extern mxengine* mx;

class TMEStep
{
public:
    static void NewStory();
    static void NightFalls();
    static void CurrentLordIsDead();
    static void SetCurrentLord(LPCSTR name);
    static void GameLost();
    static void GameWon();
    
    static void LordCarryingObject(LPCSTR name, mxthing_t thing);
    static void LordAtLocation(LPCSTR name, LPCSTR location);
    static void LordAtLocation(LPCSTR name, loc_t location);
    static void LordsAtSameLocation(LPCSTR name1, LPCSTR name2);
    
    static void LordIsNotRecruited(LPCSTR name);
    static void LordIsRecruited(LPCSTR name);
    static void LordPerformsApproach(LPCSTR name1, LPCSTR name2 = "");
    static void LordsIsLookingAt(LPCSTR name1, LPCSTR name2);
    static void LordMovesForward(LPCSTR name);
    static void LordIsDead(LPCSTR name);
    
    //
    static bool LordHasBeenRecruited(LPCSTR name);
    
    //
    static constexpr LPCSTR ch_morkin = "CH_MORKIN";
    static constexpr LPCSTR ch_luxor = "CH_LUXOR";
    static constexpr LPCSTR ch_rorthron = "CH_RORTHRON";
    static constexpr LPCSTR ch_shadows = "CH_SHADOWS";
};

template <typename T> T* GetEntity(LPCSTR name)
{
    return static_cast<T*>(tme::mx->EntityByName(name));
}

inline mxcharacter* GetCharacter(LPCSTR name)
{
    return GetEntity<mxcharacter>(name);
}

inline mxstronghold* GetStronghold(LPCSTR name)
{
    return GetEntity<mxstronghold>(name);
}


