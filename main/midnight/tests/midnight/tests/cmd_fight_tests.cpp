//
//  cmd_fight_tests.cpp
//  midnight
//
//  Created by Chris Wild on 25/03/2023.
//
#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


void ThingKilled(string text)
{
    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains(text));
    REQUIRE( MapStep::GetObjectAtLocation(loc_t(1,1)) == OB_NONE );
}

void KillsTheWolves()
{
    ThingKilled("He slew the Wolves.");
}

void KillsTheSkulrin()
{
    ThingKilled("He slew the Skulkrin.");
}

void KillsTheIceTrolls()
{
    ThingKilled("He slew the Ice Trolls.");
}

void KillsTheDragons()
{
    ThingKilled("He slew the Dragons.");
}

void LordDead(const string& name)
{
    auto lord = GetMockCharacter(name);
    REQUIRE ( lord->HasEvent("Cmd_Dead") );
    REQUIRE ( lord->IsDead() );
    REQUIRE ( TME_LastActionMsg() == "");
}

void ShouldFightAndWin(const string& lord)
{
    TMEStep::LordShouldNotDieInFight(lord);
    TMEStep::LordFights(lord);
}

void ShouldFightAndCouldDie(const string& lord)
{
    TMEStep::LordShouldDieInFight(lord);
    TMEStep::LordFights(lord);
}

string LordNeedstoFightNasty( const string& name, mxthing_t nasty)
{
    auto location = loc_t(1,1);
    TMEStep::LordAtLocation(name, location);
    MapStep::SetObjectAtLocation(location, nasty);
    return name;
}

// OB_WOLVES,
// OB_DRAGONS,
// OB_ICETROLLS,
// OB_SKULKRIN,

SCENARIO("Lord fights Wolves")
{
    TMEStep::NewStory();

    GIVEN("That lord is at same location as Wolves")
    {
        auto lord = LordNeedstoFightNasty( TMEStep::ch_morkin, OB_WOLVES );
        
        AND_GIVEN("that lord is carrying the wolf slayer")
        {
            TMEStep::LordCarryingObject(lord, OB_WOLFSLAYER);
            
            WHEN("lord fights thing")
            {
                ShouldFightAndCouldDie(lord);
                
                THEN("wolves will be killed by the wolfslayer")
                {
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("He slew the Wolves with the Sword Wolfslayer."));
                }
            }
        }

        AND_GIVEN("that lord is carrying dragon slayer")
        {
            TMEStep::LordCarryingObject(lord, OB_DRAGONSLAYER);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("wolves will be killed")
                {
                    KillsTheWolves();
                }
            }
        }
        
        AND_GIVEN("that lord is carrying moon ring")
        {
            TMEStep::LordCarryingObject(lord, OB_MOONRING);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("wolves will be killed")
                {
                    KillsTheWolves();
                }
            }
        }
        
        AND_GIVEN("that lord is carrying ice crown")
        {
            TMEStep::LordCarryingObject(lord, OB_ICECROWN);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("wolves will be killed")
                {
                    KillsTheWolves();
                }
            }
        }

        AND_GIVEN("the lord is carrying nothing")
        {
            TMEStep::LordCarryingObject(lord, OB_NONE);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("wolves will be killed")
                {
                    KillsTheWolves();
                }
            }
            
            WHEN("lord fights thing and loses")
            {
                ShouldFightAndCouldDie(lord);
                
                THEN("then lord dies")
                {
                    LordDead(lord);
                }
            }
        }
    }
}

SCENARIO("Lord fights Skulkrin")
{
    TMEStep::NewStory();

    GIVEN("That lord is at same location as Skulkrin")
    {
        auto lord = LordNeedstoFightNasty( TMEStep::ch_morkin, OB_SKULKRIN );
        
        AND_GIVEN("that lord is carrying the wolf slayer")
        {
            TMEStep::LordCarryingObject(lord, OB_WOLFSLAYER);
            
            WHEN("lord fights thing")
            {
                ShouldFightAndWin(lord);
    
                THEN("skulkrin will be killed")
                {
                    KillsTheSkulrin();
                }
            }
        }

        AND_GIVEN("that lord is carrying dragon slayer")
        {
            TMEStep::LordCarryingObject(lord, OB_DRAGONSLAYER);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);;
                
                THEN("skulkrin will be killed")
                {
                    KillsTheSkulrin();
                }
            }
        }
        
        AND_GIVEN("that lord is carrying moon ring")
        {
            TMEStep::LordCarryingObject(lord, OB_MOONRING);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndCouldDie(lord);
                
                THEN("skullkrin flee")
                {
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("The Skulkrin fled from the warmth of the Moon Ring."));
                }
            }
        }
        
        AND_GIVEN("that lord is carrying ice crown")
        {
            TMEStep::LordCarryingObject(lord, OB_ICECROWN);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("skulkrin will be killed")
                {
                    KillsTheSkulrin();
                }
            }
        }

        AND_GIVEN("the lord is carrying nothing")
        {
            TMEStep::LordCarryingObject(lord, OB_NONE);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("skulkrin will be killed")
                {
                    KillsTheSkulrin();
                }
            }
            
            WHEN("lord fights thing and loses")
            {
                ShouldFightAndCouldDie(lord);
                
                THEN("then lord dies")
                {
                    LordDead(lord);
                }
            }
        }
    }
}

SCENARIO("Lord fights Ice Trolls")
{
    TMEStep::NewStory();

    GIVEN("That lord is at same location as Ice Trolls")
    {
        auto lord = LordNeedstoFightNasty( TMEStep::ch_morkin, OB_ICETROLLS );

        AND_GIVEN("that lord is carrying the wolf slayer")
        {
            TMEStep::LordCarryingObject(lord, OB_WOLFSLAYER);
            
            WHEN("lord fights thing")
            {
                ShouldFightAndWin(lord);
                
                THEN("Ice Trolls will be killed")
                {
                    KillsTheIceTrolls();
                }
            }
        }

        AND_GIVEN("that lord is carrying dragon slayer")
        {
            TMEStep::LordCarryingObject(lord, OB_DRAGONSLAYER);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("Ice Trolls will be killed")
                {
                    KillsTheIceTrolls();
                }
            }
        }
        
        AND_GIVEN("that lord is carrying moon ring")
        {
            TMEStep::LordCarryingObject(lord, OB_MOONRING);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("Ice Trolls will be killed")
                {
                    KillsTheIceTrolls();
                }
            }
        }
        
        AND_GIVEN("that lord is carrying ice crown")
        {
            TMEStep::LordCarryingObject(lord, OB_ICECROWN);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndCouldDie(lord);
                
                THEN("the Ice trolls will be killed by the dragonslayer")
                {
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("He touches the Ice Trolls with the Ice Crown and they were slain by their own fear."));
                }
            }
        }

        AND_GIVEN("the lord is carrying nothing")
        {
            TMEStep::LordCarryingObject(lord, OB_NONE);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("Ice Trolls will be killed")
                {
                    KillsTheIceTrolls();
                }
            }
            
            WHEN("lord fights thing and loses")
            {
                ShouldFightAndCouldDie(lord);
                
                THEN("then lord dies")
                {
                    LordDead(lord);
                }
            }
        }
    }
}

SCENARIO("Lord fights Dragons")
{
    TMEStep::NewStory();

    GIVEN("That lord is at same location as Dragons")
    {
        auto lord = LordNeedstoFightNasty( TMEStep::ch_morkin, OB_DRAGONS );
        
        AND_GIVEN("that lord is carrying the wolf slayer")
        {
            TMEStep::LordCarryingObject(lord, OB_WOLFSLAYER);
            
            WHEN("lord fights thing")
            {
                ShouldFightAndWin(lord);
                
                THEN("the dragons will be killed")
                {
                    KillsTheDragons();
                }
            }
        }

        AND_GIVEN("that lord is carrying dragon slayer")
        {
            TMEStep::LordCarryingObject(lord, OB_DRAGONSLAYER);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndCouldDie(lord);
                
                THEN("the dragons will be killed by the dragonslayer")
                {
                    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("He slew the Dragons with the Sword Dragonslayer."));
                }
            }
        }
        
        AND_GIVEN("that lord is carrying moon ring")
        {
            TMEStep::LordCarryingObject(lord, OB_MOONRING);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("the dragons will be killed")
                {
                    KillsTheDragons();
                }
            }
        }
        
        AND_GIVEN("that lord is carrying ice crown")
        {
            TMEStep::LordCarryingObject(lord, OB_ICECROWN);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("the dragons will be killed")
                {
                    KillsTheDragons();
                }
            }
        }

        AND_GIVEN("the lord is carrying nothing")
        {
            TMEStep::LordCarryingObject(lord, OB_NONE);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndWin(lord);
                
                THEN("the dragons will be killed")
                {
                    KillsTheDragons();
                }
            }
            
            WHEN("lord fights thing and loses")
            {
                ShouldFightAndCouldDie(lord);
                
                THEN("then lord dies")
                {
                    LordDead(lord);
                }
            }
        }
    }
}

SCENARIO("Lord fights a nasty with no object and no army")
{
    TMEStep::NewStory();

    GIVEN("The lord has no army")
    {
        auto lord = LordNeedstoFightNasty( TMEStep::ch_morkin, OB_DRAGONS );

        AND_GIVEN("the lord is carrying nothing")
        {
            TMEStep::LordCarryingObject(lord, OB_NONE);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndCouldDie(lord);
                
                THEN("the lord will be killed")
                {
                    LordDead(lord);
                }
            }
        }
    }
}

SCENARIO("Lord fights a nasty with no object but has army")
{
    TMEStep::NewStory();

    GIVEN("The lord has an army")
    {
        auto lord = LordNeedstoFightNasty( "CH_KORINEL", OB_DRAGONS );
            
        AND_GIVEN("the lord is carrying nothing")
        {
            TMEStep::LordCarryingObject(lord, OB_NONE);
            
            WHEN("lord fights thing and wins")
            {
                ShouldFightAndCouldDie(lord);
                
                THEN("the dragons will be killed")
                {
                    KillsTheDragons();
                }
            }
        }
    }
}

SCENARIO("Lord fights a nasty with no object and no army, but another army present")
{
    TMEStep::NewStory();

    GIVEN("The lord has no army")
    {
        auto lord = LordNeedstoFightNasty( TMEStep::ch_morkin, OB_DRAGONS );
            
        AND_GIVEN("another lord is present with an army")
        {
            TMEStep::LordsAtSameLocation("CH_KORINEL", lord);
      
            AND_GIVEN("the lord is carrying nothing")
            {
                TMEStep::LordCarryingObject(lord, OB_NONE);
                
                WHEN("lord fights thing and wins")
                {
                    ShouldFightAndCouldDie(lord);
                    
                    THEN("the dragons will be killed")
                    {
                        KillsTheDragons();
                    }
                }
            }
        }
    }
}

SCENARIO("Lord fights a nasty with no object and no army, but another army less lord is present")
{
    TMEStep::NewStory();

    GIVEN("The lord has no army")
    {
        auto lord = LordNeedstoFightNasty( TMEStep::ch_morkin, OB_DRAGONS );
            
        AND_GIVEN("another lord is present with no army")
        {
            TMEStep::LordsAtSameLocation(TMEStep::ch_rorthron, lord);
      
            AND_GIVEN("the lord is carrying nothing")
            {
                TMEStep::LordCarryingObject(lord, OB_NONE);
                
                WHEN("lord fights thing and loses")
                {
                    ShouldFightAndCouldDie(lord);
                    
                    THEN("the lord will be killed")
                    {
                        LordDead(lord);
                    }
                }
            }
        }
    }
}

SCENARIO("Lord fights a nasty with no object and no army but leads another with an army")
{
    TMEStep::NewStory();

    GIVEN("The lord has no army")
    {
        auto lord = LordNeedstoFightNasty( TMEStep::ch_morkin, OB_DRAGONS );
            
        AND_GIVEN("and leads another lord with an army")
        {
            TMEStep::LordHasFollowers(lord, { "CH_KORINEL" });
      
            AND_GIVEN("the lord is carrying nothing")
            {
                TMEStep::LordCarryingObject(lord, OB_NONE);
                
                WHEN("lord fights thing and wins")
                {
                    ShouldFightAndCouldDie(lord);
                    
                    THEN("the dragons will be killed")
                    {
                        KillsTheDragons();
                    }
                }
            }
        }
    }
}

SCENARIO("Lord fights a nasty with no object and no army but leads others")
{
    LPCSTR mode_text[] = { "NORMAL", "EASY", "MEDIUM", "HARD",  };
    auto mode = GENERATE( DF_NORMAL, DF_EASY, DF_MEDIUM, DF_HARD );
        
    GIVEN(StringExtensions::Format("Difficulty is %s", mode_text[mode]))
    {
        TMEStep::NewStory(RF_DEFAULT, mode);

        AND_GIVEN("The lord has no army")
        {
            auto lord = LordNeedstoFightNasty( TMEStep::ch_morkin, OB_DRAGONS );

            AND_GIVEN("the lord is carrying nothing")
            {
                TMEStep::LordCarryingObject(lord, OB_NONE);
            
                AND_GIVEN("and leads one lord without armies")
                {
                    TMEStep::LordHasFollowers(lord, { TMEStep::ch_rorthron });

                    WHEN("lord fights dragon")
                    {
                        ShouldFightAndCouldDie(lord);
                        
                        THEN("the dragons will be killed")
                        {
                            LordDead(lord);
                        }
                    }
                }
                
                AND_GIVEN("and leads two lord without armies")
                {
                    TMEStep::LordHasFollowers(lord, { TMEStep::ch_rorthron, TMEStep::ch_luxor });

                    WHEN("lord fights dragon")
                    {
                        ShouldFightAndCouldDie(lord);
                        
                        THEN("the dragons will be killed")
                        {
                            if ( mode == DF_EASY ) {
                                KillsTheDragons();
                            }else{
                                LordDead(lord);
                            }
                        }
                    }
                }
                
                AND_GIVEN("and leads three lord without armies")
                {
                    TMEStep::LordHasFollowers(lord, { TMEStep::ch_rorthron, TMEStep::ch_luxor, "CH_CORLETH" });

                    WHEN("lord fights dragon")
                    {
                        ShouldFightAndCouldDie(lord);
                        
                        THEN("the dragons will be killed")
                        {
                            if ( mode == DF_HARD ) {
                                LordDead(lord);
                            }else{
                                KillsTheDragons();
                            }
                        }
                    }
                }
            }
        }
    }
}

