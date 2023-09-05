//
//  battle_tests.cpp
//  revenge
//
//  Created by Chris Wild on 02/09/2023.
//

#include "../tme_steps.h"
#include "../mocks.h"

// GIVEN a lord has followers
// WHEN he loses a battle another lord
// THEN in difficult mode the leader should be displaced
// AND the group should be dispanded
// AND the leader should still be available for battle

SCENARIO("A leader loses to another lord and should be displaced")
{
    TMEStep::NewStory();
  
    GIVEN("that the current lord has followers")
    {
        // 
    
        TMEStep::SetCurrentLord( TMEStep::ch_rorthron );
        TMEStep::CurrentLordIsDead();
//
//        WHEN("the player loads the story")
//        {
//            UIStep::PlayerContinuesAStory();
//
//            THEN("The Think screen should be shown")
//            {
//                REQUIRE( UIStep::CurrentPanelIs(MODE_THINK) );
//            }
//        }
    }
}



// WHEN a leader loses to another lord
// THEN when not difficult mode the leader should not be displaced
// AND the group should not be dispanded
// AND the leader should no longer be available for battle

// WHEN a follower loses to another lord
// THEN in difficult mode the leader should be displaced
// AND the group should be dispanded
// AND the leader should still be available for battle

// WHEN a follower loses to another lord
// THEN when not difficult mode the leader should not be displaced
// AND the group should not be dispanded
// AND the leader should no longer be available for battle

// WHEN a character's battle is over
// THEN the character should not be included in any more battles

// WHEN a character has been displaced
// THEN the character should be available for any more battles

// WHEN a leader's army is wiped out
// THEN in difficult mode the leader should be displaced
// AND the group should be dispanded
// AND the leader should still be available for battle

// WHEN a leader's army is wiped out
// THEN when not difficult mode the leader should not be displaced
// AND the group should not be dispanded
// AND the leader should no longer be available for battle

// WHEN a follower's army is wiped out
// THEN in difficult mode the leader should be displaced
// AND the group should be dispanded
// AND the leader should still be available for battle

// WHEN a follower's army is wiped out
// THEN when not difficult mode the leader should not be displaced
// AND the group should not be dispanded
// AND the leader should no longer be available for battle
