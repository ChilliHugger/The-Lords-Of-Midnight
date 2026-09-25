//
//  attack_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here  = mxgridref(1, 1);
    const mxgridref k_south = mxgridref(1, 2);

}


SCENARIO("A lord attacks an enemy army ahead of them")
{
    TMEStep::NewStory();

    GIVEN("an enemy regiment occupies the location ahead")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::ResetLocation(k_south);
        TMEStep::RegimentAtLocation(k_south, 10);

        WHEN("the lord attacks")
        {
            auto result = lord->Cmd_Attack();

            THEN("they walk into the enemy's location and enter battle")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->Location() == k_south );
                REQUIRE( lord->Flags().Is(cf_inbattle) );
            }
        }
    }
}

SCENARIO("A lord cannot attack when there is nothing to fight ahead")
{
    TMEStep::NewStory();

    GIVEN("the location ahead is empty")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::ResetLocation(k_south);

        WHEN("the lord attacks")
        {
            auto result = lord->Cmd_Attack();

            THEN("the attack fails and they stay put")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( lord->Location() == k_here );
                REQUIRE_FALSE( lord->Flags().Is(cf_inbattle) );
            }
        }
    }
}

SCENARIO("A lord who is following someone else cannot attack on their own")
{
    TMEStep::NewStory();

    GIVEN("a lord who is following another lord, with an enemy ahead")
    {
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::ResetLocation(k_south);
        TMEStep::RegimentAtLocation(k_south, 10);

        lord->following = leader;

        WHEN("the follower attacks")
        {
            auto result = lord->Cmd_Attack();

            THEN("the attack fails")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( lord->Location() == k_here );
            }
        }
    }
}
