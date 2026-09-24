//
//  combat_success_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    // Mirrors mxwarriors::BattleSuccess (units.cpp) - one place to update if
    // that formula changes.
    s32 ExpectedWarriorsSuccess(s32 adjustment, s32 energy)
    {
        return (adjustment + energy) / 2 + (s32) sv_success_base_level;
    }

    // Mirrors mxriders::BattleSuccess (units.cpp) - one place to update if
    // that formula changes.
    s32 ExpectedRidersSuccess(s32 adjustment, s32 energy, mxterrain_t terrain, bool feyOnHorseBonus)
    {
        s32 energyadj = (terrain == TN_MOUNTAIN)
            ? (s32) sv_success_riders_energy_mountain
            : (s32) sv_success_riders_energy_other;

        s32 success = adjustment + energy + energyadj;

        if ( feyOnHorseBonus )
            success += (s32) sv_success_fey_on_horse_adjuster;

        return success / 2 + (s32) sv_success_base_level;
    }

}


SCENARIO("Warriors' battle success combines location adjustment, unit energy, and the base success level")
{
    TMEStep::NewStory();

    GIVEN("a lord's warriors, and a location with a known adjustment")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);
        lord->warriors.Energy(20);

        mxlocinfo locinfo;
        locinfo.friends.adjustment = 10;

        WHEN("their battle success is calculated")
        {
            auto success = lord->warriors.BattleSuccess(locinfo, lord);

            THEN("it matches the warriors' success formula")
            {
                REQUIRE( success == ExpectedWarriorsSuccess(10, 20) );
            }
        }
    }
}

SCENARIO("Riders' battle success uses the mountain energy bonus on mountainous terrain")
{
    TMEStep::NewStory();

    GIVEN("a lord's riders, on a mountain")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);
        lord->riders.Energy(20);

        mxlocinfo locinfo;
        locinfo.friends.adjustment = 10;
        locinfo.mapsqr.terrain = TN_MOUNTAIN;

        WHEN("their battle success is calculated")
        {
            auto success = lord->riders.BattleSuccess(locinfo, lord);

            THEN("it matches the riders' success formula, with the mountain bonus")
            {
                REQUIRE( success == ExpectedRidersSuccess(10, 20, TN_MOUNTAIN, false) );
            }
        }
    }
}

SCENARIO("Riders' battle success uses the standard energy bonus off mountainous terrain")
{
    TMEStep::NewStory();

    GIVEN("a lord's riders, on the plains")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);
        lord->riders.Energy(20);

        mxlocinfo locinfo;
        locinfo.friends.adjustment = 10;
        locinfo.mapsqr.terrain = TN_PLAINS;

        WHEN("their battle success is calculated")
        {
            auto success = lord->riders.BattleSuccess(locinfo, lord);

            THEN("it matches the riders' success formula, with the standard bonus")
            {
                REQUIRE( success == ExpectedRidersSuccess(10, 20, TN_PLAINS, false) );
            }
        }
    }
}

SCENARIO("A mounted Fey rider gets an extra bonus fighting in a forest")
{
    TMEStep::NewStory();

    GIVEN("a mounted Fey lord's riders, fighting in a forest")
    {
        auto lord = GetCharacter(LOMStep::ch_dreams);
        REQUIRE( lord->Race() == RA_FEY );
        lord->Flags().Set(cf_riding);
        lord->riders.Energy(20);

        mxlocinfo locinfo;
        locinfo.friends.adjustment = 10;
        locinfo.mapsqr.terrain = TN_FOREST;

        WHEN("their battle success is calculated")
        {
            auto success = lord->riders.BattleSuccess(locinfo, lord);

            THEN("it includes the Fey-on-horseback bonus")
            {
                REQUIRE( success == ExpectedRidersSuccess(10, 20, TN_FOREST, true) );
            }
        }
    }
}

SCENARIO("A dismounted Fey lord gets no riding bonus, even in a forest")
{
    TMEStep::NewStory();

    GIVEN("a dismounted Fey lord's riders, fighting in a forest")
    {
        auto lord = GetCharacter(LOMStep::ch_dreams);
        REQUIRE( lord->Race() == RA_FEY );
        lord->Flags().Reset(cf_riding);
        lord->riders.Energy(20);

        mxlocinfo locinfo;
        locinfo.friends.adjustment = 10;
        locinfo.mapsqr.terrain = TN_FOREST;

        WHEN("their battle success is calculated")
        {
            auto success = lord->riders.BattleSuccess(locinfo, lord);

            THEN("no Fey-on-horseback bonus is applied")
            {
                REQUIRE( success == ExpectedRidersSuccess(10, 20, TN_FOREST, false) );
            }
        }
    }
}

SCENARIO("A non-Fey rider gets no Fey bonus, even mounted in a forest")
{
    TMEStep::NewStory();

    GIVEN("a mounted, non-Fey lord's riders, fighting in a forest")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);
        REQUIRE_FALSE( lord->Race() == RA_FEY );
        lord->Flags().Set(cf_riding);
        lord->riders.Energy(20);

        mxlocinfo locinfo;
        locinfo.friends.adjustment = 10;
        locinfo.mapsqr.terrain = TN_FOREST;

        WHEN("their battle success is calculated")
        {
            auto success = lord->riders.BattleSuccess(locinfo, lord);

            THEN("no Fey-on-horseback bonus is applied")
            {
                REQUIRE( success == ExpectedRidersSuccess(10, 20, TN_FOREST, false) );
            }
        }
    }
}
