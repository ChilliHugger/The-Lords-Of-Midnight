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
using namespace std;
extern mxengine* mx;

class TMEStep
{
public:
    static void NewStory(mxdifficulty_t difficulty = DF_NORMAL);
    
    static void GivenNormalGame() { NewStory(); }
    static void GivenHardGame() { NewStory(DF_HARD); }
    
    static void NightFalls();
    static void CurrentLordIsDead();
    static void SetCurrentLord(LPCSTR name);
    static void GameLost();
    static void GameWon();
    
    static void LordCarryingObject(LPCSTR name, mxthing_t thing);
    static void LordCarryingObject(LPCSTR name, LPCSTR object);

    static void LordAtLocation(LPCSTR name, LPCSTR location);
    static void LordAtLocation(LPCSTR name, loc_t location);
    static void LordsAtSameLocation(LPCSTR name1, LPCSTR name2);
    
    static void LordIsNotRecruited(LPCSTR name);
    static void LordIsRecruited(LPCSTR name);
    static void LordPerformsApproach(LPCSTR name1, LPCSTR name2 = "");
    static void LordsIsLookingAt(LPCSTR name1, LPCSTR name2);
    static void LordMovesForward(LPCSTR name);
    static void LordIsDead(LPCSTR name);
    static void LordKilledByObject(LPCSTR name, LPCSTR killedby);

    // Grouping
    static void LordHasFollowers(LPCSTR lord, vector<string> names);

    
    //
    static bool LordHasBeenRecruited(LPCSTR name);
    
    //
    static constexpr LPCSTR ch_morkin = "CH_MORKIN";
    static constexpr LPCSTR ch_luxor = "CH_LUXOR";
    static constexpr LPCSTR ch_rorthron = "CH_RORTHRON";
#if defined(_LOM_)
    static constexpr LPCSTR ch_shadows = "CH_SHADOWS";
#endif

    static constexpr LPCSTR ob_wolves = "OB_WOLVES";
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

inline mxobject* GetObject(LPCSTR name)
{
    return GetEntity<mxobject>(name);
}

