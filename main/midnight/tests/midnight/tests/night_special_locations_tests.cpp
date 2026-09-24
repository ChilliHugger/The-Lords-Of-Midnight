//
//  night_special_locations_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(1, 1);

    // Exposes mxnight's protected internals for direct testing - only
    // Process() and MoveMidwinter() are public.
    class TestableNight : public mxnight
    {
    public:
        using mxnight::SetSpecialLocationsCharacter;
        using mxnight::SetSpecialLocationsStrongholds;
        using mxnight::ResetLocationSpecial;
        using mxnight::ResetSpecialLocations;
    };

}


SCENARIO("A living, visible, non-Midwinter lord takes part in battle")
{
    TMEStep::NewStory();

    GIVEN("an ordinary living lord")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);

        THEN("they take part in battle")
        {
            REQUIRE( lord->TakesPartInBattle() );
        }
    }
}

SCENARIO("A hidden lord does not take part in battle")
{
    TMEStep::NewStory();

    GIVEN("a hidden lord")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);
        lord->Flags().Set(cf_hidden);

        THEN("they do not take part in battle")
        {
            REQUIRE_FALSE( lord->TakesPartInBattle() );
        }
    }
}

SCENARIO("A dead lord does not take part in battle")
{
    TMEStep::NewStory();

    GIVEN("a dead lord")
    {
        auto lord = GetCharacter(TMEStep::ch_morkin);
        TMEStep::LordIsDead(TMEStep::ch_morkin);

        THEN("they do not take part in battle")
        {
            REQUIRE_FALSE( lord->TakesPartInBattle() );
        }
    }
}

SCENARIO("The Lord of Midwinter does not take part in battle")
{
    TMEStep::NewStory();

    GIVEN("the Lord of Midwinter, alive and visible")
    {
        auto midwinter = GetCharacter("CH_MIDWINTER");
        REQUIRE( midwinter->IsAlive() );
        REQUIRE_FALSE( midwinter->IsHidden() );

        THEN("he still does not take part in battle")
        {
            REQUIRE_FALSE( midwinter->TakesPartInBattle() );
        }
    }
}

SCENARIO("Setting special locations marks where lords who take part in battle stand")
{
    TMEStep::NewStory();

    GIVEN("a lord who takes part in battle")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        REQUIRE( lord->TakesPartInBattle() );

        WHEN("special locations are marked")
        {
            TestableNight().SetSpecialLocationsCharacter();

            THEN("their location is marked special")
            {
                REQUIRE( tme::mx->gamemap->IsLocationSpecial(k_here) );
            }
        }
    }
}

SCENARIO("Setting special locations does not mark where a hidden lord stands")
{
    TMEStep::NewStory();

    GIVEN("a hidden lord, who does not take part in battle")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Set(cf_hidden);
        REQUIRE_FALSE( lord->TakesPartInBattle() );

        WHEN("special locations are marked")
        {
            TestableNight().SetSpecialLocationsCharacter();

            THEN("their location is not marked special")
            {
                REQUIRE_FALSE( tme::mx->gamemap->IsLocationSpecial(k_here) );
            }
        }
    }
}

SCENARIO("Setting special locations marks a friendly stronghold's location")
{
    TMEStep::NewStory();

    GIVEN("a friendly stronghold")
    {
        TMEStep::StrongholdAtLocation(k_here, UT_WARRIORS, RA_FREE, 500, 100, 1200);

        WHEN("special locations are marked")
        {
            TestableNight().SetSpecialLocationsStrongholds();

            THEN("its location is marked special")
            {
                REQUIRE( tme::mx->gamemap->IsLocationSpecial(k_here) );
            }
        }
    }
}

SCENARIO("Setting special locations does not mark the enemy's own strongholds")
{
    TMEStep::NewStory();

    GIVEN("an enemy-occupied stronghold")
    {
        TMEStep::StrongholdAtLocation(k_here, UT_WARRIORS, RA_ENEMY, 500, 100, 1200);

        WHEN("special locations are marked")
        {
            TestableNight().SetSpecialLocationsStrongholds();

            THEN("its location is not marked special - it is already the enemy's")
            {
                REQUIRE_FALSE( tme::mx->gamemap->IsLocationSpecial(k_here) );
            }
        }
    }
}

SCENARIO("Resetting a special location clears its mark")
{
    TMEStep::NewStory();

    GIVEN("a location marked as special")
    {
        MapStep::ResetLocation(k_here);
        tme::mx->gamemap->SetLocationSpecial(k_here, 1);
        REQUIRE( tme::mx->gamemap->IsLocationSpecial(k_here) );

        WHEN("it is reset")
        {
            TestableNight().ResetLocationSpecial(k_here);

            THEN("it is no longer marked special")
            {
                REQUIRE_FALSE( tme::mx->gamemap->IsLocationSpecial(k_here) );
            }
        }
    }
}

SCENARIO("Resetting all special locations clears the mark at every lord's location")
{
    TMEStep::NewStory();

    GIVEN("a lord standing on a location marked special")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        tme::mx->gamemap->SetLocationSpecial(k_here, 1);

        WHEN("all special locations are reset")
        {
            TestableNight().ResetSpecialLocations();

            THEN("the lord's location is no longer marked special")
            {
                REQUIRE_FALSE( tme::mx->gamemap->IsLocationSpecial(k_here) );
            }
        }
    }
}

SCENARIO("Moving the Lord of Midwinter relocates him to one of his fixed haunts")
{
    TMEStep::NewStory();

    GIVEN("a fresh night")
    {
        mxnight night;

        WHEN("the Lord of Midwinter moves")
        {
            night.MoveMidwinter();

            THEN("he ends up at one of his five fixed locations")
            {
                auto midwinter = GetCharacter("CH_MIDWINTER");
                bool atKnownHaunt = (midwinter->Location() == mxgridref(38, 18))
                    || (midwinter->Location() == mxgridref(41, 21))
                    || (midwinter->Location() == mxgridref(42, 15))
                    || (midwinter->Location() == mxgridref(46, 14))
                    || (midwinter->Location() == mxgridref(49, 13));
                REQUIRE( atKnownHaunt );
            }
        }
    }
}
