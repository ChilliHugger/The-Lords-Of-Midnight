//
//  movement_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here  = mxgridref(1, 1);
    const mxgridref k_south = mxgridref(1, 2);
    const mxgridref k_east  = mxgridref(2, 1);
    const mxgridref k_southeast = mxgridref(2, 2);

    s32 MeasureMovementEnergyCost(const string& name, mxgridref here, mxgridref there, mxdir_t looking, bool riding)
    {
        MapStep::ResetLocation(there);

        auto lord = TMEStep::PlaceLordAt(name, here, looking);
        if ( riding )
            lord->Flags().Set(cf_riding);
        else
            lord->Flags().Reset(cf_riding);

        u32 before = lord->energy;
        REQUIRE( lord->Cmd_MoveForward() == MX_OK );
        REQUIRE( lord->Location() == there );

        return (s32)(before - lord->energy);
    }

}


SCENARIO("A dead lord cannot walk forward")
{
    TMEStep::NewStory();

    GIVEN("a lord who is dead")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        TMEStep::LordIsDead(TMEStep::ch_morkin);

        WHEN("checking if they can walk forward")
        {
            auto result = lord->CanWalkForward();

            THEN("they cannot walk forward")
            {
                REQUIRE_FALSE( result );
            }
        }

        WHEN("they try to move forward")
        {
            auto result = lord->Cmd_MoveForward();

            THEN("the move fails and they stay put")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( lord->Location() == k_here );
            }
        }
    }
}

SCENARIO("A lord with no energy left cannot walk forward")
{
    TMEStep::NewStory();

    GIVEN("a lord whose energy is at the cannot-continue threshold")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->energy = (u32) sv_energy_cannot_continue;

        WHEN("checking if they can walk forward")
        {
            auto result = lord->CanWalkForward();

            THEN("they cannot walk forward")
            {
                REQUIRE_FALSE( result );
            }
        }
    }

    GIVEN("a lord whose energy is above the cannot-continue threshold")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->energy = (u32) sv_energy_cannot_continue + 1;

        WHEN("checking if they can walk forward")
        {
            auto result = lord->CanWalkForward();

            THEN("they can walk forward")
            {
                REQUIRE( result );
            }
        }
    }
}

SCENARIO("A hidden lord cannot walk forward unless auto-unhide is on")
{
    GIVEN("a lord who is hidden and auto-unhide is off")
    {
        TMEStep::NewStory();
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Set(cf_hidden);
        sv_auto_unhide = false;

        WHEN("checking if they can walk forward")
        {
            auto result = lord->CanWalkForward();

            THEN("they cannot walk forward")
            {
                REQUIRE_FALSE( result );
            }
        }
    }

    GIVEN("a lord who is hidden and auto-unhide is on")
    {
        TMEStep::NewStory();
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Set(cf_hidden);
        sv_auto_unhide = true;

        WHEN("checking if they can walk forward")
        {
            auto result = lord->CanWalkForward();

            THEN("they can walk forward")
            {
                REQUIRE( result );
            }
        }

        sv_auto_unhide = false;
    }
}

SCENARIO("A lord cannot walk forward at night")
{
    TMEStep::NewStory();

    GIVEN("it is night for this lord")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->time = (mxtime_t) sv_time_night;

        WHEN("checking if they can walk forward")
        {
            auto result = lord->CanWalkForward();

            THEN("they cannot walk forward")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A lord who is following someone else cannot move on their own")
{
    TMEStep::NewStory();

    GIVEN("a lord is following another")
    {
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto follower = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        MapStep::ResetLocation(k_south);

        TMEStep::LordHasFollowers(TMEStep::ch_morkin, { TMEStep::ch_luxor });

        WHEN("the follower tries to move forward on their own")
        {
            auto result = follower->Cmd_MoveForward();

            THEN("the move fails and they stay put")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( follower->Location() == k_here );
            }
        }
    }
}

SCENARIO("A leader cannot move forward if a follower is unable to walk")
{
    TMEStep::NewStory();

    GIVEN("a leader whose follower cannot walk")
    {
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto follower = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        MapStep::ResetLocation(k_south);

        TMEStep::LordHasFollowers(TMEStep::ch_morkin, { TMEStep::ch_luxor });
        follower->energy = (u32) sv_energy_cannot_continue;

        WHEN("the leader tries to move the group forward")
        {
            auto result = leader->Cmd_MoveForward();

            THEN("the move fails and the leader stays put")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( leader->Location() == k_here );
            }
        }
    }
}

SCENARIO("Movement is blocked by impassable terrain ahead")
{
    TMEStep::NewStory();

    GIVEN("the location ahead of a lord is impassable")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::ResetLocation(k_south);
        MapStep::SetImpassable(k_south);

        WHEN("the lord tries to move forward")
        {
            auto result = lord->Cmd_MoveForward();

            THEN("the move fails and they stay put")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( lord->Location() == k_here );
            }
        }
    }
}

SCENARIO("Movement is blocked by a nasty at the lord's own location")
{
    TMEStep::NewStory();
    sv_cheat_nasties_noblock = false;

    GIVEN("a nasty is at the lord's own location")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::ResetLocation(k_south);
        MapStep::SetObjectAtLocation(k_here, OB_WOLVES);

        WHEN("the lord tries to move forward")
        {
            auto result = lord->Cmd_MoveForward();

            THEN("the move fails until the nasty is dealt with")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( lord->Location() == k_here );
            }
        }

        WHEN("the nasties-noblock cheat is enabled")
        {
            sv_cheat_nasties_noblock = true;

            auto result = lord->Cmd_MoveForward();

            THEN("the lord can move past the nasty")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->Location() == k_south );
            }

            sv_cheat_nasties_noblock = false;
        }
    }
}

SCENARIO("Movement is blocked by an enemy army at the lord's own location, except at dawn")
{
    TMEStep::NewStory();
    sv_cheat_armies_noblock = false;

    GIVEN("an enemy regiment occupies the lord's own location")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::ResetLocation(k_south);
        TMEStep::RegimentAtLocation(k_here, 10);

        WHEN("it is not dawn")
        {
            lord->time = (mxtime_t)(sv_time_dawn - 1);

            auto result = lord->Cmd_MoveForward();

            THEN("the move is blocked")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( lord->Location() == k_here );
            }
        }

        WHEN("it is dawn")
        {
            lord->time = (mxtime_t) sv_time_dawn;

            auto result = lord->Cmd_MoveForward();

            THEN("the lord may still move")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->Location() == k_south );
            }
        }

        WHEN("the armies-noblock cheat is enabled and it is not dawn")
        {
            lord->time = (mxtime_t)(sv_time_dawn - 1);
            sv_cheat_armies_noblock = true;

            auto result = lord->Cmd_MoveForward();

            THEN("the lord may still move")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->Location() == k_south );
            }

            sv_cheat_armies_noblock = false;
        }
    }
}

SCENARIO("A successful move updates the lord's position, time, energy and battle state")
{
    TMEStep::NewStory();

    GIVEN("a lord in the open, ready to move")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::ResetLocation(k_south);

        lord->Flags().Set(cf_riding);
        lord->Flags().Set(cf_inbattle);
        lord->Flags().Set(cf_preparesbattle);

        u32 energyBefore = lord->energy;
        mxtime_t timeBefore = lord->time;

        WHEN("they move forward")
        {
            auto result = lord->Cmd_MoveForward();

            THEN("they arrive at the new location")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->Location() == k_south );
            }

            THEN("time and energy are both spent")
            {
                REQUIRE( lord->time < timeBefore );
                REQUIRE( lord->energy < energyBefore );
                REQUIRE( (energyBefore - lord->energy) == (timeBefore - lord->time) );
            }

            THEN("they are no longer flagged as being in or preparing for battle")
            {
                REQUIRE_FALSE( lord->Flags().Is(cf_inbattle) );
                REQUIRE_FALSE( lord->Flags().Is(cf_preparesbattle) );
            }
        }
    }
}

SCENARIO("Diagonal movement costs more than cardinal movement")
{
    TMEStep::NewStory();

    GIVEN("the same lord moving on the same terrain")
    {
        WHEN("they move cardinally and diagonally")
        {
            auto cardinalCost = MeasureMovementEnergyCost(TMEStep::ch_morkin, k_here, k_south, DR_SOUTH, true);
            auto diagonalCost = MeasureMovementEnergyCost(TMEStep::ch_morkin, k_here, k_southeast, DR_SOUTHEAST, true);

            THEN("the diagonal move costs strictly more")
            {
                REQUIRE( diagonalCost > cardinalCost );
            }
        }
    }
}

SCENARIO("Walking on foot costs more than riding")
{
    TMEStep::NewStory();

    GIVEN("the same lord making the same move")
    {
        WHEN("they make the move riding and on foot")
        {
            auto ridingCost = MeasureMovementEnergyCost(TMEStep::ch_morkin, k_here, k_south, DR_SOUTH, true);
            auto walkingCost = MeasureMovementEnergyCost(TMEStep::ch_morkin, k_here, k_south, DR_SOUTH, false);

            THEN("walking costs strictly more than riding")
            {
                REQUIRE( walkingCost > ridingCost );
            }
        }
    }
}

SCENARIO("A leader's followers move forward together")
{
    TMEStep::NewStory();

    GIVEN("a leader with a follower, both facing the same way")
    {
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto follower = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        MapStep::ResetLocation(k_south);

        TMEStep::LordHasFollowers(TMEStep::ch_morkin, { TMEStep::ch_luxor });

        WHEN("the leader moves forward")
        {
            auto result = leader->Cmd_MoveForward();

            THEN("the leader and the follower both arrive at the new location")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( leader->Location() == k_south );
                REQUIRE( follower->Location() == k_south );
                REQUIRE( follower->Looking() == leader->Looking() );
            }
        }
    }
}
