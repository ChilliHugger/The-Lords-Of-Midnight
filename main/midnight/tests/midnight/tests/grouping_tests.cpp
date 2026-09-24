//
//  grouping_tests.cpp
//  midnight
//

#include "../steps/common_steps.h"
#include "../../steps/map_steps.h"
#include "../../mocks/mocks_entity.h"


namespace {

    // Morkin, Luxor and Rorthron all start loyal to RA_FREE, so by default
    // they are already "friends" for CanFollow purposes.
    const mxgridref k_here = mxgridref(1, 1);
    const mxgridref k_elsewhere = mxgridref(5, 5);

}


SCENARIO("A lord cannot follow a dead character")
{
    TMEStep::NewStory();

    GIVEN("a lord and a dead character at the same location")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto other = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        TMEStep::LordIsDead(TMEStep::ch_luxor);

        WHEN("checking if the lord can follow them")
        {
            auto result = lord->CanFollow(other);

            THEN("they cannot")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A dead lord cannot follow anyone")
{
    TMEStep::NewStory();

    GIVEN("a dead lord and a living character at the same location")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto other = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        TMEStep::LordIsDead(TMEStep::ch_morkin);

        WHEN("checking if the lord can follow them")
        {
            auto result = lord->CanFollow(other);

            THEN("they cannot")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A lord cannot follow a hidden character")
{
    TMEStep::NewStory();

    GIVEN("a lord and a hidden character at the same location")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto other = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        other->Flags().Set(cf_hidden);

        WHEN("checking if the lord can follow them")
        {
            auto result = lord->CanFollow(other);

            THEN("they cannot")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A hidden lord cannot follow anyone")
{
    TMEStep::NewStory();

    GIVEN("a hidden lord and a visible character at the same location")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto other = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->Flags().Set(cf_hidden);

        WHEN("checking if the lord can follow them")
        {
            auto result = lord->CanFollow(other);

            THEN("they cannot")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A lord cannot follow a leader whose group is already full")
{
    TMEStep::NewStory();

    GIVEN("a leader whose group is at the maximum size")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        leader->followers = MAX_CHARACTERS_FOLLOWING;
        leader->Flags().Set(cf_followers);

        WHEN("checking if the lord can follow them")
        {
            auto result = lord->CanFollow(leader);

            THEN("they cannot")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A lord cannot follow someone at a different location")
{
    TMEStep::NewStory();

    GIVEN("a lord and another character at different locations")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto other = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_elsewhere, DR_SOUTH);

        WHEN("checking if the lord can follow them")
        {
            auto result = lord->CanFollow(other);

            THEN("they cannot")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A lord cannot follow someone who is above ground while they are underground")
{
    TMEStep::NewStory();

    GIVEN("a lord underground and another character at the same map square, above ground")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto other = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->Flags().Set(cf_tunnel);

        WHEN("checking if the lord can follow them")
        {
            auto result = lord->CanFollow(other);

            THEN("they cannot")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A lord cannot follow someone they are not loyal to")
{
    TMEStep::NewStory();

    GIVEN("a lord and another character at the same location, loyal to different races")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto other = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        other->loyalty = RA_DOOMDARK;

        WHEN("checking if the lord can follow them")
        {
            auto result = lord->CanFollow(other);

            THEN("they cannot")
            {
                REQUIRE_FALSE( result );
            }
        }
    }
}

SCENARIO("A lord can follow a friendly character at the same location")
{
    TMEStep::NewStory();

    GIVEN("a lord and a friendly character at the same location")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);

        WHEN("the lord follows them")
        {
            auto result = lord->Cmd_Follow(leader);

            THEN("they successfully join the group")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( lord->Following() == leader );
                REQUIRE( lord->IsFollowing() );
                REQUIRE( leader->HasFollowers() );
                REQUIRE( leader->followers == 1 );
            }
        }
    }
}

SCENARIO("Cmd_UnFollow fails if the lord is not following the given character")
{
    TMEStep::NewStory();

    GIVEN("a lord who is not following anyone")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto other = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);

        WHEN("they try to unfollow that character")
        {
            auto result = lord->Cmd_UnFollow(other);

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("A follower can leave their leader's group")
{
    TMEStep::NewStory();

    GIVEN("a lord following a leader")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        lord->Cmd_Follow(leader);

        WHEN("they unfollow the leader")
        {
            auto result = lord->Cmd_UnFollow(leader);

            THEN("they are no longer part of the group")
            {
                REQUIRE( result == MX_OK );
                REQUIRE_FALSE( lord->IsFollowing() );
                REQUIRE( lord->Following() == nullptr );
                REQUIRE_FALSE( leader->HasFollowers() );
                REQUIRE( leader->followers == 0 );
            }
        }
    }
}

SCENARIO("Joining a new leader brings a follower's own followers with them")
{
    TMEStep::NewStory();

    GIVEN("a leader with a follower, who then joins another leader")
    {
        auto newLeader = TMEStep::PlaceLordAt(TMEStep::ch_rorthron, k_here, DR_SOUTH);
        auto oldLeader = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto follower = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        follower->Cmd_Follow(oldLeader);

        WHEN("the old leader joins the new leader")
        {
            auto result = oldLeader->Cmd_Follow(newLeader);

            THEN("both the old leader and their follower now follow the new leader")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( oldLeader->Following() == newLeader );
                REQUIRE( follower->Following() == newLeader );
                REQUIRE( newLeader->followers == 2 );
            }
        }
    }
}

SCENARIO("Disbanding a group fails if the lord has no followers")
{
    TMEStep::NewStory();

    GIVEN("a lord with no followers")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);

        WHEN("they try to disband the group")
        {
            auto result = lord->Cmd_DisbandGroup();

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("A leader can disband their group")
{
    TMEStep::NewStory();

    GIVEN("a leader with two followers")
    {
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto follower1 = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        auto follower2 = TMEStep::PlaceLordAt(TMEStep::ch_rorthron, k_here, DR_SOUTH);
        follower1->Cmd_Follow(leader);
        follower2->Cmd_Follow(leader);

        WHEN("the leader disbands the group")
        {
            auto result = leader->Cmd_DisbandGroup();

            THEN("no one follows the leader any more")
            {
                REQUIRE( result == MX_OK );
                REQUIRE_FALSE( leader->HasFollowers() );
                REQUIRE( leader->followers == 0 );
                REQUIRE_FALSE( follower1->IsFollowing() );
                REQUIRE_FALSE( follower2->IsFollowing() );
            }
        }
    }
}

SCENARIO("Swapping the group leader fails if there are no followers")
{
    TMEStep::NewStory();

    GIVEN("a lord with no followers")
    {
        auto lord = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto other = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);

        WHEN("they try to swap group leader")
        {
            auto result = lord->Cmd_SwapGroupLeader(other);

            THEN("the attempt fails")
            {
                REQUIRE( result == MX_FAILED );
            }
        }
    }
}

SCENARIO("Swapping group leader promotes a follower to lead the rest of the group")
{
    TMEStep::NewStory();

    GIVEN("a leader with two followers")
    {
        auto leader = TMEStep::PlaceLordAt(TMEStep::ch_morkin, k_here, DR_SOUTH);
        auto follower1 = TMEStep::PlaceLordAt(TMEStep::ch_luxor, k_here, DR_SOUTH);
        auto follower2 = TMEStep::PlaceLordAt(TMEStep::ch_rorthron, k_here, DR_SOUTH);
        follower1->Cmd_Follow(leader);
        follower2->Cmd_Follow(leader);

        WHEN("leadership is swapped to the first follower")
        {
            auto result = leader->Cmd_SwapGroupLeader(follower1);

            THEN("the old leader and the other follower now follow the new leader")
            {
                REQUIRE( result == MX_OK );
                REQUIRE( follower1->Following() == nullptr );
                REQUIRE( leader->Following() == follower1 );
                REQUIRE( follower2->Following() == follower1 );
                REQUIRE( follower1->followers == 2 );
            }
        }
    }
}
