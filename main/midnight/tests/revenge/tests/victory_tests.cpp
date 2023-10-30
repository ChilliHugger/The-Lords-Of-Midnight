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

#include "../steps/common_steps.h"

#include <iostream>
#include <vector>
using namespace std;

const string SharethVictory = "The evil Shareth has won victory!";
const string LuxorVictory = "The Victory of the Moonprince";
const string Watchwords = "The Watchwords of Midnight are revealed!";
const string LuxorHere = "Luxor is here";
const string MorkinHere = "Morkin is here";
const string TarithelHere = "Tarithel is here";
const string RorthronHere = "Rorthron is here";
const string SharethDead = "Shareth is dead";
const string LuxorVictoryIs = "The Victory of the Moonprince is ";
const string LuxurKilledBy = "Luxor the Moonprince is dead,";


void WatchwordsShouldBeShown()
{
    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains(Watchwords));
}

void WatchwordsShouldNotBeShown()
{
    REQUIRE_THAT( TME_LastActionMsg(), !Catch::Matchers::Contains(Watchwords));
}

void LuxorVictoryShouldBe(string howmuch)
{
    REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_WIN );
    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains(LuxorVictoryIs + howmuch));
}

void SharethShouldHaveVictory()
{
    REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_LOSE );
    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains(SharethVictory));
}

void LuxorShouldBeDeadBy(string howdead)
{
    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains(LuxurKilledBy + howdead));
}

void SharethShouldBeDead()
{
    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains(SharethDead));
}

void SharethShouldNotBeDead()
{
    REQUIRE_THAT( TME_LastActionMsg(), !Catch::Matchers::Contains(SharethDead));
}

void LordsShouldBeHere(vector<string> names) {
    for (string &name : names) {
        REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains(name));
    }
}

void LordsShouldNotBeHere(vector<string> names) {
    for (string &name : names) {
        REQUIRE_THAT( TME_LastActionMsg(), !Catch::Matchers::Contains(name));
    }
}

void ThereShouldBeNoVictory()
{
    REQUIRE( TME_CheckWinLose() == m_gameover_t::MG_NONE );
    LordsShouldNotBeHere( { LuxorHere, MorkinHere, TarithelHere, RorthronHere } );
    SharethShouldNotBeDead();
    REQUIRE_THAT( TME_LastActionMsg(), !Catch::Matchers::Contains(LuxorVictory));
    WatchwordsShouldNotBeShown();
}

SCENARIO("Luxor is dead - Victory to the Heartstealer")
{
    TMEStep::NewStory();

    // 1
    GIVEN("Luxor is dead, killed in battle")
    {
        TMEStep::LordIsDead(TMEStep::ch_luxor);

        WHEN("night falls")
        {
            TMEStep::NightFalls();

            THEN("victory to the heartstealer")
            {
                LuxorShouldBeDeadBy("  killed in battle");
                SharethShouldHaveVictory();
                WatchwordsShouldNotBeShown();
            }
        }
    }
    
    // 2
    GIVEN("Luxor is dead, killed by wolves")
    {
        TMEStep::LordKilledByObject(TMEStep::ch_luxor, TMEStep::ob_wolves);
     
        WHEN("night falls")
        {
            TMEStep::NightFalls();
            
            THEN("victory to the heartstealer")
            {
                LuxorShouldBeDeadBy(" slain by wolves");
                SharethShouldHaveVictory();
                WatchwordsShouldNotBeShown();
            }
        }
    }
    
    // 3
    GIVEN("Luxor is dead, killed by shareth")
    {
        DDRStep::LordKilledByCharacter(TMEStep::ch_luxor, DDRStep::ch_shareth);
     
        WHEN("night falls")
        {
            TMEStep::NightFalls();
            
            THEN("victory to the heartstealer")
            {
                LuxorShouldBeDeadBy("  killed in battle by Shareth the Heartstealer");
                SharethShouldHaveVictory();
                WatchwordsShouldNotBeShown();
            }
        }
    }
    
}

// 4
SCENARIO("All main lords back to gate and shareth is dead - victory extremly overwhelming - watchwords")
{
    TMEStep::NewStory();
    
    GIVEN("Shareth is dead")
    {
        TMEStep::LordIsDead(DDRStep::ch_shareth);
        
        AND_GIVEN("all main lords have returned to the gate of varenorn")
        {
            DDRStep::MorkinRescued();
            DDRStep::LordsAtGateOfVarenorn({
                TMEStep::ch_luxor, TMEStep::ch_morkin, DDRStep::ch_tarithel, TMEStep::ch_rorthron
            });
                    
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("victory to the free")
                {
                    LordsShouldBeHere( { LuxorHere, MorkinHere, TarithelHere, RorthronHere } );
                    SharethShouldBeDead();
                    LuxorVictoryShouldBe("extremely overwhelming");
                    WatchwordsShouldBeShown();
                }
            }
        }
    }
}

//
SCENARIO("All main lords back to gate, artifacts carried, and shareth is dead - victory utterly overwhelming - watchwords")
{
    TMEStep::NewStory();
    
    GIVEN("Shareth is dead")
    {
        TMEStep::LordIsDead(DDRStep::ch_shareth);
        
        AND_GIVEN("all main lords have returned to the gate of varenorn")
        {
            DDRStep::MorkinRescued();
            DDRStep::LordsAtGateOfVarenorn({
                TMEStep::ch_luxor, TMEStep::ch_morkin, DDRStep::ch_tarithel, TMEStep::ch_rorthron
            });

            
            AND_GIVEN("one of the lords is carrying the Crown of Varenand")
            {
                TMEStep::LordCarryingObject(DDRStep::ch_tarithel, "OB_CROWN_VARENAND");
         
                WHEN("night falls")
                {
                    TMEStep::NightFalls();
                    
                    THEN("victory to the free")
                    {
                        LordsShouldBeHere( { LuxorHere, MorkinHere, TarithelHere, RorthronHere } );
                        SharethShouldBeDead();
                        LuxorVictoryShouldBe("utterly overwhelming");
                        WatchwordsShouldBeShown();
                    }
                }
            }
            
            AND_GIVEN("one of the lords is carrying the Crown of Carudrium")
            {
                TMEStep::LordCarryingObject(TMEStep::ch_luxor, "OB_CROWN_CARUDRIUM");
         
                WHEN("night falls")
                {
                    TMEStep::NightFalls();
                    
                    THEN("victory to the free")
                    {
                        LordsShouldBeHere( { LuxorHere, MorkinHere, TarithelHere, RorthronHere } );
                        SharethShouldBeDead();
                        LuxorVictoryShouldBe("utterly overwhelming");
                        WatchwordsShouldBeShown();
                    }
                }
            }

            AND_GIVEN("one of the lords is carrying the Spell of Thigrorn")
            {
                TMEStep::LordCarryingObject(TMEStep::ch_rorthron, "OB_SPELL_THIGRORN");
         
                WHEN("night falls")
                {
                    TMEStep::NightFalls();
                    
                    THEN("victory to the free")
                    {
                        LordsShouldBeHere( { LuxorHere, MorkinHere, TarithelHere, RorthronHere } );
                        SharethShouldBeDead();
                        LuxorVictoryShouldBe("utterly overwhelming");
                        WatchwordsShouldBeShown();
                    }
                }
            }

            AND_GIVEN("one of the lords is carrying the Runes of Finorn")
            {
                TMEStep::LordCarryingObject(TMEStep::ch_morkin, "OB_RUNES_FINORN");
         
                WHEN("night falls")
                {
                    TMEStep::NightFalls();
                    
                    THEN("victory to the free")
                    {
                        LordsShouldBeHere( { LuxorHere, MorkinHere, TarithelHere, RorthronHere } );
                        SharethShouldBeDead();
                        LuxorVictoryShouldBe("utterly overwhelming");
                        WatchwordsShouldBeShown();
                    }
                }
            }
            
            AND_GIVEN("one of the lords is carrying the Crown of Imiriel")
            {
                TMEStep::LordCarryingObject(DDRStep::ch_tarithel, "OB_CROWN_IMIRIEL");
         
                WHEN("night falls")
                {
                    TMEStep::NightFalls();
                    
                    THEN("victory to the free")
                    {
                        LordsShouldBeHere( { LuxorHere, MorkinHere, TarithelHere, RorthronHere } );
                        SharethShouldBeDead();
                        LuxorVictoryShouldBe("utterly overwhelming");
                        WatchwordsShouldBeShown();
                    }
                }
            }

        }
    }
}


// 5
SCENARIO("All main lords back to gate and shareth is alive - victory is very worthy")
{
    TMEStep::NewStory();
    
    GIVEN("Shareth is alive")
    {
        AND_GIVEN("all main lords have returned to the gate of varenorn")
        {
            DDRStep::MorkinRescued();
            DDRStep::LordsAtGateOfVarenorn({
                TMEStep::ch_luxor, TMEStep::ch_morkin, DDRStep::ch_tarithel, TMEStep::ch_rorthron
            });

                    
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("victory to the free")
                {
                    LordsShouldBeHere( { LuxorHere, MorkinHere, TarithelHere, RorthronHere } );
                    SharethShouldNotBeDead();
                    LuxorVictoryShouldBe("very worthy");
                    WatchwordsShouldNotBeShown();

                }
            }
        }
    }
}

// 6
SCENARIO("Morkin, Tarithel, and Rorthron back to gate and shareth is alive - No victory")
{
    TMEStep::NewStory();
    
    GIVEN("Shareth is alive")
    {
        AND_GIVEN("Morkin, Tarithel, and Rorthron have returned to the gate of varenorn")
        {
            DDRStep::MorkinRescued();
            DDRStep::LordNotAtGateOfVarenorn(TMEStep::ch_luxor);
            DDRStep::LordsAtGateOfVarenorn({
                TMEStep::ch_morkin, DDRStep::ch_tarithel, TMEStep::ch_rorthron
            });

                    
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("there is no victory")
                {
                    ThereShouldBeNoVictory();
                }
            }
        }
    }
}

// 7
SCENARIO("Luxor, Tarithel, and Rorthron back to gate and shareth is alive - No victory")
{
    TMEStep::NewStory();
    
    GIVEN("Shareth is alive")
    {
        AND_GIVEN("Luxor, Tarithel, and Rorthron have returned to the gate of varenorn")
        {
            DDRStep::MorkinRescued();
            DDRStep::LordsAtGateOfVarenorn({
                TMEStep::ch_luxor, DDRStep::ch_tarithel, TMEStep::ch_rorthron
            });
            DDRStep::LordNotAtGateOfVarenorn(TMEStep::ch_morkin);
                    
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("there is no victory")
                {
                    ThereShouldBeNoVictory();
                }
            }
        }
    }
}

// 8
SCENARIO("Luxor, Tarithel, and Rorthron back to gate and shareth is dead - victory is very noble")
{
    TMEStep::NewStory();
    
    GIVEN("Shareth is dead")
    {
        TMEStep::LordIsDead(DDRStep::ch_shareth);
                
        AND_GIVEN("Luxor, Tarithel, and Rorthron have returned to the gate of varenorn")
        {
            DDRStep::MorkinRescued();
            DDRStep::LordsAtGateOfVarenorn({
                TMEStep::ch_luxor, DDRStep::ch_tarithel, TMEStep::ch_rorthron
            });
            DDRStep::LordNotAtGateOfVarenorn(TMEStep::ch_morkin);
                    
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("victory to the free")
                {
                    LordsShouldBeHere( { LuxorHere, TarithelHere, RorthronHere } );
                    LordsShouldNotBeHere( { MorkinHere } );
                    SharethShouldBeDead();
                    LuxorVictoryShouldBe("very noble");
                    WatchwordsShouldNotBeShown();
                }
            }
        }
    }
}

// 9
SCENARIO("Luxor and Morkin back to gate and shareth is alive - victory is somewhat worth")
{
    TMEStep::NewStory();
    
    GIVEN("Shareth is Alive")
    {
        AND_GIVEN("Luxor and Morkin have returned to the gate of varenorn")
        {
            DDRStep::MorkinRescued();
            
            DDRStep::LordsAtGateOfVarenorn({
                TMEStep::ch_luxor, TMEStep::ch_morkin
            });

            DDRStep::LordsNotAtGateOfVarenorn({
                DDRStep::ch_tarithel, TMEStep::ch_rorthron
            });
                    
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("victory to the free")
                {
                    LordsShouldBeHere( { LuxorHere, MorkinHere } );
                    LordsShouldNotBeHere( { TarithelHere, RorthronHere } );
                    SharethShouldNotBeDead();
                    LuxorVictoryShouldBe("somewhat worthy");
                    WatchwordsShouldNotBeShown();
                }
            }
        }
    }
}


// 10
SCENARIO("Luxor and Morkin back to gate and shareth is dead - victory is quite noble")
{
    TMEStep::NewStory();
    
    GIVEN("Shareth is Dead")
    {
        TMEStep::LordIsDead(DDRStep::ch_shareth);
        
        AND_GIVEN("Luxor and Morkin have returned to the gate of varenorn")
        {
            DDRStep::MorkinRescued();
            DDRStep::LordsAtGateOfVarenorn({
                TMEStep::ch_luxor, TMEStep::ch_morkin
            });

            DDRStep::LordsNotAtGateOfVarenorn({
                DDRStep::ch_tarithel, TMEStep::ch_rorthron
            });
                    
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("victory to the free")
                {
                    LordsShouldBeHere( { LuxorHere, MorkinHere } );
                    LordsShouldNotBeHere( { TarithelHere, RorthronHere } );
                    SharethShouldBeDead();
                    LuxorVictoryShouldBe("quite noble");
                    WatchwordsShouldNotBeShown();
                }
            }
        }
    }
}

SCENARIO("Luxor back to gate and shareth is dead - victory is somewhat noble")
{
    TMEStep::NewStory();
    
    GIVEN("Shareth is Dead")
    {
        TMEStep::LordIsDead(DDRStep::ch_shareth);
        
        AND_GIVEN("Luxor has returned to the gate of varenorn")
        {
            DDRStep::MorkinRescued();
            DDRStep::LordAtGateOfVarenorn(TMEStep::ch_luxor);
            DDRStep::LordsNotAtGateOfVarenorn({
                TMEStep::ch_morkin, DDRStep::ch_tarithel, TMEStep::ch_rorthron
            });
                    
            WHEN("night falls")
            {
                TMEStep::NightFalls();
                
                THEN("victory to the free")
                {
                    LordsShouldBeHere( { LuxorHere } );
                    LordsShouldNotBeHere( { MorkinHere, TarithelHere, RorthronHere } );
                    SharethShouldBeDead();
                    LuxorVictoryShouldBe("somewhat noble");
                    WatchwordsShouldNotBeShown();
                }
            }
        }
    }
}
