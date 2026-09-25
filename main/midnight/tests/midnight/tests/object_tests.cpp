//
//  object_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here = mxgridref(1, 1);

}


SCENARIO("Dropping a carried object places it at the lord's location")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying an object, standing on empty ground")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::SetObjectAtLocation(k_here, OB_NONE);
        TMEStep::LordCarryingObject(TMEStep::ch_morkin, OB_WOLFSLAYER);

        WHEN("the lord drops the object")
        {
            auto result = lord->Cmd_DropObject();

            THEN("the object is left at their location and they carry nothing")
            {
                REQUIRE( result == MX_OK );
                REQUIRE_FALSE( lord->IsCarryingObject() );
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_WOLFSLAYER );
            }
        }
    }
}

SCENARIO("Picking up an object at the lord's location")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying nothing, standing where an object lies")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        TMEStep::LordNotCarryingObject(TMEStep::ch_morkin);
        MapStep::SetObjectAtLocation(k_here, OB_WOLFSLAYER);

        WHEN("the lord picks up the object")
        {
            auto result = lord->Cmd_PickupObject();

            THEN("they are now carrying it and it is removed from the map")
            {
                REQUIRE( result != nullptr );
                REQUIRE( lord->Carrying() == GetObject("OB_WOLFSLAYER") );
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_NONE );
            }
        }
    }
}

SCENARIO("Picking up an object while already carrying one swaps them")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying an object, standing where a different object lies")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        TMEStep::LordCarryingObject(TMEStep::ch_morkin, OB_WOLFSLAYER);
        MapStep::SetObjectAtLocation(k_here, OB_MOONRING);

        WHEN("the lord picks up the new object")
        {
            auto result = lord->Cmd_PickupObject();

            THEN("they carry the new object and the old one is left behind")
            {
                REQUIRE( result == GetObject("OB_MOONRING") );
                REQUIRE( lord->Carrying() == GetObject("OB_MOONRING") );
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_WOLFSLAYER );
            }
        }
    }
}

SCENARIO("Picking up when there is nothing there drops what was being carried")
{
    TMEStep::NewStory();

    GIVEN("a lord carrying an object, standing on empty ground")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        TMEStep::LordCarryingObject(TMEStep::ch_morkin, OB_WOLFSLAYER);
        MapStep::SetObjectAtLocation(k_here, OB_NONE);

        WHEN("the lord tries to pick something up")
        {
            auto result = lord->Cmd_PickupObject();

            THEN("they end up carrying nothing, and the old object is left behind")
            {
                REQUIRE( result == nullptr );
                REQUIRE_FALSE( lord->IsCarryingObject() );
                REQUIRE( MapStep::GetObjectAtLocation(k_here) == OB_WOLFSLAYER );
            }
        }
    }
}
