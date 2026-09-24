//
//  recruit_men_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(1, 1);

}


SCENARIO("Cmd_RecruitMen fails when there is no stronghold to recruit from")
{
    TMEStep::NewStory();

    GIVEN("a lord standing on open ground")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);

        WHEN("they try to recruit men")
        {
            auto result = lord->Cmd_RecruitMen(nullptr, 0);

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("Cmd_RecruitMen fails when the stronghold is already at its minimum troops")
{
    TMEStep::NewStory();

    GIVEN("a stronghold with no troops to spare")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        TMEStep::StrongholdAtLocation(k_here, UT_WARRIORS, RA_FREE, 100, 100, 1200);

        WHEN("they try to recruit men")
        {
            auto result = lord->Cmd_RecruitMen(nullptr, 0);

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("Cmd_RecruitMen recruits warriors from a friendly stronghold")
{
    TMEStep::NewStory();

    GIVEN("a friendly warrior stronghold with troops to spare")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->warriors.Total(0);
        auto stronghold = TMEStep::StrongholdAtLocation(k_here, UT_WARRIORS, RA_FREE, 500, 50, 1200);

        WHEN("they recruit men")
        {
            auto result = lord->Cmd_RecruitMen(nullptr, 0);

            THEN("they gain the standard recruit amount, taken from the stronghold")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->warriors.Total() == (u32) sv_character_recruit_amount );
                REQUIRE( stronghold->TotalTroops() == 500 - (u32) sv_character_recruit_amount );
            }
        }
    }
}

SCENARIO("Cmd_RecruitMen recruits riders from a friendly stronghold")
{
    TMEStep::NewStory();

    GIVEN("a friendly rider stronghold with troops to spare")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->riders.Total(0);
        auto stronghold = TMEStep::StrongholdAtLocation(k_here, UT_RIDERS, RA_FREE, 500, 50, 1200);

        WHEN("they recruit men")
        {
            auto result = lord->Cmd_RecruitMen(nullptr, 0);

            THEN("they gain the standard recruit amount, taken from the stronghold")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->riders.Total() == (u32) sv_character_recruit_amount );
                REQUIRE( stronghold->TotalTroops() == 500 - (u32) sv_character_recruit_amount );
            }
        }
    }
}

SCENARIO("Cmd_RecruitMen cannot take a lord beyond their maximum warriors")
{
    TMEStep::NewStory();

    GIVEN("a lord with less room to spare than they are requesting, but at least the standard recruit amount")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        u32 roomToSpare = (u32) sv_character_recruit_amount + 50;
        u32 nearMax = (u32) sv_character_max_warriors - roomToSpare;
        lord->warriors.Total(nearMax);
        auto stronghold = TMEStep::StrongholdAtLocation(k_here, UT_WARRIORS, RA_FREE, 500, 50, 1200);

        WHEN("they request far more than they have room for")
        {
            auto result = lord->Cmd_RecruitMen(nullptr, 500);

            THEN("they are topped up to exactly their maximum, no further")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->warriors.Total() == (u32) sv_character_max_warriors );
                REQUIRE( stronghold->TotalTroops() == 500 - roomToSpare );
            }
        }
    }
}

SCENARIO("Cmd_PostMen fails when there is no stronghold to post to")
{
    TMEStep::NewStory();

    GIVEN("a lord standing on open ground, carrying warriors")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->warriors.Total(200);

        WHEN("they try to post men")
        {
            auto result = lord->Cmd_PostMen(nullptr, 0);

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("Cmd_PostMen fails when the stronghold has no room to spare")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying warriors, and a stronghold with no room for more")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->warriors.Total(200);
        TMEStep::StrongholdAtLocation(k_here, UT_WARRIORS, RA_FREE, 950, 50, 1000);

        WHEN("they try to post men")
        {
            auto result = lord->Cmd_PostMen(nullptr, 0);

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("Cmd_PostMen deposits warriors into a friendly stronghold")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying warriors, and a stronghold with room to spare")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->warriors.Total(200);
        auto stronghold = TMEStep::StrongholdAtLocation(k_here, UT_WARRIORS, RA_FREE, 500, 50, 1200);

        WHEN("they post men")
        {
            auto result = lord->Cmd_PostMen(nullptr, 0);

            THEN("the standard guard amount moves from the lord to the stronghold")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->warriors.Total() == 200 - (u32) sv_character_guard_amount );
                REQUIRE( stronghold->TotalTroops() == 500 + (u32) sv_character_guard_amount );
            }
        }
    }
}

SCENARIO("Cmd_PostMen deposits riders into a friendly stronghold")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying riders, and a stronghold with room to spare")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->riders.Total(200);
        auto stronghold = TMEStep::StrongholdAtLocation(k_here, UT_RIDERS, RA_FREE, 500, 50, 1200);

        WHEN("they post men")
        {
            auto result = lord->Cmd_PostMen(nullptr, 0);

            THEN("the standard guard amount moves from the lord to the stronghold")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->riders.Total() == 200 - (u32) sv_character_guard_amount );
                REQUIRE( stronghold->TotalTroops() == 500 + (u32) sv_character_guard_amount );
            }
        }
    }
}
