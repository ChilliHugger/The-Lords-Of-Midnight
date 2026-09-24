//
//  dismount_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(1, 1);

}


SCENARIO("A lord who is not riding cannot dismount")
{
    TMEStep::NewStory();

    GIVEN("a lord who is on foot")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Reset(cf_riding);

        WHEN("they try to dismount")
        {
            auto result = lord->Cmd_Dismount();

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("A lord underground cannot dismount")
{
    TMEStep::NewStory();

    GIVEN("a riding lord who is underground")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Set(cf_riding);
        lord->Flags().Set(cf_tunnel);

        WHEN("they try to dismount")
        {
            auto result = lord->Cmd_Dismount();

            THEN("the attempt fails and they remain mounted")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( lord->IsRiding() );
            }
        }
    }
}

SCENARIO("A riding lord can dismount")
{
    TMEStep::NewStory();

    GIVEN("a riding lord")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Set(cf_riding);

        WHEN("they dismount")
        {
            auto result = lord->Cmd_Dismount();

            THEN("they are no longer riding")
            {
                REQUIRE( result == MX_OK );
                REQUIRE_FALSE( lord->IsRiding() );
            }
        }
    }
}

SCENARIO("Losing a fight can dismount a riding lord")
{
    TMEStep::NewStory();

    GIVEN("a riding lord who loses a fight and should lose their horse, but should not die")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Set(cf_riding);
        TMEStep::LordShouldLoseHorse(TMEStep::ch_morkin);
        TMEStep::LordShouldNotDieInFight(TMEStep::ch_morkin);

        WHEN("they lose the fight")
        {
            lord->LostFight();

            THEN("they are dismounted, but survive")
            {
                REQUIRE_FALSE( lord->IsRiding() );
                REQUIRE_FALSE( lord->IsDead() );
            }
        }
    }
}

SCENARIO("Losing a fight does not always dismount a riding lord")
{
    TMEStep::NewStory();

    GIVEN("a riding lord who loses a fight but should not lose their horse or die")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Set(cf_riding);
        TMEStep::LordShouldNotLoseHorse(TMEStep::ch_morkin);
        TMEStep::LordShouldNotDieInFight(TMEStep::ch_morkin);

        WHEN("they lose the fight")
        {
            lord->LostFight();

            THEN("they remain mounted")
            {
                REQUIRE( lord->IsRiding() );
            }
        }
    }
}

SCENARIO("Losing a fight can kill a lord regardless of their horse")
{
    TMEStep::NewStory();

    GIVEN("a lord who loses a fight and should die")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Reset(cf_riding);
        TMEStep::LordShouldDieInFight(TMEStep::ch_morkin);

        WHEN("they lose the fight")
        {
            lord->LostFight();

            THEN("they die")
            {
                REQUIRE( lord->IsDead() );
            }
        }
    }
}

SCENARIO("A dead lord does not lose their horse or die again from losing a fight")
{
    TMEStep::NewStory();

    GIVEN("a dead, riding lord who should lose their horse and should die")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Set(cf_riding);
        TMEStep::LordShouldLoseHorse(TMEStep::ch_morkin);
        TMEStep::LordShouldDieInFight(TMEStep::ch_morkin);
        TMEStep::LordIsDead(TMEStep::ch_morkin);

        WHEN("they lose a fight")
        {
            lord->LostFight();

            THEN("nothing further happens to them")
            {
                REQUIRE( lord->IsRiding() );
                REQUIRE( lord->IsDead() );
            }
        }
    }
}

SCENARIO("ShouldDieInFight compares a random roll against the lord's energy-based threshold")
{
    TMEStep::NewStory();

    GIVEN("a lord whose threshold (energy/2 - 64 + reckless) is 36")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->energy = 200;
        lord->reckless = 0;

        WHEN("the random roll is below the threshold")
        {
            ScopedRandom random({ 35 });
            auto result = lord->ShouldDieInFight();

            THEN("they do not die")
            {
                REQUIRE_FALSE( result );
            }
        }

        WHEN("the random roll equals the threshold")
        {
            ScopedRandom random({ 36 });
            auto result = lord->ShouldDieInFight();

            THEN("they die")
            {
                REQUIRE( result );
            }
        }

        WHEN("the random roll is above the threshold")
        {
            ScopedRandom random({ 100 });
            auto result = lord->ShouldDieInFight();

            THEN("they die")
            {
                REQUIRE( result );
            }
        }
    }
}

SCENARIO("ShouldLoseHorse is decided by the low bit of a random roll")
{
    TMEStep::NewStory();

    GIVEN("a lord in a fight")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);

        WHEN("the random roll is odd")
        {
            ScopedRandom random({ 1 });
            auto result = lord->ShouldLoseHorse();

            THEN("they lose their horse")
            {
                REQUIRE( result );
            }
        }

        WHEN("the random roll is even")
        {
            ScopedRandom random({ 0 });
            auto result = lord->ShouldLoseHorse();

            THEN("they keep their horse")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}
