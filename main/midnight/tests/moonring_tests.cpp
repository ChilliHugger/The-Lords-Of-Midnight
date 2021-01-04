//
//  moonring_tests.cpp
//  midnight
//
//  Created by Chris Wild on 04/01/2021.
//

#include "tme_steps.h"
#include "mocks.h"
#include "ui_steps.h"

#if defined(_LOM_)
#include "midnight/common_steps.h"
#endif


SCENARIO("Continuing a story with a dead lord should show the Think Screen")
{
    TMEStep::NewStory();
  
    GIVEN("that the current lord is dead")
    {
        TMEStep::CurrentLordIsDead();
        
        WHEN("the player loads the story")
        {
            UIStep::PlayerContinuesAStory();
            
            THEN("The Think screen should be shown")
            {
                REQUIRE( UIStep::CurrentPanelIs(MODE_THINK) );
            }
        }
    }
}

SCENARIO("Continuing a story after a win should show the Win Screen")
{
    TMEStep::NewStory();
  
    GIVEN("that the game has been won")
    {
        TMEStep::GameWon();
        
        WHEN("the player loads the story")
        {
            UIStep::PlayerContinuesAStory();
            
            THEN("The Win screen should be shown")
            {
                REQUIRE( UIStep::CurrentPanelIs(MODE_WIN) );
            }
        }
    }
}

SCENARIO("Continuing a story after losing should show the Lose Screen")
{
    TMEStep::NewStory();
  
    GIVEN("that the game has been won")
    {
        TMEStep::GameLost();

        WHEN("the player loads the story")
        {
            UIStep::PlayerContinuesAStory();
            
            THEN("The Lost screen should be shown")
            {
                REQUIRE( UIStep::CurrentPanelIs(MODE_LOSE) );
            }
        }
    }
}

SCENARIO("Closing a story should save, cleanup, and show the main menu")
{
    TMEStep::NewStory();
    mocks::reset();
  
    WHEN("the player closes the story")
    {
        UIStep::PlayerClosesTheCurrentStory();
        
        THEN("the story should be saved")
        {
            REQUIRE( mocks::storyHasBeenSaved() );
            
            AND_THEN("The main menu should be shown")
            {
                REQUIRE( UIStep::CurrentPanelIs(MODE_MAINMENU) );
            }
        }
    }
}

SCENARIO("Selecting the Look Screen after a Win condition should show the Win Screen")
{
    TMEStep::NewStory();
  
    GIVEN("that the game has been won")
    {
        TMEStep::GameWon();

        WHEN("the player chooses the look screen")
        {
            UIStep::PlayerTapsLook();

            THEN("The Win screen should be shown")
            {
                REQUIRE( UIStep::CurrentPanelIs(MODE_WIN) );
            }
        }
    }
}

SCENARIO("Selecting the Look Screen after a Lose condition should show the Lose Screen")
{
    TMEStep::NewStory();
  
    GIVEN("that the game has been lost")
    {
        TMEStep::GameLost();

        WHEN("the player chooses the look screen")
        {
            UIStep::PlayerTapsLook();
            
            THEN("the lost screen should be shown")
            {
                REQUIRE( UIStep::CurrentPanelIs(MODE_LOSE) );
            }
        }
    }
}

SCENARIO("Selecting the Look Screen for a dead lord should show the Select screen")
{
    TMEStep::NewStory();
  
    GIVEN("that the current lord is dead")
    {
        TMEStep::CurrentLordIsDead();

        WHEN("the player chooses the look screen")
        {
            UIStep::PlayerTapsLook();
            
            THEN("The Select screen should be shown")
            {
                REQUIRE( UIStep::CurrentPanelIs(MODE_SELECT) );
            }
        }
    }
}

SCENARIO("Selecting the Look Screen for a alive lord should show the Look screen")
{
    TMEStep::NewStory();
  
    GIVEN("that the current lord is alive")
    {
        // do nothing
        
        WHEN("the player chooses the look screen")
        {
            UIStep::PlayerTapsLook();
            
            THEN("the Look screen should be shown")
            {
                REQUIRE( UIStep::CurrentPanelIs(MODE_LOOK) );
                
                AND_THEN("the current panel should be for the current lord")
                {
                    REQUIRE( UIStep::CurrentPanelObjectIs(TME_CurrentCharacter().id) );
                }
            }
        }
    }
}

// Scenario mr->swapGroupLeader
// Scenario mr->joinGroup
// Scenario mr->leaveGroup
// Scenario mr->disbandGroup
// Scenario mr->look
// Scenario mr->selectCharacter
// Scenario mr->undo
// Scenario mr->night
// Scenario mr->approach
// Scenario mr->seek
// Scenario mr->hideunhide
// Scenario mr->fight
// Scenario mr->think
// Scenario mr->recruitment
// Scenario mr->postmen
// Scenario mr->attack
// Scenario mr->dawn

// DDR
// Scenario mr->give
// Scenario mr->take
// Scenario mr->use
// Scenario mr->rest
// Scenario mr->entertunnel













