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
    TME_Init();
    auto morkin = GetCharacter("CH_MORKIN");

    GIVEN("Farmflame the Dragonlord is at the same location as the ice crown ")
    {
        auto towerOfDoom = GetEntity<mxplace>("PL_TOWER_OF_DOOM");
        auto iceCrown = GetEntity<mxobject>("OB_ICECROWN");

        // Morkin is not carrying anything and is at the Tower of Doom
        morkin->carrying = nullptr;
        morkin->Location( towerOfDoom->Location() ) ;

        // And the Ice Crown is at the Tower of Doom
        tme::mx->scenario->DropObject(towerOfDoom->Location(), iceCrown);
        
        // And Farflame is at the Tower of Doom
        GetCharacter("CH_FARFLAME")->Location( towerOfDoom->Location() ) ;

        WHEN("night falls")
        {
            TMEStep::NightFalls();

            THEN("the ice crown should not be destroyed by Farflame")
            {
              REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_NONE );
            }
        }
    }


    GIVEN("Morkin is carrying the ice crown and Farmflame the Dragonlord is at the same location ")
    {
        morkin->carrying = tme::mx->ObjectById(OB_ICECROWN);
        GetCharacter("CH_FARFLAME")->Location( morkin->Location() ) ;

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

    GIVEN("Morkin is carrying the ice crown and Lorgrim the Wise is at the same location ")
    {
        morkin->carrying = tme::mx->ObjectById(OB_ICECROWN);
        GetCharacter("CH_LORGRIM")->Location( morkin->Location() ) ;

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

    GIVEN("Morkin is carrying the ice crown Fawkrin the Skulkrin is at the same location ")
    {
        morkin->carrying = tme::mx->ObjectById(OB_ICECROWN);
        GetCharacter("CH_FAWKRIN")->Location( morkin->Location() ) ;

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

    GIVEN("Morkin is carrying the ice crown and is at Lake Mirrow ")
    {
        morkin->carrying = tme::mx->ObjectById(OB_ICECROWN);
        morkin->Location( GetEntity<mxplace>("PL_LAKE_MIRROW")->Location() ) ;
           
        WHEN("night falls")
        {
            TMEStep::NightFalls();
            
            THEN("the ice crown should be destroyed by the Lake")
            {
              REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
              REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Morkin cast the Ice Crown into Lake Mirrow and destroyed it"));
            }
        }
    }

}

SCENARIO("Only Morkin can destroy the ice crown")
{
    TME_Init();
    auto morkin = GetCharacter("CH_MORKIN");

    GIVEN("Farmflame the Dragonlord is at the same location as the ice crown ")
    {
        auto towerOfDoom = GetEntity<mxplace>("PL_TOWER_OF_DOOM");
        auto iceCrown = GetEntity<mxobject>("OB_ICECROWN");

        // Morkin is not carrying anything and is at the Tower of Doom
        morkin->carrying = nullptr;
        morkin->Location( towerOfDoom->Location() ) ;

        // And the Ice Crown is at the Tower of Doom
        tme::mx->scenario->DropObject(towerOfDoom->Location(), iceCrown);
        
        // And Farflame is at the Tower of Doom
        GetCharacter("CH_FARFLAME")->Location( towerOfDoom->Location() ) ;

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

SCENARIO("Ushgarak has fallen in battle")
{
    TME_Init();

    GIVEN("Victory to the Free when the Citadel of Ushgarak falls in battle")
    {
        TMEStep::UshgarakHasFallenInBattle();
        
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
}

SCENARIO("Victory to the Free when Ushgarak has fallen in battle and the Ice Crown is destroyed")
{
    TME_Init();

    GIVEN("Luxor the Moonprince takes the Citadel of Ushgarak in battle")
    {
        // Given
        TMEStep::UshgarakHasFallenInBattle();

        // And
        TMEStep::IceCrownHasBeenDestroyed();

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

SCENARIO("Xajorkith has fallen in battle and Morkin is Dead then victory to Doomdark")
{
    TME_Init();

    GIVEN("That Xajorkith has fallen in battle and Morkin is Dead")
    {
        // Given
        TMEStep::XajorkithHasFallenInBattle();
        
        // And
        TMEStep::MorkinIsDead();
        
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

SCENARIO("Luxor and Morkin are dead victory should be to Doomdark")
{
    TME_Init();

    GIVEN("Luxor and Morkin are dead")
    {
        // Given
        TMEStep::MorkinIsDead();
        
        // And
        TMEStep::LuxorIsDead();
        
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
