//
//  regiment_ai_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"
#include "../../../Source/tme/baseinc/regiment_turn_processor.h"


namespace {

    // mxgridref::operator+ is not const-qualified, so this is deliberately
    // not const - it is used in direction arithmetic (k_here + DR_NORTH etc).
    mxgridref k_here = mxgridref(5, 5);

    // Exposes RegimentTurnProcessor's protected internals for direct testing.
    // Process() alone only lets us observe the end result of up to
    // sv_regiment_default_moves worth of movement; these let individual
    // decisions (Next/Wander/Retarget/Follow) be exercised one at a time.
    class TestableRegimentTurnProcessor : public RegimentTurnProcessor
    {
    public:
        void SetRegiment(mxregiment* r)
        {
            regiment = r;
            turns = 100;
        }

        using RegimentTurnProcessor::Next;
        using RegimentTurnProcessor::Wander;
        using RegimentTurnProcessor::Retarget;
        using RegimentTurnProcessor::Follow;

        mxgridref TargetLocation() const { return targetlocation; }
        void TargetLocation(mxgridref loc) { targetlocation = loc; }
        u32 Turns() const { return turns; }
    };

    mxregiment* SetUpRegiment(mxorders_t orders, u32 total = 10)
    {
        auto regiment = TMEStep::RegimentAtLocation(k_here, total);
        regiment->Orders(orders);
        regiment->Delay(0);
        regiment->Race(RA_DOOMGUARD);
        regiment->Type(UT_WARRIORS);
        return regiment;
    }

    // Clears a location and all eight of its neighbours to known-passable,
    // unremarkable ground.
    void ClearSurroundings(mxgridref centre)
    {
        MapStep::ResetLocation(centre);
        for ( int dir = DR_NORTH; dir <= DR_NORTHWEST; dir++ ) {
            MapStep::ResetLocation(centre + (mxdir_t) dir);
        }
    }

}


SCENARIO("A regiment with no troops left stops moving")
{
    TMEStep::NewStory();

    GIVEN("a wandering regiment with no troops")
    {
        ClearSurroundings(k_here);
        auto regiment = SetUpRegiment(OD_WANDER, 0);

        TestableRegimentTurnProcessor processor;
        processor.SetRegiment(regiment);

        WHEN("its turn is processed")
        {
            processor.Next();

            THEN("it is stopped for the night, without moving")
            {
                REQUIRE( processor.Turns() == 0 );
                REQUIRE( regiment->Location() == k_here );
            }
        }
    }
}

SCENARIO("A regiment on hold stops moving")
{
    TMEStep::NewStory();

    GIVEN("a regiment ordered to hold")
    {
        ClearSurroundings(k_here);
        auto regiment = SetUpRegiment(OD_HOLD);

        TestableRegimentTurnProcessor processor;
        processor.SetRegiment(regiment);

        WHEN("its turn is processed")
        {
            processor.Next();

            THEN("it is stopped for the night, without moving")
            {
                REQUIRE( processor.Turns() == 0 );
                REQUIRE( regiment->Location() == k_here );
            }
        }
    }
}

SCENARIO("A delayed regiment counts down its delay instead of moving")
{
    TMEStep::NewStory();

    GIVEN("a wandering regiment with a delay still to run down")
    {
        ClearSurroundings(k_here);
        auto regiment = SetUpRegiment(OD_WANDER);
        regiment->Delay(3);

        TestableRegimentTurnProcessor processor;
        processor.SetRegiment(regiment);

        WHEN("its turn is processed")
        {
            processor.Next();

            THEN("its delay counts down by one, and it does not move")
            {
                REQUIRE( regiment->Delay() == 2 );
                REQUIRE( processor.Turns() == 0 );
                REQUIRE( regiment->Location() == k_here );
            }
        }
    }
}

SCENARIO("A regiment already standing on a special location stops moving")
{
    TMEStep::NewStory();

    GIVEN("a wandering regiment standing on a special location")
    {
        ClearSurroundings(k_here);
        MapStep::SetSpecial(k_here);
        auto regiment = SetUpRegiment(OD_WANDER);

        TestableRegimentTurnProcessor processor;
        processor.SetRegiment(regiment);

        WHEN("its turn is processed")
        {
            processor.Next();

            THEN("it is stopped for the night, without moving")
            {
                REQUIRE( processor.Turns() == 0 );
                REQUIRE( regiment->Location() == k_here );
            }
        }
    }
}

SCENARIO("A regiment with no orders stops moving")
{
    TMEStep::NewStory();

    GIVEN("a regiment with no orders at all")
    {
        ClearSurroundings(k_here);
        auto regiment = SetUpRegiment(OD_NONE);

        TestableRegimentTurnProcessor processor;
        processor.SetRegiment(regiment);

        WHEN("its turn is processed")
        {
            processor.Next();

            THEN("it is stopped for the night, without moving")
            {
                REQUIRE( processor.Turns() == 0 );
                REQUIRE( regiment->Location() == k_here );
            }
        }
    }
}

SCENARIO("A wandering regiment moves onto open ground")
{
    TMEStep::NewStory();

    GIVEN("a wandering regiment surrounded by open, passable ground")
    {
        ClearSurroundings(k_here);
        auto regiment = SetUpRegiment(OD_WANDER);

        TestableRegimentTurnProcessor processor;
        processor.SetRegiment(regiment);

        WHEN("its turn is processed")
        {
            processor.Next();

            THEN("it moves to one of the neighbouring squares, spending some of its turns")
            {
                REQUIRE_FALSE( regiment->Location() == k_here );
                REQUIRE( processor.Turns() < 100 );
            }
        }
    }
}

SCENARIO("A regiment boxed in on all sides cannot wander anywhere")
{
    TMEStep::NewStory();

    GIVEN("a regiment surrounded on all eight sides by impassable ground")
    {
        ClearSurroundings(k_here);
        for ( int dir = DR_NORTH; dir <= DR_NORTHWEST; dir++ ) {
            MapStep::SetImpassable(k_here + (mxdir_t) dir);
        }
        auto regiment = SetUpRegiment(OD_WANDER);

        TestableRegimentTurnProcessor processor;
        processor.SetRegiment(regiment);

        WHEN("it tries to wander")
        {
            processor.Wander();

            THEN("it stays exactly where it is, and gives up for the night")
            {
                REQUIRE( processor.TargetLocation() == k_here );
                REQUIRE( processor.Turns() == 0 );
            }
        }
    }
}

SCENARIO("Retargeting finds a route when at least one direction is passable")
{
    TMEStep::NewStory();

    GIVEN("a regiment wanting to move north, with north, northeast and northwest all open")
    {
        ClearSurroundings(k_here);
        auto regiment = SetUpRegiment(OD_WANDER);

        TestableRegimentTurnProcessor processor;
        processor.SetRegiment(regiment);
        processor.TargetLocation(k_here + DR_NORTH);

        WHEN("it retargets")
        {
            auto result = processor.Retarget();

            THEN("it succeeds, landing on one of the open neighbouring squares")
            {
                REQUIRE( result );

                auto landed = processor.TargetLocation();
                bool isNeighbour = (landed == k_here + DR_NORTH)
                    || (landed == k_here + DR_NORTHEAST)
                    || (landed == k_here + DR_NORTHWEST);
                REQUIRE( isNeighbour );
            }
        }
    }
}

SCENARIO("Retargeting fails when every candidate direction is impassable")
{
    TMEStep::NewStory();

    GIVEN("a regiment wanting to move north, with north, northeast and northwest all blocked")
    {
        ClearSurroundings(k_here);
        // Retarget checks terrain-level impassability (isTerrainImpassable),
        // not the ad-hoc lf_impassable map flag, so blocking these squares
        // means giving them genuinely blocked terrain.
        MapStep::SetTerrain(k_here + DR_NORTH, TN_FROZENWASTE);
        MapStep::SetTerrain(k_here + DR_NORTHEAST, TN_FROZENWASTE);
        MapStep::SetTerrain(k_here + DR_NORTHWEST, TN_FROZENWASTE);
        auto regiment = SetUpRegiment(OD_WANDER);

        TestableRegimentTurnProcessor processor;
        processor.SetRegiment(regiment);
        processor.TargetLocation(k_here + DR_NORTH);

        WHEN("it retargets")
        {
            auto result = processor.Retarget();

            THEN("it fails to find anywhere to go")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A regiment following a dead character switches to following the moon ring wearer instead")
{
    TMEStep::NewStory();

    GIVEN("a regiment following a character who has since died")
    {
        ClearSurroundings(k_here);
        auto regiment = SetUpRegiment(OD_FOLLOW);

        auto target = GetCharacter(TMEStep::ch_luxor);
        TMEStep::LordIsDead(TMEStep::ch_luxor);
        regiment->TargetId( mxentity::SafeIdt(target) );

        auto moonringWearer = tme::mx->scenario->MoonringWearer();
        REQUIRE( moonringWearer != nullptr );

        TestableRegimentTurnProcessor processor;
        processor.SetRegiment(regiment);

        WHEN("it follows")
        {
            processor.Follow();

            THEN("it redirects to whoever now wears the moon ring")
            {
                REQUIRE( regiment->TargetId() == mxentity::SafeIdt(moonringWearer) );
                REQUIRE( processor.TargetLocation() == moonringWearer->Location() );
            }
        }
    }
}

SCENARIO("Processing a held regiment's turn terminates immediately")
{
    TMEStep::NewStory();

    GIVEN("a regiment ordered to hold")
    {
        ClearSurroundings(k_here);
        auto regiment = SetUpRegiment(OD_HOLD);
        auto originalLocation = regiment->Location();

        WHEN("its full turn is processed")
        {
            RegimentTurnProcessor processor;
            processor.Process(regiment);

            THEN("it never moves")
            {
                REQUIRE( regiment->Location() == originalLocation );
            }
        }
    }
}
