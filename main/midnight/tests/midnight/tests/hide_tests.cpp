//
//  hide_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(1, 1);

    // Morkin has no HIDE flag in the TSV data; Luxor does.
    s32 TimeSpentHiding(mxdifficulty_t difficulty)
    {
        TMEStep::NewStory(RF_DEFAULT, difficulty);
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        mxtime_t before = lord->time;

        REQUIRE( lord->Cmd_Hide() == MX_OK );

        return (s32)(before - lord->time);
    }

    s32 TimeSpentUnhiding(mxdifficulty_t difficulty)
    {
        TMEStep::NewStory(RF_DEFAULT, difficulty);
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->Flags().Set(cf_hidden);
        mxtime_t before = lord->time;

        REQUIRE( lord->Cmd_UnHide() == MX_OK );

        return (s32)(before - lord->time);
    }

}


SCENARIO("A lord who is not allowed to hide cannot hide")
{
    TMEStep::NewStory();

    GIVEN("a lord who is not permitted to hide")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        REQUIRE_FALSE( lord->IsAllowedHide() );

        WHEN("they try to hide")
        {
            auto result = lord->Cmd_Hide();

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE_FALSE( lord->IsHidden() );
            }
        }
    }
}

SCENARIO("A lord who is allowed to hide can hide")
{
    TMEStep::NewStory();

    GIVEN("a lord who is permitted to hide")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        REQUIRE( lord->IsAllowedHide() );

        WHEN("they hide")
        {
            auto result = lord->Cmd_Hide();

            THEN("they become hidden")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->IsHidden() );
            }
        }
    }
}

SCENARIO("A lord who is already hidden cannot hide again")
{
    TMEStep::NewStory();

    GIVEN("a lord who is already hidden")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->Flags().Set(cf_hidden);

        WHEN("they try to hide")
        {
            auto result = lord->Cmd_Hide();

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("Hiding on medium difficulty costs extra time over normal difficulty")
{
    GIVEN("the same lord hiding on normal and on medium difficulty")
    {
        WHEN("they hide on each difficulty")
        {
            auto normalCost = TimeSpentHiding(DF_NORMAL);
            auto mediumCost = TimeSpentHiding(DF_MEDIUM);

            THEN("medium difficulty costs exactly the time scale more")
            {
                REQUIRE( (mediumCost - normalCost) == (s32) sv_time_scale );
            }
        }
    }
}

SCENARIO("Hiding on hard difficulty sends the lord straight to night")
{
    TMEStep::NewStory(RF_DEFAULT, DF_HARD);

    GIVEN("a lord hiding on hard difficulty")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);

        WHEN("they hide")
        {
            auto result = lord->Cmd_Hide();

            THEN("their time jumps straight to night")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->time == (mxtime_t) sv_time_night );
            }
        }
    }
}

SCENARIO("Cmd_UnHide fails when the lord is not hidden")
{
    TMEStep::NewStory();

    GIVEN("a lord who is not hidden")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);

        WHEN("they try to unhide")
        {
            auto result = lord->Cmd_UnHide();

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("A hidden lord can unhide")
{
    TMEStep::NewStory();

    GIVEN("a hidden lord")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->Flags().Set(cf_hidden);

        WHEN("they unhide")
        {
            auto result = lord->Cmd_UnHide();

            THEN("they are no longer hidden")
            {
                REQUIRE( result == MX_OK );
                REQUIRE_FALSE( lord->IsHidden() );
            }
        }
    }
}

SCENARIO("Unhiding above easy difficulty costs extra time over easy difficulty")
{
    GIVEN("the same lord unhiding on easy and on medium difficulty")
    {
        WHEN("they unhide on each difficulty")
        {
            // DF_NORMAL=0, DF_EASY=1, DF_MEDIUM=2, DF_HARD=3 - the ">DF_EASY"
            // rule in Cmd_UnHide only kicks in from medium difficulty up.
            auto easyCost = TimeSpentUnhiding(DF_EASY);
            auto mediumCost = TimeSpentUnhiding(DF_MEDIUM);

            THEN("medium difficulty costs exactly the time scale more")
            {
                REQUIRE( (mediumCost - easyCost) == (s32) sv_time_scale );
            }
        }
    }
}
