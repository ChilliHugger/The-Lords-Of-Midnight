//
//  tunnel_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    const mxgridref k_here  = mxgridref(1, 1);
    const mxgridref k_north = mxgridref(1, 0);
    const mxgridref k_east  = mxgridref(2, 1);
    const mxgridref k_south = mxgridref(1, 2);
    const mxgridref k_west  = mxgridref(0, 1);

    // Resets the entrance square and all four cardinal neighbours, so no
    // stray tunnel/terrain state from the real map data leaks into a test.
    void ClearSurroundings()
    {
        MapStep::ResetLocation(k_here);
        MapStep::ResetLocation(k_north);
        MapStep::ResetLocation(k_east);
        MapStep::ResetLocation(k_south);
        MapStep::ResetLocation(k_west);
    }

}


SCENARIO("A lord who is following someone else cannot enter a tunnel")
{
    TMEStep::NewStory();

    GIVEN("a lord is following another, at a tunnel entrance")
    {
        ClearSurroundings();
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto follower = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        MapStep::SetTunnelEntrance(k_here);
        TMEStep::LordHasFollowers(TMEStep::ch_morkin, { TMEStep::ch_luxor });

        WHEN("the follower tries to enter the tunnel on their own")
        {
            auto result = follower->Cmd_EnterTunnel();

            THEN("the attempt fails and they remain above ground")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE_FALSE( follower->IsInTunnel() );
            }
        }
    }
}

SCENARIO("A lord already in a tunnel cannot enter a tunnel again")
{
    TMEStep::NewStory();

    GIVEN("a lord who is already underground, at a tunnel entrance")
    {
        ClearSurroundings();
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::SetTunnelEntrance(k_here);
        lord->Flags().Set(cf_tunnel);

        WHEN("they try to enter the tunnel")
        {
            auto result = lord->Cmd_EnterTunnel();

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("A lord cannot enter a tunnel where there is no entrance")
{
    TMEStep::NewStory();

    GIVEN("a lord standing on ordinary ground")
    {
        ClearSurroundings();
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);

        WHEN("they try to enter a tunnel")
        {
            auto result = lord->Cmd_EnterTunnel();

            THEN("the attempt fails and they remain above ground")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE_FALSE( lord->IsInTunnel() );
            }
        }
    }
}

SCENARIO("A lord entering a tunnel is dropped into the connecting tunnel square")
{
    TMEStep::NewStory();

    GIVEN("a tunnel entrance connects to one adjacent tunnel square")
    {
        ClearSurroundings();
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::SetTunnelEntrance(k_here);
        MapStep::SetTunnel(k_east);

        WHEN("they enter the tunnel")
        {
            auto result = lord->Cmd_EnterTunnel();

            THEN("they are placed in the connecting square, underground")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->IsInTunnel() );
                REQUIRE( lord->Location() == k_east );
                REQUIRE( lord->Looking() == DR_EAST );
            }
        }
    }
}

SCENARIO("A lord entering a tunnel with no connecting square stays put, but underground")
{
    TMEStep::NewStory();

    GIVEN("a tunnel entrance with no adjacent tunnel square")
    {
        ClearSurroundings();
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::SetTunnelEntrance(k_here);

        WHEN("they enter the tunnel")
        {
            auto result = lord->Cmd_EnterTunnel();

            THEN("they remain at the entrance, but are now underground")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->IsInTunnel() );
                REQUIRE( lord->Location() == k_here );
            }
        }
    }
}

SCENARIO("A leader's followers enter the tunnel together")
{
    TMEStep::NewStory();

    GIVEN("a leader with a follower, at a tunnel entrance")
    {
        ClearSurroundings();
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto follower = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        MapStep::SetTunnelEntrance(k_here);
        MapStep::SetTunnel(k_east);
        TMEStep::LordHasFollowers(TMEStep::ch_morkin, { TMEStep::ch_luxor });

        WHEN("the leader enters the tunnel")
        {
            auto result = leader->Cmd_EnterTunnel();

            THEN("the leader and follower both end up underground, together")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( leader->IsInTunnel() );
                REQUIRE( follower->IsInTunnel() );
                REQUIRE( leader->Location() == k_east );
                REQUIRE( follower->Location() == k_east );
                REQUIRE( follower->Looking() == leader->Looking() );
            }
        }
    }
}

SCENARIO("A lord who is following someone else cannot exit a tunnel")
{
    TMEStep::NewStory();

    GIVEN("a lord is following another, both underground at a tunnel exit")
    {
        ClearSurroundings();
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto follower = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        MapStep::SetTunnelExit(k_here);
        leader->Flags().Set(cf_tunnel);
        follower->Flags().Set(cf_tunnel);
        TMEStep::LordHasFollowers(TMEStep::ch_morkin, { TMEStep::ch_luxor });

        WHEN("the follower tries to exit the tunnel on their own")
        {
            auto result = follower->Cmd_ExitTunnel();

            THEN("the attempt fails and they remain underground")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( follower->IsInTunnel() );
            }
        }
    }
}

SCENARIO("A lord who is not in a tunnel cannot exit one")
{
    TMEStep::NewStory();

    GIVEN("a lord above ground, standing where a tunnel exit happens to be")
    {
        ClearSurroundings();
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::SetTunnelExit(k_here);

        WHEN("they try to exit a tunnel")
        {
            auto result = lord->Cmd_ExitTunnel();

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("A lord cannot exit a tunnel where there is no exit")
{
    TMEStep::NewStory();

    GIVEN("a lord underground, with no tunnel exit here")
    {
        ClearSurroundings();
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        lord->Flags().Set(cf_tunnel);

        WHEN("they try to exit the tunnel")
        {
            auto result = lord->Cmd_ExitTunnel();

            THEN("the attempt fails and they remain underground")
            {
                REQUIRE( result == MX_FAILED );
                REQUIRE( lord->IsInTunnel() );
            }
        }
    }
}

SCENARIO("A lord exiting a tunnel is no longer flagged as underground")
{
    TMEStep::NewStory();

    GIVEN("a lord underground, at a tunnel exit")
    {
        ClearSurroundings();
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        MapStep::SetTunnelExit(k_here);
        lord->Flags().Set(cf_tunnel);

        WHEN("they exit the tunnel")
        {
            auto result = lord->Cmd_ExitTunnel();

            THEN("they surface at the same location")
            {
                REQUIRE( result == MX_OK );
                REQUIRE_FALSE( lord->IsInTunnel() );
                REQUIRE( lord->Location() == k_here );
            }
        }
    }
}

SCENARIO("A leader's followers exit the tunnel together")
{
    TMEStep::NewStory();

    GIVEN("a leader with a follower, both underground at a tunnel exit")
    {
        ClearSurroundings();
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto follower = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        MapStep::SetTunnelExit(k_here);
        leader->Flags().Set(cf_tunnel);
        follower->Flags().Set(cf_tunnel);
        TMEStep::LordHasFollowers(TMEStep::ch_morkin, { TMEStep::ch_luxor });

        WHEN("the leader exits the tunnel")
        {
            auto result = leader->Cmd_ExitTunnel();

            THEN("the leader and follower both surface, together")
            {
                REQUIRE( result == MX_OK );
                REQUIRE_FALSE( leader->IsInTunnel() );
                REQUIRE_FALSE( follower->IsInTunnel() );
            }
        }
    }
}
