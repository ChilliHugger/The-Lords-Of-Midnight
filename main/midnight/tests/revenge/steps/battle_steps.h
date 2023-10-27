//
//  battle_steps.h
//  revenge
//
//  Created by Chris Wild on 24/10/2023.
//

#pragma once

#include "../../steps/tme_steps.h"

struct BattleSteps
{
public:
    static void GivenLeaderWithFollowers();
    
    static void WhenLeaderLosesTheirArmy();
    static void WhenLeaderKilledByLord();

    static void ThenTheLeaderShouldBeDisplaced();
    static void ThenTheLeaderShouldNotBeDisplaced();
    static void ThenTheLeaderGroupShouldBeDispanded();
    static void ThenTheLeaderGroupShouldNotBeDispanded();
    static void ThenTheLeaderShouldBeAvailableForBattle();
    static void ThenTheLeaderShouldNotBeAvailableForBattle();
    
    static void WhenFollowerLosesTheirArmy();
    static void WhenFollowerKilledByLord();
    static void ThenTheFollowerShouldBeDisplaced();
    static void ThenTheFollowerShouldNotBeDisplaced();
    static void ThenTheFollowerShouldBeAvailableForBattle();
    static void ThenTheFollowerShouldNotBeAvailableForBattle();
       
    static void LordBattleIsOver();
    static void LordBattleIsNotOver();
    static void LordShouldNotBeSelectedForBattle();
    static void LordShouldBeSelectedForBattle();

    static void LordFleesFromBattle();

};
