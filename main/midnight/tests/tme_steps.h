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

#include <stdio.h>

using namespace tme;
extern mxengine* mx;

class TMEStep
{
public:
    static void NewStory();
    static void NightFalls();
    static void CurrentLordIsDead();
    static void GameLost();
    static void GameWon();
    
    static void LordCarryingObject(LPCSTR name, mxthing_t thing);
    static void LordAtLocation(LPCSTR name, LPCSTR location);

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


constexpr LPCSTR ch_morkin = "CH_MORKIN";
constexpr LPCSTR ch_luxor = "CH_LUXOR";
constexpr LPCSTR ch_rorthron = "CH_RORTHRON";
