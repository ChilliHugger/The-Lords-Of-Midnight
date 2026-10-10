//
//  group_rotation_tests.cpp
//  midnight
//

#include "catch2/catch.hpp"
#include "../../../Source/ui/characters/grouprotation.h"

using namespace grouprotation;

namespace {
    const int k_max_visible = 8;
}

SCENARIO("Group followers can only rotate while there are hidden lords")
{
    GIVEN("a group with 8 or fewer followers")
    {
        THEN("it cannot rotate in either direction")
        {
            REQUIRE_FALSE( canRotateLeft(0, 8, k_max_visible) );
            REQUIRE_FALSE( canRotateLeft(0, 5, k_max_visible) );
            REQUIRE_FALSE( canRotateRight(0) );
        }
    }

    GIVEN("a group with 10 followers")
    {
        THEN("it can rotate left twice, and then right again")
        {
            REQUIRE( canRotateLeft(0, 10, k_max_visible) );
            REQUIRE( canRotateLeft(-1, 10, k_max_visible) );
            REQUIRE_FALSE( canRotateLeft(-2, 10, k_max_visible) );
            REQUIRE( canRotateRight(-2) );
            REQUIRE( canRotateRight(-1) );
            REQUIRE_FALSE( canRotateRight(0) );
        }
    }
}

SCENARIO("Followers are visible and fully opaque in the group slots")
{
    for ( int slot=0; slot<k_max_visible; slot++ ) {
        REQUIRE( isVisible((float)slot, k_max_visible) );
        REQUIRE( opacity((float)slot, k_max_visible) == Approx(1.0f) );
    }
}

SCENARIO("Followers outside the group slots are hidden")
{
    REQUIRE_FALSE( isVisible(-1.0f, k_max_visible) );
    REQUIRE_FALSE( isVisible((float)k_max_visible, k_max_visible) );
    REQUIRE( opacity(-1.0f, k_max_visible) == Approx(0.0f) );
    REQUIRE( opacity((float)k_max_visible, k_max_visible) == Approx(0.0f) );
}

SCENARIO("Followers fade in and out while rotating past the ends of the group")
{
    GIVEN("a lord sliding in from the first slot")
    {
        REQUIRE( isVisible(-0.5f, k_max_visible) );
        REQUIRE( opacity(-0.5f, k_max_visible) == Approx(0.5f) );
    }

    GIVEN("a lord sliding out of the last slot")
    {
        REQUIRE( isVisible(7.5f, k_max_visible) );
        REQUIRE( opacity(7.25f, k_max_visible) == Approx(0.75f) );
    }
}
