//
//  common_steps.hpp
//  midnight
//
//  Created by Chris Wild on 03/01/2021.
//

#pragma once

#include "../tme_steps.h"

class LOMStep
{
public:
    static void UshgarakHasFallenInBattle();
    static void XajorkithHasFallenInBattle();
    static void MorkinIsDead();
    static void LuxorIsDead();
    static void IceCrownHasBeenDestroyed();
    static void GameWon();
    static void GameLost();
};

constexpr LPCSTR ch_farflame = "CH_FARFLAME";
constexpr LPCSTR ch_lorgrim = "CH_LORGRIM";
constexpr LPCSTR ch_fawkrin = "CH_FAWKRIN";

constexpr LPCSTR pl_lake_mirrow = "PL_LAKE_MIRROW";
constexpr LPCSTR pl_tower_of_doom = "PL_TOWER_OF_DOOM";

constexpr LPCSTR ob_icecrown = "OB_ICECROWN";
