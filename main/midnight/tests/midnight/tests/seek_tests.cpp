//
//  seek_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(1, 1);

}


SCENARIO("Seeking with nothing here finds nothing")
{
    TMEStep::NewStory();

    GIVEN("a lord standing on an empty square")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);

        WHEN("they seek")
        {
            auto result = lord->Cmd_Seek();

            THEN("they find nothing")
            {
                REQUIRE( result == nullptr );
            }
        }
    }
}

SCENARIO("Seeking wild horses mounts a lord who is allowed a horse")
{
    TMEStep::NewStory();

    GIVEN("a lord who is allowed a horse, on foot, standing near wild horses")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Reset(cf_riding);
        REQUIRE( lord->IsAllowedHorse() );
        MapStep::SetObjectAtLocation(k_here, OB_WILDHORSES);

        WHEN("they seek")
        {
            lord->Cmd_Seek();

            THEN("they are now riding")
            {
                REQUIRE( lord->IsRiding() );
            }
        }
    }
}

SCENARIO("Seeking wild horses does nothing for a lord who is not allowed a horse")
{
    TMEStep::NewStory();

    GIVEN("a lord who is not allowed a horse, on foot, standing near wild horses")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Reset(cf_riding);
        lord->Flags().Reset(cf_horse);
        MapStep::SetObjectAtLocation(k_here, OB_WILDHORSES);

        WHEN("they seek")
        {
            lord->Cmd_Seek();

            THEN("they remain on foot")
            {
                REQUIRE_FALSE( lord->IsRiding() );
            }
        }
    }
}

SCENARIO("Seeking shelter restores some energy")
{
    TMEStep::NewStory();

    GIVEN("a weary lord standing near shelter")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->energy = 10;
        MapStep::SetObjectAtLocation(k_here, OB_SHELTER);

        WHEN("they seek")
        {
            u32 before = lord->energy;
            lord->Cmd_Seek();

            THEN("their energy increases")
            {
                REQUIRE( lord->energy > before );
            }

            THEN("the shelter is no longer there")
            {
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_NONE );
            }
        }
    }
}

SCENARIO("Seeking the shadows of death drains a lord's energy to a fixed level")
{
    TMEStep::NewStory();

    GIVEN("a lord standing near the shadows of death")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->energy = 200;
        MapStep::SetObjectAtLocation(k_here, OB_SHADOWSOFDEATH);

        WHEN("they seek")
        {
            lord->Cmd_Seek();

            THEN("their energy drops to the fixed shadows-of-death level")
            {
                REQUIRE( lord->energy == (u32) sv_object_energy_shadowsofdeath );
            }
        }
    }
}

SCENARIO("Seeking the waters of life restores a lord's energy to a fixed level")
{
    TMEStep::NewStory();

    GIVEN("a lord standing near the waters of life")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->energy = 1;
        MapStep::SetObjectAtLocation(k_here, OB_WATERSOFLIFE);

        WHEN("they seek")
        {
            lord->Cmd_Seek();

            THEN("their energy rises to the fixed waters-of-life level")
            {
                REQUIRE( lord->energy == (u32) sv_object_energy_watersoflife );
            }
        }
    }
}

SCENARIO("Seeking the hand of dark sends a lord straight to night")
{
    TMEStep::NewStory();

    GIVEN("a lord standing near the hand of dark")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::SetObjectAtLocation(k_here, OB_HANDOFDARK);

        WHEN("they seek")
        {
            lord->Cmd_Seek();

            THEN("their time jumps straight to night")
            {
                REQUIRE( lord->time == (mxtime_t) sv_time_night );
            }
        }
    }
}

SCENARIO("Seeking the cup of dreams sends a lord straight to dawn")
{
    TMEStep::NewStory();

    GIVEN("a lord standing near the cup of dreams")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->time = (mxtime_t) sv_time_night;
        MapStep::SetObjectAtLocation(k_here, OB_CUPOFDREAMS);

        WHEN("they seek")
        {
            lord->Cmd_Seek();

            THEN("their time jumps straight to dawn")
            {
                REQUIRE( lord->time == (mxtime_t) sv_time_dawn );
            }
        }
    }
}

SCENARIO("Seeking the ice crown picks it up when a lord is allowed to carry it")
{
    TMEStep::NewStory();

    GIVEN("a lord who is allowed the ice crown, standing near it")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        REQUIRE( lord->IsAllowedIcecrown() );
        TMEStep::LordNotCarryingObject(TMEStep::ch_morkin);
        MapStep::SetObjectAtLocation(k_here, OB_ICECROWN);

        WHEN("they seek")
        {
            lord->Cmd_Seek();

            THEN("they are now carrying the ice crown, which is no longer on the ground")
            {
                REQUIRE( lord->Carrying() == GetObject("OB_ICECROWN") );
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_NONE );
            }
        }
    }
}

SCENARIO("Seeking the ice crown fails when a lord is not allowed to carry it")
{
    TMEStep::NewStory();

    GIVEN("a lord who is not allowed the ice crown, standing near it")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        REQUIRE_FALSE( lord->IsAllowedIcecrown() );
        TMEStep::LordNotCarryingObject(TMEStep::ch_luxor);
        MapStep::SetObjectAtLocation(k_here, OB_ICECROWN);

        WHEN("they seek")
        {
            auto result = lord->Cmd_Seek();

            THEN("they find nothing, and the ice crown stays where it is")
            {
                REQUIRE( result == nullptr );
                REQUIRE( lord->Carrying() == nullptr );
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_ICECROWN );
            }
        }
    }
}

SCENARIO("Seeking the moon ring picks it up when a lord is allowed to carry it")
{
    TMEStep::NewStory();

    GIVEN("a lord who is allowed the moon ring, standing near it")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        REQUIRE( lord->IsAllowedMoonring() );
        TMEStep::LordNotCarryingObject(TMEStep::ch_luxor);
        MapStep::SetObjectAtLocation(k_here, OB_MOONRING);

        WHEN("they seek")
        {
            lord->Cmd_Seek();

            THEN("they are now carrying the moon ring, which is no longer on the ground")
            {
                REQUIRE( lord->Carrying() == GetObject("OB_MOONRING") );
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_NONE );
            }
        }
    }
}

SCENARIO("Seeking the moon ring fails when a lord is not allowed to carry it")
{
    TMEStep::NewStory();

    GIVEN("a lord who is not allowed the moon ring, standing near it")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_rorthron, k_here, DR_SOUTH);
        REQUIRE_FALSE( lord->IsAllowedMoonring() );
        TMEStep::LordNotCarryingObject(TMEStep::ch_rorthron);
        MapStep::SetObjectAtLocation(k_here, OB_MOONRING);

        WHEN("they seek")
        {
            auto result = lord->Cmd_Seek();

            THEN("they find nothing, and the moon ring stays where it is")
            {
                REQUIRE( result == nullptr );
                REQUIRE( lord->Carrying() == nullptr );
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_MOONRING );
            }
        }
    }
}

SCENARIO("Seeking an ordinary object picks it up")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying nothing, standing near a sword")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        TMEStep::LordNotCarryingObject(TMEStep::ch_morkin);
        MapStep::SetObjectAtLocation(k_here, OB_WOLFSLAYER);

        WHEN("they seek")
        {
            lord->Cmd_Seek();

            THEN("they pick up the sword, which is no longer on the ground")
            {
                REQUIRE( lord->Carrying() == GetObject("OB_WOLFSLAYER") );
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_NONE );
            }
        }
    }
}

SCENARIO("Seeking cannot pick up a new object if the current one cannot be dropped")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying an object they cannot drop, standing near a sword")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        TMEStep::LordCarryingObject(TMEStep::ch_morkin, OB_MOONRING);
        REQUIRE_FALSE( lord->Carrying()->CanDrop() );
        MapStep::SetObjectAtLocation(k_here, OB_WOLFSLAYER);

        WHEN("they seek")
        {
            auto result = lord->Cmd_Seek();

            THEN("they find nothing, still carrying what they had, and the sword stays put")
            {
                REQUIRE( result == nullptr );
                REQUIRE( lord->Carrying() == GetObject("OB_MOONRING") );
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_WOLFSLAYER );
            }
        }
    }
}

SCENARIO("Seeking a nasty finds it without fighting it")
{
    TMEStep::NewStory();

    GIVEN("a lord standing near a nasty")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::SetObjectAtLocation(k_here, OB_WOLVES);

        WHEN("they seek")
        {
            auto result = lord->Cmd_Seek();

            THEN("they find the nasty, unharmed and not in battle")
            {
                REQUIRE( result == GetObject("OB_WOLVES") );
                REQUIRE_FALSE( lord->IsDead() );
                REQUIRE_FALSE( lord->IsInBattle() );
            }
        }
    }
}
