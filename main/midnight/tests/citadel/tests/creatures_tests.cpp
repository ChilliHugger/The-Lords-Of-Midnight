//
//  creatures_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"

static int CreaturesOnMap( mxthing_t thing )
{
    int count = 0;
    for ( auto [loc, sqr] : tme::mx->gamemap->Locations() )
        if ( sqr.object == thing )
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

    THEN("the wolves must be fought, and the dragons are only seen")
    {
        REQUIRE( GetObject("OB_WOLVES")->CanFight() );
        REQUIRE_FALSE( GetObject("OB_DRAGONS")->CanFight() );
    }
}
