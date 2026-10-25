//
//  selectlayout_tests.cpp
//  midnight
//
#include "catch2/catch.hpp"
#include <cmath>
#include "../../../Source/ui/characters/uiselectlayout.h"

using namespace chilli::ui::select;

// a screen of 1280 x 800 with squares of 128: eight columns of which the last is the strip of icons
static gridinfo makeGrid()
{
    gridinfo grid;
    grid.cellWidth = 128;
    grid.cellHeight = 128;
    grid.columns = 7;
    grid.rows = 5;
    grid.startX = 64;
    grid.startY = 64;
    grid.pageWidth = 1280;
    grid.pageHeight = 800;
    grid.rightStrip = 128;
    grid.bottomStrip = 128;
    grid.maxPages = 10;
    return grid;
}

static float gap( const lordspot& a, const lordspot& b )
{
    return std::hypot(a.position.x - b.position.x, a.position.y - b.position.y);
}

static float radius( const lordspot& lord )
{
    return lord.group ? 128.0f : 64.0f;
}

static bool overlaps( const lordspot& a, const lordspot& b )
{
    return a.page == b.page && gap(a, b) < radius(a) + radius(b);
}

TEST_CASE("Tidy puts lords in the grid in order")
{
    selectlayout layout(makeGrid());
    std::vector<lordspot> lords(3);

    layout.tidy(lords);

    REQUIRE( lords[0].page == 1 );
    REQUIRE( lords[0].position == layout.gridLocation(0) );
    REQUIRE( lords[1].position == layout.gridLocation(1) );
    REQUIRE( lords[2].position == layout.gridLocation(2) );
}

TEST_CASE("Tidy gives a group the block of four squares")
{
    selectlayout layout(makeGrid());
    std::vector<lordspot> lords(4);
    lords[1].group = true;

    layout.tidy(lords);

    // the group is placed after the first lord, so starts on the second square, with the next lord beyond it
    auto first = layout.gridLocation(1);
    auto last = layout.gridLocation(1 + 7 + 1);
    REQUIRE( lords[1].position.x == Approx((first.x + last.x) / 2) );
    REQUIRE( lords[1].position.y == Approx((first.y + last.y) / 2) );

    for ( size_t a = 0; a < lords.size(); a++ ) {
        for ( size_t b = a + 1; b < lords.size(); b++ )
            REQUIRE_FALSE( overlaps(lords[a], lords[b]) );
    }
}

TEST_CASE("Tidy fills the squares beside and below a group")
{
    selectlayout layout(makeGrid());
    std::vector<lordspot> lords(6);
    lords[0].group = true;

    layout.tidy(lords);

    // the group has the top left block, the next lord is beside it and the rest are in the squares left on the page
    REQUIRE( lords[1].position == layout.gridLocation(2) );
    REQUIRE( lords[2].position == layout.gridLocation(3) );
    for ( size_t a = 0; a < lords.size(); a++ ) {
        REQUIRE( lords[a].page == 1 );
        for ( size_t b = a + 1; b < lords.size(); b++ )
            REQUIRE_FALSE( overlaps(lords[a], lords[b]) );
    }
}

TEST_CASE("Tidy moves on to the next page when one is full")
{
    selectlayout layout(makeGrid());
    std::vector<lordspot> lords(36);

    layout.tidy(lords);

    REQUIRE( lords[34].page == 1 );
    REQUIRE( lords[35].page == 2 );
    REQUIRE( lords[35].position == layout.gridLocation(0) );
}

TEST_CASE("A group on the page and clear of others stays where it is")
{
    selectlayout layout(makeGrid());
    std::vector<lordspot> lords(2);
    lords[0] = { true, 1, { 400, 400 } };
    lords[1] = { false, 1, { 800, 400 } };

    REQUIRE_FALSE( layout.makeRoom(lords) );
    REQUIRE( lords[0].position == (point{ 400, 400 }) );
    REQUIRE( lords[1].position == (point{ 800, 400 }) );
}

TEST_CASE("A group over the edge of the page is moved onto it")
{
    selectlayout layout(makeGrid());
    std::vector<lordspot> lords(1);
    lords[0] = { true, 1, { 40, 780 } };

    REQUIRE( layout.makeRoom(lords) );

    // the whole circle on the page, in from the left and the top
    REQUIRE( lords[0].position.x >= 128 );
    REQUIRE( lords[0].position.y <= 800 - 128 );
}

TEST_CASE("A group is kept out of the strips of icons")
{
    selectlayout layout(makeGrid());
    std::vector<lordspot> lords(1);
    lords[0] = { true, 1, { 1270, 20 } };

    layout.makeRoom(lords);

    REQUIRE( lords[0].position.x <= 1280 - 128 );
    REQUIRE( lords[0].position.y >= 128 );
}

TEST_CASE("A lord covered by a group moves just clear of it")
{
    selectlayout layout(makeGrid());
    std::vector<lordspot> lords(2);
    lords[0] = { true, 1, { 500, 400 } };
    lords[1] = { false, 1, { 560, 400 } };

    REQUIRE( layout.makeRoom(lords) );

    REQUIRE_FALSE( overlaps(lords[0], lords[1]) );
    REQUIRE( lords[0].position == (point{ 500, 400 }) );

    // no further than needed: the lord is only just outside the circle
    REQUIRE( gap(lords[0], lords[1]) < 128 + 64 + 20 );
}

TEST_CASE("A lord at an edge is moved the other way when there is no room beyond")
{
    selectlayout layout(makeGrid());
    std::vector<lordspot> lords(2);

    // the group is against the left of the page, the lord is on its left side, which has no room
    lords[0] = { true, 1, { 128, 400 } };
    lords[1] = { false, 1, { 64, 400 } };

    REQUIRE( layout.makeRoom(lords) );

    REQUIRE_FALSE( overlaps(lords[0], lords[1]) );
    REQUIRE( lords[1].position.x >= 64 );
}

TEST_CASE("Lords moved aside do not land on each other")
{
    selectlayout layout(makeGrid());
    std::vector<lordspot> lords(5);
    lords[0] = { true, 1, { 500, 400 } };
    lords[1] = { false, 1, { 420, 400 } };
    lords[2] = { false, 1, { 580, 400 } };
    lords[3] = { false, 1, { 500, 330 } };
    lords[4] = { false, 1, { 500, 470 } };

    // the lords around the group are also close to each other, so each has to be put somewhere the others are not
    layout.makeRoom(lords);

    for ( size_t a = 0; a < lords.size(); a++ ) {
        for ( size_t b = a + 1; b < lords.size(); b++ )
            REQUIRE_FALSE( overlaps(lords[a], lords[b]) );
    }
}

TEST_CASE("A lord with no room on the page goes to the next free space")
{
    selectlayout layout(makeGrid());

    // a page full of lords, and a group set in the middle of them
    std::vector<lordspot> lords;
    for ( int index = 0; index < 35; index++ ) {
        lordspot lord;
        lord.page = 1;
        lord.position = layout.gridLocation(index);
        lords.push_back(lord);
    }
    lordspot group;
    group.group = true;
    group.page = 1;
    group.position = { 500, 400 };
    lords.push_back(group);

    REQUIRE( layout.makeRoom(lords) );

    for ( size_t a = 0; a < lords.size(); a++ ) {
        for ( size_t b = a + 1; b < lords.size(); b++ )
            REQUIRE_FALSE( overlaps(lords[a], lords[b]) );
    }
}
