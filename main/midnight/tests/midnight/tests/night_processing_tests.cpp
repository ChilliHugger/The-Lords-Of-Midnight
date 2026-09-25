//
//  night_processing_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(1, 1);

}


SCENARIO("A dead lord is untouched by night processing")
{
    TMEStep::NewStory();

    GIVEN("a dead lord with battle-worn state")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->time = 5;
        lord->energy = 10;
        lord->Flags().Set(cf_resting|cf_inbattle);
        TMEStep::LordIsDead(TMEStep::ch_morkin);

        WHEN("night processing runs")
        {
            lord->InitNightProcessing();

            THEN("their state is untouched")
            {
                REQUIRE( lord->time == 5 );
                REQUIRE( lord->energy == 10 );
                REQUIRE( lord->Flags().Is(cf_resting) );
                REQUIRE( lord->Flags().Is(cf_inbattle) );
            }
        }
    }
}

SCENARIO("Night processing resets a lord's time to dawn")
{
    TMEStep::NewStory();

    GIVEN("a lord partway through their day")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->time = 5;

        WHEN("night processing runs")
        {
            lord->InitNightProcessing();

            THEN("their time resets to dawn")
            {
                REQUIRE( lord->time == (mxtime_t) sv_time_dawn );
            }
        }
    }
}

SCENARIO("Night processing restores energy based on remaining time and the lord's race")
{
    TMEStep::NewStory();

    GIVEN("a weary lord with some time left in the day")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Reset(cf_resting);
        lord->time = 20;
        lord->energy = 10;

        WHEN("night processing runs")
        {
            lord->InitNightProcessing();

            THEN("energy rises by half the remaining time, plus the race's base rest amount")
            {
                auto baseRest = tme::mx->RaceById(lord->Race())->BaseRestAmount();
                u32 expected = std::min<u32>(10 + 20/2 + baseRest, (u32) sv_character_max_energy);
                REQUIRE( lord->energy == expected );
            }
        }
    }
}

SCENARIO("Night processing clears battle-related flags and counters")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying the marks of the previous day's battle")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Set(cf_resting|cf_inbattle|cf_wonbattle|cf_battleover);
        lord->battleloc = mxgridref(3, 3);
        lord->battleslew = 5;
        lord->riders.Lost(2);
        lord->riders.Killed(3);
        lord->warriors.Lost(2);
        lord->warriors.Killed(3);

        WHEN("night processing runs")
        {
            lord->InitNightProcessing();

            THEN("the battle flags and counters are all cleared")
            {
                REQUIRE_FALSE( lord->Flags().Is(cf_resting) );
                REQUIRE_FALSE( lord->Flags().Is(cf_inbattle) );
                REQUIRE_FALSE( lord->Flags().Is(cf_wonbattle) );
                REQUIRE_FALSE( lord->Flags().Is(cf_battleover) );
                REQUIRE( lord->battleloc == mxgridref(-1, -1) );
                REQUIRE( lord->battleslew == 0 );
                REQUIRE( lord->riders.Lost() == 0 );
                REQUIRE( lord->riders.Killed() == 0 );
                REQUIRE( lord->warriors.Lost() == 0 );
                REQUIRE( lord->warriors.Killed() == 0 );
            }
        }
    }
}

SCENARIO("Night processing ends an overnight wait")
{
    TMEStep::NewStory();

    GIVEN("a lord waiting overnight")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->wait = WM_OVERNIGHT;

        WHEN("night processing runs")
        {
            lord->InitNightProcessing();

            THEN("they are no longer waiting")
            {
                REQUIRE( lord->WaitMode() == WM_NONE );
            }
        }
    }
}
