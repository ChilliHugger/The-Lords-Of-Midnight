//
//  cmd_fight_tests.cpp
//  revenge
//
//  Created by Chris Wild on 25/03/2023.
//
#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


void ThingKilled()
{
    REQUIRE_THAT( TME_LastActionMsg(), Catch::Matchers::Contains("and slain them. "));
    REQUIRE( MapStep::GetObjectAtLocation(loc_t(1,1)) == OB_NONE );
}

void LordDead(const string& name)
{
    auto lord = GetMockCharacter(name);
    REQUIRE ( lord->HasEvent("Cmd_Dead") );
    REQUIRE ( lord->IsDead() );
    REQUIRE ( TME_LastActionMsg() == "");
}

mxthing_t currentThing = OB_NONE;

void ShouldFightAndWillLose(const string& lord)
{
    auto mock = GetMockThing(currentThing);
    mock->mockData.properties["FightHP"] = "16";
    TMEStep::LordShouldDieInFight(lord);
    TMEStep::LordFights(lord);
}

void ShouldFightAndWillWin(const string& lord)
{
    auto mock = GetMockThing(currentThing);
    mock->mockData.properties["FightHP"] = "0";
    TMEStep::LordShouldDieInFight(lord);
    TMEStep::LordFights(lord);
}

string LordNeedstoFightNasty( const string& name, mxthing_t nasty)
{
    auto location = loc_t(1,1);
    TMEStep::LordAtLocation(name, location);
    MapStep::SetObjectAtLocation(location, nasty);
    TMEStep::LordCarryingObject(name, OB_NONE);
    currentThing = nasty;
    return name;
}

void ThingHasZeroKillRate()
{
    auto mock = GetMockThing(currentThing);
    mock->mockData.properties["KillRate"] = "0";
}

SCENARIO("Lord fights a thing")
{
    LPCSTR nasty_text[] = { "", "WOLVES","ICE TROLLS", "SKULKRIN", "DRAGONS"  };
    auto nasty = GENERATE( OB_WOLVES, OB_ICETROLLS, OB_SKULKRIN, OB_DRAGONS );

    TMEStep::NewStory();

    GIVEN( StringExtensions::Format("That lord is fighting %s", nasty_text[nasty]) )
    {
        auto lord = LordNeedstoFightNasty( TMEStep::ch_luxor, nasty );
        
        WHEN("lord fights and the thing loses")
        {
            ShouldFightAndWillWin(lord);
            
            THEN("thing will be killed")
            {
                ThingKilled();
            }
        }
        
        WHEN("lord fights and the thing wins")
        {
            ShouldFightAndWillLose(lord);
            
            THEN("then lord dies")
            {
                LordDead(lord);
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

        auto lord = LordNeedstoFightNasty( TMEStep::ch_morkin, OB_DRAGONS );

        AND_GIVEN("and leads one lord without armies")
        {
            TMEStep::LordHasFollowers(lord, { TMEStep::ch_rorthron });

            WHEN("lord fights thing")
            {
                ShouldFightAndWillLose(lord);

                THEN("the thing will win")
                {
                    LordDead(lord);
                }
            }
        }

        AND_GIVEN("and leads two lord without armies")
        {
            TMEStep::LordHasFollowers(lord, { TMEStep::ch_rorthron, TMEStep::ch_luxor });

            WHEN("lord fights thing")
            {
                ShouldFightAndWillLose(lord);

                THEN("the thing will be killed in easy")
                {
                    if ( mode == DF_EASY ) {
                        ThingKilled();
                    }else{
                        LordDead(lord);
                    }
                }
            }
        }

        AND_GIVEN("and leads three lord without armies")
        {
            TMEStep::LordHasFollowers(lord, { TMEStep::ch_rorthron, TMEStep::ch_luxor, "CH_TARITHEL" });

            WHEN("lord fights thing")
            {
                ShouldFightAndWillLose(lord);

                THEN("the lord will be killed in hard and normal")
                {
                    if ( mode == DF_HARD ) {
                        LordDead(lord);
                    }else{
                        ThingKilled();
                    }
                }
            }
        }
    }
}


SCENARIO("Lord fights a nasty with no object but armies")
{
    LPCSTR mode_text[] = { "NORMAL", "EASY", "MEDIUM", "HARD",  };
    auto mode = GENERATE( DF_NORMAL, DF_EASY, DF_MEDIUM, DF_HARD );

    GIVEN(StringExtensions::Format("Difficulty is %s", mode_text[mode]))
    {
        TMEStep::NewStory(RF_DEFAULT, mode);

        auto lord = LordNeedstoFightNasty( TMEStep::ch_morkin, OB_DRAGONS );
        ThingHasZeroKillRate();

        AND_GIVEN("and has 249 soldiers")
        {
            GetCharacter(lord)->warriors.Total(100);
            GetCharacter(lord)->riders.Total(149);

            WHEN("lord fights thing")
            {
                ShouldFightAndWillLose(lord);

                THEN("the lord will be killed")
                {
                    LordDead(lord);
                }
            }
        }
        
        AND_GIVEN("and has 250 soldiers")
        {
            GetCharacter(lord)->warriors.Total(101);
            GetCharacter(lord)->riders.Total(149);

            WHEN("lord fights thing")
            {
                ShouldFightAndWillLose(lord);

                THEN("the thing will be killed in easy")
                {
                    if ( mode == DF_EASY ) {
                        ThingKilled();
                    }else{
                        LordDead(lord);
                    }
                }
            }
        }
        
        AND_GIVEN("and has 749 soldiers")
        {
            GetCharacter(lord)->warriors.Total(600);
            GetCharacter(lord)->riders.Total(149);

            WHEN("lord fights thing")
            {
                ShouldFightAndWillLose(lord);

                THEN("the thing will be killed in easy")
                {
                    if ( mode == DF_EASY ) {
                        ThingKilled();
                    }else{
                        LordDead(lord);
                    }
                }
            }
        }
        
        AND_GIVEN("and has 750 soldiers")
        {
            GetCharacter(lord)->warriors.Total(601);
            GetCharacter(lord)->riders.Total(149);

            WHEN("lord fights thing")
            {
                ShouldFightAndWillLose(lord);

                THEN("the thing will be killed in easy and medium")
                {
                    if ( mode == DF_EASY || mode == DF_MEDIUM) {
                        ThingKilled();
                    }else{
                        LordDead(lord);
                    }
                }
            }
        }
    }
}



