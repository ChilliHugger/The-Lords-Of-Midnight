//
//  creatures_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"

static int CreaturesOnMap( mxthing_t thing )
{
    auto size = tme::mx->gamemap->Size();
    int count = 0;
    for ( int y = 0; y < size.cy; y++ )
        for ( int x = 0; x < size.cx; x++ )
            if ( tme::mx->gamemap->GetAt(mxgridref(x,y)).object == thing )
                count++;
    return count;
}

SCENARIO("The 1995 wolves and dragons are on the map, and only the wolves must be fought")
{
    TMEStep::NewStory();

    THEN("the creatures stand where the 1995 game placed them")
    {
        REQUIRE( CreaturesOnMap(OB_WOLVES) == 94 );
        REQUIRE( CreaturesOnMap(OB_DRAGONS) == 29 );
        REQUIRE( CreaturesOnMap(OB_ICETROLLS) == 0 );
        REQUIRE( tme::mx->gamemap->GetAt(mxgridref(26,9)).object == OB_WOLVES );
        REQUIRE( tme::mx->gamemap->GetAt(mxgridref(238,13)).object == OB_DRAGONS );
    }

    GIVEN("a lord at dawn")
    {
        auto lord = GetCharacter("CH_RORTHRON");
        lord->time = (mxtime_t)tme::variables::sv_time_dawn;
        lord->energy = 127;
        lord->looking = DR_NORTH;

        WHEN("he comes upon wolves")
        {
            lord->Location(mxgridref(26,9));
            auto info = lord->GetLocInfo();

            THEN("he must fight them before he can go on")
            {
                REQUIRE( info->flags.Is(lif_fight) );
                REQUIRE( info->fightthing == OB_WOLVES );
                REQUIRE( info->flags.Is(lif_moveforward) );
                REQUIRE( lord->Cmd_MoveForward() == MX_FAILED );
            }
        }

        WHEN("he comes upon dragons")
        {
            lord->Location(mxgridref(238,13));
            auto info = lord->GetLocInfo();

            THEN("he sees them, and they let him pass")
            {
                REQUIRE_FALSE( info->flags.Is(lif_fight) );
                REQUIRE( lord->LocationThing() == OB_DRAGONS );
                REQUIRE( lord->Cmd_MoveForward() == MX_OK );
            }
        }
    }
}
