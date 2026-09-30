//
//  map_layers_tests.cpp
//  citadel
//
#include "../../steps/tme_steps.h"

SCENARIO("The Mists of Oblivion ring the Bloodmarch, and no lord walks into them")
{
    TMEStep::NewStory();

    THEN("the four squares round the edge of the map are mist, and the fifth is not")
    {
        REQUIRE( tme::mx->gamemap->GetAt(mxgridref(0,0)).terrain == TN_MIST );
        REQUIRE( tme::mx->gamemap->GetAt(mxgridref(3,100)).terrain == TN_MIST );
        REQUIRE( tme::mx->gamemap->GetAt(mxgridref(252,252)).terrain == TN_MIST );
        REQUIRE( tme::mx->gamemap->GetAt(mxgridref(4,100)).terrain != TN_MIST );
    }

    GIVEN("a lord at the edge of the Bloodmarch")
    {
        auto lord = GetCharacter("CH_RORTHRON");
        lord->Location(mxgridref(4,100));
        lord->time = (mxtime_t)tme::variables::sv_time_dawn;
        lord->energy = 127;

        WHEN("he looks into the mist")
        {
            lord->looking = DR_WEST;
            auto info = lord->GetLocInfo();

            THEN("he cannot go on")
            {
                REQUIRE_FALSE( info->flags.Is(lif_moveforward) );
                REQUIRE( info->flags.Is(lif_blocked) );
            }
        }

        WHEN("he turns back")
        {
            lord->looking = DR_EAST;
            auto info = lord->GetLocInfo();

            THEN("he can")
            {
                REQUIRE( info->flags.Is(lif_moveforward) );
            }
        }
    }
}
