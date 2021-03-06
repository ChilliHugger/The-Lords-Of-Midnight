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

SCENARIO("Morkin and his friends can destroy the ice crown")
{
    TMEStep::NewStory();

    GIVEN("Morkin is carrying the ice crown ")
    {
        TMEStep::LordCarryingObject(ch_morkin,OB_ICECROWN);
     
        AND_GIVEN("that Farmflame the Dragonlord is with Morkin")
        {
            TMEStep::LordAtLocation(ch_farflame, ch_morkin);

            WHEN("night falls")
            {
                TMEStep::NightFalls();

                THEN("the ice crown should be destroyed by Farflame")
                {
                  REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
                  REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Farflame the Dragonlord has destroyed the Ice Crown"));
                }
            }
        }
        
        AND_GIVEN("that Lorgrim the Wise is with Morkin")
        {
            TMEStep::LordAtLocation(ch_lorgrim, ch_morkin);
           
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("the ice crown should be destroyed by Lorgrim")
                {
                  REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
                  REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Lorgrim the Wise has destroyed the Ice Crown"));
                }
            }
        }
        
        AND_GIVEN("that Fawkrin the Skulkrin is with Morkin")
        {
            TMEStep::LordAtLocation(ch_fawkrin, ch_morkin);

            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("the ice crown should be destroyed by Fawkrin")
                {
                  REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
                  REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Fawkrin the Skulkrin has destroyed the Ice Crown"));
                }
            }
        }

        AND_GIVEN("that Morkin is at Lake Mirrow ")
        {
            TMEStep::LordAtLocation(ch_morkin, pl_lake_mirrow);
             
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("the ice crown should be destroyed in the Lake")
                {
                  REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
                  REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Morkin cast the Ice Crown into Lake Mirrow and destroyed it"));
                }
            }
        }
    
    }
}

SCENARIO("The Ice Crown can only be destroyed when Morkinn is carrying it")
{
    TMEStep::NewStory();
  
    GIVEN("Farmflame the Dragonlord is at the Tower of Doom ")
    {
        TMEStep::LordAtLocation(ch_farflame, pl_tower_of_doom);
        
        AND_GIVEN("that Morkin is not carrying anything and is at the Tower of Doom")
        {
            TMEStep::LordCarryingObject(ch_morkin,OB_NONE);
            
            TMEStep::LordAtLocation(ch_morkin, pl_tower_of_doom);
    
            AND_GIVEN("that the Ice Crown is at the Tower of Doom")
            {
                auto iceCrown = GetEntity<mxobject>("OB_ICECROWN");
                auto towerOfDoom = GetEntity<mxplace>(pl_tower_of_doom);
                tme::mx->scenario->DropObject(towerOfDoom->Location(), iceCrown);
            
                WHEN("night falls")
                {
                    TMEStep::NightFalls();

                    THEN("the ice crown should not be destroyed by Farflame")
                    {
                        REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_NONE );
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
                    REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Ushgarak has fallen."));
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Victory to the Free!"));
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
                    REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Ushgarak has fallen and the Ice Crown has been destroyed."));
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Victory to the Free!"));
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
                    REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_LOSE );
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Xajorkith has fallen and Morkin is dead!"));
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Victory to Doomdark!"));
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
                    REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_LOSE );
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Luxor and Morkin are dead!"));
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Victory to Doomdark!"));
                }
            }
        }
    }
}
