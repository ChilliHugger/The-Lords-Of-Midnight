//
//  common_steps.hpp
//  midnight
//
//  Created by Chris Wild on 03/01/2021.
//

#pragma once

#include "catch2/catch.hpp"
#include "../../src/system/moonring.h"
#include "../../src/tme/tme_interface.h"
#include "../../src/tme/baseinc/tme_internal.h"

#include <stdio.h>


using namespace tme;
extern mxengine* mx;

class TMEStep
{
public:
    static void NightFalls();
    static void UshgarakHasFallenInBattle();
    static void XajorkithHasFallenInBattle();
    static void MorkinIsDead();
    static void LuxorIsDead();
    static void IceCrownHasBeenDestroyed();
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
