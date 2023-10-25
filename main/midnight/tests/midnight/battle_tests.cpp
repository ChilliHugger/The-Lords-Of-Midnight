//
//  battle_tests.cpp
//  revenge
//
//  Created by Chris Wild on 02/09/2023.
//

#include "battle_steps.h"

// GIVEN a hard game
// AND a lord has followers
// WHEN the lord loses a battle
// THEN the leader should be displaced
// AND the group should be dispanded
// AND the leader should still be available for battle

SCENARIO("In hard mode when leader loses a battle")
{
    GIVEN("A hard game")
    {
        TMEStep::GivenHardGame();
        
        AND_GIVEN("that the lord has followers")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the lord loses a battle")
            {
                BattleSteps::WhenLeaderLosesBattle();
                
                THEN("the lord should be displaced")
                {
                    BattleSteps::ThenTheLeaderShouldBeDisplaced();
                
                    AND_THEN("the group should be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldBeDispanded();
                    }
                }
            }
        }
    }
}

// GIVEN a normal game
// AND a lord has followers
// WHEN the lord loses a battle
// THEN the leader should not be displaced
// AND the group should not be dispanded

SCENARIO("In normal mode when leader loses a balle")
{
    GIVEN("A normal game")
    {
        TMEStep::GivenNormalGame();
        
        AND_GIVEN("that lord has followers")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the lord loses a battle")
            {
                BattleSteps::WhenLeaderLosesBattle();
                
                THEN("the lord should not be displaced")
                {
                    BattleSteps::ThenTheLeaderShouldNotBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                    }
                }
            }
        }
    }
}

// GIVEN a hard game
// AND the lord is a follower
// WHEN the lord loses a battle
// THEN the follower should be displaced
// AND the group should not be dispanded

SCENARIO("In hard mode when follower loses a battle")
{
    GIVEN("A hard game")
    {
        TMEStep::GivenHardGame();
        
        AND_GIVEN("that the lord is a follower")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the follower loses a battle")
            {
                BattleSteps::WhenFollowerLosesBattle();
                
                THEN("the lord should be displaced")
                {
                    BattleSteps::ThenTheFollowerShouldBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                    }
                }
            }
        }
    }
}

// GIVEN a normal game
// AND the lord is a follower
// WHEN the lord loses a battle
// THEN the follower should not be displaced
// AND the group should not be dispanded
// AND the follower should no longer be available for battle
SCENARIO("In normal mode when follower loses a battle")
{
    GIVEN("A normal game")
    {
        TMEStep::GivenNormalGame();
        
        AND_GIVEN("that the lord is a follower")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the follower loses a battle")
            {
                BattleSteps::WhenFollowerLosesBattle();
                
                THEN("the lord should not be displaced")
                {
                    BattleSteps::ThenTheFollowerShouldNotBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                    }
                }
            }
        }
    }
}

