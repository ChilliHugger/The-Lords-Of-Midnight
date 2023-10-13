//
//  victory_tests.cpp
//  midnight
//
//  Created by Chris Wild on 03/01/2021.
//
// "There was much commotion in the great Citadel. Men, and fouler creatures, were preparing themselves for war.
// The captains of Doomdark were tallying supplies, marshalling their war-bands, bustling to
// and fro in the Winter Palace with last-minute orders and requisitions. Their thoughts were only of
// victory; already they were exultant at the havoc they would wreak, the vast slaughter that lay at
// their command."
//

#include "common_steps.h"

void Victory(std::string text)
{
    REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains(text));
}

void NoVictory()
{
    REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_NONE );
}

void VictoryToTheFree(std::string text)
{
    Victory(text);
    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Victory to the Free!"));
}

void VictoryToDoomdark(std::string text)
{
    REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_LOSE );
    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains(text));
    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Victory to Doomdark!"));
}


SCENARIO("Morkin and his friends can destroy the ice crown")
{
    TMEStep::NewStory();

    GIVEN("Morkin is carrying the ice crown ")
    {
        TMEStep::LordCarryingObject(TMEStep::ch_morkin,OB_ICECROWN);
     
        AND_GIVEN("that Farmflame the Dragonlord is with Morkin")
        {
            TMEStep::LordAtLocation(LOMStep::ch_farflame, TMEStep::ch_morkin);

            WHEN("night falls")
            {
                TMEStep::NightFalls();

                THEN("the ice crown should be destroyed by Farflame")
                {
                    Victory("Farflame the Dragonlord has destroyed the Ice Crown");
                }
            }
        }
        
        AND_GIVEN("that Lorgrim the Wise is with Morkin")
        {
            TMEStep::LordAtLocation(LOMStep::ch_lorgrim, TMEStep::ch_morkin);
           
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("the ice crown should be destroyed by Lorgrim")
                {
                    Victory("Lorgrim the Wise has destroyed the Ice Crown");
                }
            }
        }
        
        AND_GIVEN("that Fawkrin the Skulkrin is with Morkin")
        {
            TMEStep::LordAtLocation(LOMStep::ch_fawkrin, TMEStep::ch_morkin);

            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("the ice crown should be destroyed by Fawkrin")
                {
                    Victory("Fawkrin the Skulkrin has destroyed the Ice Crown");
                }
            }
        }

        AND_GIVEN("that Morkin is at Lake Mirrow ")
        {
            TMEStep::LordAtLocation(TMEStep::ch_morkin, LOMStep::pl_lake_mirrow);
             
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("the ice crown should be destroyed in the Lake")
                {
                    Victory("Morkin cast the Ice Crown into Lake Mirrow and destroyed it");
                }
            }
        }
    }
}

SCENARIO("The Ice Crown can only be destroyed when Morkin is carrying it")
{
    TMEStep::NewStory();
  
    GIVEN("Farmflame the Dragonlord is at the Tower of Doom ")
    {
        TMEStep::LordAtLocation(LOMStep::ch_farflame, LOMStep::pl_tower_of_doom);
        
        AND_GIVEN("that Morkin is not carrying anything and is at the Tower of Doom")
        {
            TMEStep::LordCarryingObject(TMEStep::ch_morkin,OB_NONE);
            
            TMEStep::LordAtLocation(TMEStep::ch_morkin, LOMStep::pl_tower_of_doom);
    
            AND_GIVEN("that the Ice Crown is at the Tower of Doom")
            {
                auto iceCrown = GetEntity<mxobject>(LOMStep::ob_icecrown);
                auto towerOfDoom = GetEntity<mxplace>(LOMStep::pl_tower_of_doom);
                tme::mx->scenario->DropObject(towerOfDoom->Location(), iceCrown);
            
                WHEN("night falls")
                {
                    TMEStep::NightFalls();

                    THEN("the ice crown should not be destroyed by Farflame")
                    {
                        NoVictory();
                    }
                }
            }
        }
    }
}

SCENARIO("Victory to the Free when The Citadel of Ushgarak has fallen in battle")
{
    TMEStep::NewStory();

    GIVEN("that the Citadel of Ushgarak falls in battle")
    {
        LOMStep::UshgarakHasFallenInBattle();
        
        AND_GIVEN("that the Ice Crown has not been destroyed")
        {
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("Ushgark should have fallen and Victory to the Free")
                {
                    VictoryToTheFree("Ushgarak has fallen.");
                }
            }
        }
        
        AND_GIVEN("that the Ice Crown has been destroyed")
        {
            LOMStep::IceCrownHasBeenDestroyed();

            WHEN("night falls")
            {
                TMEStep::NightFalls();

                THEN("Ushgarak should have fallen and the Ice Crown has been destroyed and Victory to the Free")
                {
                    VictoryToTheFree("Ushgarak has fallen and the Ice Crown has been destroyed.");
                }
            }
        }
    }
}

SCENARIO("Xajorkith has fallen in battle and Morkin is Dead then victory to Doomdark")
{
    TMEStep::NewStory();

    GIVEN("That Xajorkith has fallen in battle")
    {
        LOMStep::XajorkithHasFallenInBattle();
        
        AND_GIVEN("that Morkin is dead")
        {
            LOMStep::MorkinIsDead();
            
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("Xajorkith should have fallen and Victory to Doomdark")
                {
                    VictoryToDoomdark("Xajorkith has fallen and Morkin is dead!");
                }
            }
        }
    }
}

SCENARIO("Luxor and Morkin are dead victory should be to Doomdark")
{
    TMEStep::NewStory();

    GIVEN("that Luxor is dead")
    {
        LOMStep::LuxorIsDead();
        
        AND_GIVEN("that Morkin is also dead")
        {
            LOMStep::MorkinIsDead();
        
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("Victory to Doomdark")
                {
                    VictoryToDoomdark("Luxor and Morkin are dead!");
                }
            }
        }
    }
}
