//
//  battle_steps.h
//  midnight
//
//  Created by Chris Wild on 24/10/2023.
//

#pragma once

#include "../../steps/tme_steps.h"

struct BattleSteps
{
public:
    static void GivenLeaderWithFollowers();
    
    static void WhenLeaderLosesBattle();

    static void ThenTheLeaderShouldBeDisplaced();
    static void ThenTheLeaderShouldNotBeDisplaced();
    static void ThenTheLeaderGroupShouldBeDispanded();
    static void ThenTheLeaderGroupShouldNotBeDispanded();

    static void WhenFollowerLosesBattle();
    static void ThenTheFollowerShouldBeDisplaced();
    static void ThenTheFollowerShouldNotBeDisplaced();

};
