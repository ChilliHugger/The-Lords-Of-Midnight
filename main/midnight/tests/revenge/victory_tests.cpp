//
//  victory_tests.cpp
//  revenge
//
//  Created by Chris Wild on 12/10/2023.
//
// You will have heard by now, surely my Lords, of the war that has been raging in the lost land of Midnight far,
// far to the south west of our Icemark. Though it is now two moons since its conclusion,
// the news of the Moonprince's victory has travelled slowly.
// Rumours of a secret traffic betwixt the Heartstealer and the Witchking have come to our ears for many moons now.
// Indeed, on the eve of the Solstice itself, did we not waylay a band of dark and foul warriors riding north
// from the Gate of Varenorn?
//

#include "common_steps.h"

SCENARIO("Luxor is dead - Victory to the Heartstealer")
{
    TMEStep::NewStory();

    GIVEN("Luxor is dead")
    {
        TMEStep::LordIsDead(TMEStep::ch_luxor);
     
        WHEN("night falls")
        {
            TMEStep::NightFalls();
            
            THEN("then victory to the heartstealer")
            {
              REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
              REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Morkin cast the Ice Crown into Lake Mirrow and destroyed it"));
            }
        }
    }
}


//
//
//
//SCENARIO("Luxor is dead - Victory to the Heartstealer")
//{
//    TMEStep::NewStory();
//
//    GIVEN("Luxor is dead")
//    {
//        TMEStep::LordCarryingObject(TMEStep::ch_morkin,OB_ICECROWN);
//     
//        AND_GIVEN("that Farmflame the Dragonlord is with Morkin")
//        {
//            TMEStep::LordAtLocation(LOMStep::ch_farflame, TMEStep::ch_morkin);
//
//            WHEN("night falls")
//            {
//                TMEStep::NightFalls();
//
//                THEN("the ice crown should be destroyed by Farflame")
//                {
//                  REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
//                  REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Farflame the Dragonlord has destroyed the Ice Crown"));
//                }
//            }
//        }
//        
//        AND_GIVEN("that Lorgrim the Wise is with Morkin")
//        {
//            TMEStep::LordAtLocation(LOMStep::ch_lorgrim, TMEStep::ch_morkin);
//           
//            WHEN("night falls")
//            {
//                TMEStep::NightFalls();
//                
//                THEN("the ice crown should be destroyed by Lorgrim")
//                {
//                  REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
//                  REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Lorgrim the Wise has destroyed the Ice Crown"));
//                }
//            }
//        }
//        
//        AND_GIVEN("that Fawkrin the Skulkrin is with Morkin")
//        {
//            TMEStep::LordAtLocation(LOMStep::ch_fawkrin, TMEStep::ch_morkin);
//
//            WHEN("night falls")
//            {
//                TMEStep::NightFalls();
//                
//                THEN("the ice crown should be destroyed by Fawkrin")
//                {
//                  REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
//                  REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Fawkrin the Skulkrin has destroyed the Ice Crown"));
//                }
//            }
//        }
//
//        AND_GIVEN("that Morkin is at Lake Mirrow ")
//        {
//            TMEStep::LordAtLocation(TMEStep::ch_morkin, LOMStep::pl_lake_mirrow);
//             
//            WHEN("night falls")
//            {
//                TMEStep::NightFalls();
//                
//                THEN("the ice crown should be destroyed in the Lake")
//                {
//                  REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
//                  REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("Morkin cast the Ice Crown into Lake Mirrow and destroyed it"));
//                }
//            }
//        }
//    
//    }
//}
//
