//
//  common_steps.hpp
//  midnight
//
//  Created by Chris Wild on 03/01/2021.
//

#pragma once

#include "../../steps/tme_steps.h"

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

    static constexpr LPCSTR ch_farflame = "CH_FARFLAME";
    static constexpr LPCSTR ch_lorgrim = "CH_LORGRIM";
    static constexpr LPCSTR ch_fawkrin = "CH_FAWKRIN";
    static constexpr LPCSTR ch_shadows = "CH_SHADOWS";
    static constexpr LPCSTR ch_dreams = "CH_DREAMS";
    static constexpr LPCSTR pl_lake_mirrow = "PL_LAKE_MIRROW";
    static constexpr LPCSTR pl_tower_of_doom = "PL_TOWER_OF_DOOM";
    static constexpr LPCSTR ob_icecrown = "OB_ICECROWN";
};
