//
//  rest_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(1, 1);

}


SCENARIO("A lord can rest")
{
    TMEStep::NewStory();

    GIVEN("a lord who is awake and not resting")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);

        WHEN("they rest")
        {
            auto result = lord->Cmd_Rest();

            THEN("they become flagged as resting")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->IsResting() );
            }
        }
    }
}

SCENARIO("A lord who is already resting cannot rest again")
{
    TMEStep::NewStory();

    GIVEN("a lord who is already resting")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Set(cf_resting);

        WHEN("they try to rest")
        {
            auto result = lord->Cmd_Rest();

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("A lord cannot rest at night")
{
    TMEStep::NewStory();

    GIVEN("it is night for this lord")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->time = (mxtime_t) sv_time_night;

        WHEN("they try to rest")
        {
            auto result = lord->Cmd_Rest();

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE_FALSE( lord->IsResting() );
            }
        }
    }
}

SCENARIO("A leader's followers rest together when the leader rests")
{
    TMEStep::NewStory();

    GIVEN("a leader with a follower, both awake")
    {
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto follower = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        follower->Cmd_Follow(leader);

        WHEN("the leader rests")
        {
            auto result = leader->Cmd_Rest();

            THEN("the follower rests too")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( leader->IsResting() );
                REQUIRE( follower->IsResting() );
            }
        }
    }
}
