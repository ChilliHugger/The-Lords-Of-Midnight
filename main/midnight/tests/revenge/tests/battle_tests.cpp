//
//  battle_tests.cpp
//  revenge
//
//  Created by Chris Wild on 02/09/2023.
//

#include "../steps/battle_steps.h"

// GIVEN a hard game
// AND a lord has followers
// WHEN the lord loses their army
// THEN the leader should be displaced
// AND the group should be dispanded
// AND the leader should still be available for battle

SCENARIO("In hard mode when leader loses their army")
{
    GIVEN("A hard game")
    {
        TMEStep::GivenHardGame();
        
        AND_GIVEN("that the lord has followers")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the lord loses their army")
            {
                BattleSteps::WhenLeaderLosesTheirArmy();
                
                THEN("the lord should be displaced")
                {
                    BattleSteps::ThenTheLeaderShouldBeDisplaced();
                
                    AND_THEN("the group should be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldBeDispanded();
                        
                        AND_THEN("the lord should be available for battle")
                        {
                            BattleSteps::ThenTheLeaderShouldBeAvailableForBattle();
                        }
                    }
                }
            }
        }
    }
}

// GIVEN a normal game
// WHEN the lord loses their army
// THEN the leader should not be displaced
// AND the group should not be dispanded
// AND the leader should no longer be available for battle
SCENARIO("In normal mode when leader loses their army")
{
    GIVEN("A normal game")
    {
        TMEStep::GivenNormalGame();
        
        AND_GIVEN("that lord has followers")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the lord loses their army")
            {
                BattleSteps::WhenLeaderLosesTheirArmy();
                
                THEN("the lord should not be displaced")
                {
                    BattleSteps::ThenTheLeaderShouldNotBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                        
                        AND_THEN("the lord should not be available for battle")
                        {
                            BattleSteps::ThenTheLeaderShouldNotBeAvailableForBattle();
                        }
                    }
                }
            }
        }
    }
}

// GIVEN a hard game
// WHEN a follower loses their army
// THEN the follower should be displaced
// AND the group should not be dispanded
// AND the follower should still be available for battle

SCENARIO("In hard mode when follower loses their army")
{
    GIVEN("A hard game")
    {
        TMEStep::GivenHardGame();
        
        AND_GIVEN("that the lord is a follower")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the follower loses their army")
            {
                BattleSteps::WhenFollowerLosesTheirArmy();
                
                THEN("the lord should be displaced")
                {
                    BattleSteps::ThenTheFollowerShouldBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                        
                        AND_THEN("the lord should be available for battle")
                        {
                            BattleSteps::ThenTheFollowerShouldBeAvailableForBattle();
                        }
                    }
                }
            }
        }
    }
}

// GIVEN a normal game
// WHEN the lord loses their army
// THEN the follower should not be displaced
// AND the group should not be dispanded
// AND the follower should no longer be available for battle
SCENARIO("In normal mode when follower loses their army")
{
    GIVEN("A normal game")
    {
        TMEStep::GivenNormalGame();
        
        AND_GIVEN("that the lord is a follower")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("the lord loses their army")
            {
                BattleSteps::WhenFollowerLosesTheirArmy();
                
                THEN("the lord should not be displaced")
                {
                    BattleSteps::ThenTheFollowerShouldNotBeDisplaced();
                
                    AND_THEN("the group should not be dispanded")
                    {
                        BattleSteps::ThenTheLeaderGroupShouldNotBeDispanded();
                        
                        AND_THEN("the lord should not be available for battle")
                        {
                            BattleSteps::ThenTheFollowerShouldNotBeAvailableForBattle();
                        }
                    }
                }
            }
        }
    }
}


// GIVEN a character's battle is over
// THEN the character should not be included in any more battles
SCENARIO("A character whose battle is over should not be selected for battle")
{
    TMEStep::NewStory();
    
    GIVEN("A characters battle is over")
    {
        BattleSteps::LordBattleIsOver();
        
        THEN("they should not be selected for battle")
        {
            BattleSteps::LordShouldNotBeSelectedForBattle();
        }
    }
}

// GIVEN a character's battle is not over
// THEN the character should be included in any more battles
SCENARIO("A character whose battle is not over should be selected for battle")
{
    TMEStep::NewStory();
    
    GIVEN("A characters battle is not over")
    {
        BattleSteps::LordBattleIsNotOver();
        
        THEN("they should be selected for battle")
        {
            BattleSteps::LordShouldBeSelectedForBattle();
        }
    }
}

// GIVEN a character flees from a battle
// THEN the character should be available for any more battles
SCENARIO("A character flees from a battle should be selected for another battle")
{
    TMEStep::NewStory();
    
    GIVEN("A character flees from battle")
    {
        BattleSteps::LordFleesFromBattle();
        
        THEN("they should be selected for battle")
        {
            BattleSteps::LordShouldBeSelectedForBattle();
        }
    }
}

// GIVEN a hard game
// AND a lord has followers
// WHEN they are killed by another lord
// THEN they should not be displaced
// AND the group should be dispanded

SCENARIO("In hard mode when leader loses to another lord")
{
    GIVEN("A hard game")
    {
        TMEStep::GivenHardGame();
        
        AND_GIVEN("that the lord has followers")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("they are killed by another lord")
            {
                BattleSteps::WhenLeaderKilledByLord();
                
                THEN("they should not be displaced")
                {
                    BattleSteps::ThenTheLeaderShouldNotBeDisplaced();
                
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
// WHEN they are killed by another lord
// THEN the leader should not be displaced
// AND the group should not be dispanded
SCENARIO("In normal mode when leader loses to another lord")
{
    GIVEN("A normal game")
    {
        TMEStep::GivenNormalGame();
        
        AND_GIVEN("that lord has followers")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("they are killed by another lord")
            {
                BattleSteps::WhenFollowerKilledByLord();
                
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
// AND that the lord is a follower
// WHEN they are killed by another lord
// THEN they should not be displaced
// AND the group should not be dispanded

SCENARIO("In hard mode when follower loses to another lord")
{
    GIVEN("A hard game")
    {
        TMEStep::GivenHardGame();
        
        AND_GIVEN("that the lord is a follower")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("they are killed by another lord")
            {
                BattleSteps::WhenFollowerKilledByLord();
                
                THEN("they should not be displaced")
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

// GIVEN a normal game
// AND that the lord is a follower
// WHEN they are killed by another lord
// THEN they should not be displaced
// AND the group should not be dispanded
// AND the follower should no longer be available for battle
SCENARIO("In normal mode when follower loses to another lord")
{
    GIVEN("A normal game")
    {
        TMEStep::GivenNormalGame();
        
        AND_GIVEN("that the lord is a follower")
        {
            BattleSteps::GivenLeaderWithFollowers();

            WHEN("they are killed by another lord")
            {
                BattleSteps::WhenFollowerKilledByLord();
                
                THEN("they should not be displaced")
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
