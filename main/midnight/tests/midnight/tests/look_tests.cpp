//
//  look_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(1, 1);

}


SCENARIO("A lord can turn to look in a specific direction")
{
    TMEStep::NewStory();

    GIVEN("a lord looking north")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_NORTH);

        WHEN("they look south-east")
        {
            auto result = lord->Cmd_LookDir(DR_SOUTHEAST);

            THEN("they are now looking south-east")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->Looking() == DR_SOUTHEAST );
            }
        }
    }
}

SCENARIO("A lord can turn left, wrapping around from north to north-west")
{
    TMEStep::NewStory();

    GIVEN("a lord looking north")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_NORTH);

        WHEN("they look left")
        {
            auto result = lord->Cmd_LookLeft();

            THEN("they are now looking north-west")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->Looking() == DR_NORTHWEST );
            }
        }
    }
}

SCENARIO("A lord can turn right, wrapping around from north-west to north")
{
    TMEStep::NewStory();

    GIVEN("a lord looking north-west")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_NORTHWEST);

        WHEN("they look right")
        {
            auto result = lord->Cmd_LookRight();

            THEN("they are now looking north")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->Looking() == DR_NORTH );
            }
        }
    }
}

SCENARIO("A lord can look towards another location")
{
    TMEStep::NewStory();

    GIVEN("a lord looking north, with a location to their south-east")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_NORTH);
        mxgridref target = k_here;
        target += DR_SOUTHEAST;

        WHEN("they look at that location")
        {
            auto result = lord->Cmd_Lookat(target);

            THEN("they turn to face it")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->Looking() == DR_SOUTHEAST );
            }
        }
    }
}
