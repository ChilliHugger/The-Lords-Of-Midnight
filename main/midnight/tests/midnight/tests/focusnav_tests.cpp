//
//  focusnav_tests.cpp
//  midnight
//
#include "catch2/catch.hpp"
#include "../../../Source/ui/uifocusnav.h"

using namespace chilli::ui::focus;

// y points up, so the top row of a grid has the largest y
static rect cell( int column, int row, int rows = 3, float size = 100, float spacing = 20 )
{
    float pitch = size + spacing;
    return { column * pitch, (rows - 1 - row) * pitch, size, size };
}

TEST_CASE("Focus moves through a vertical list")
{
    std::vector<rect> list = { cell(0,0), cell(0,1), cell(0,2) };

    REQUIRE( findNext(list[0], list, direction::down) == 1 );
    REQUIRE( findNext(list[1], list, direction::down) == 2 );
    REQUIRE( findNext(list[2], list, direction::up) == 1 );
    REQUIRE( findNext(list[1], list, direction::up) == 0 );
}

TEST_CASE("Focus stops at the ends of a list")
{
    std::vector<rect> list = { cell(0,0), cell(0,1), cell(0,2) };

    REQUIRE( findNext(list[0], list, direction::up) == -1 );
    REQUIRE( findNext(list[2], list, direction::down) == -1 );
    REQUIRE( findNext(list[1], list, direction::left) == -1 );
    REQUIRE( findNext(list[1], list, direction::right) == -1 );
}

TEST_CASE("Focus does not move when there is nothing to move to")
{
    std::vector<rect> none;
    REQUIRE( findNext(cell(0,0), none, direction::right) == -1 );
    REQUIRE( findFirst(none) == -1 );
}

TEST_CASE("Focus stays on the row or column of a grid")
{
    std::vector<rect> grid;
    for ( int row = 0; row < 3; row++ )
        for ( int column = 0; column < 3; column++ )
            grid.push_back(cell(column, row));

    auto at = [&](int column, int row) { return row * 3 + column; };

    REQUIRE( findNext(grid[at(1,1)], grid, direction::right) == at(2,1) );
    REQUIRE( findNext(grid[at(1,1)], grid, direction::left) == at(0,1) );
    REQUIRE( findNext(grid[at(1,1)], grid, direction::up) == at(1,0) );
    REQUIRE( findNext(grid[at(1,1)], grid, direction::down) == at(1,2) );
}

TEST_CASE("Focus skips a missing item to the next in line")
{
    // the middle of the row is disabled or hidden, so it is not a candidate
    std::vector<rect> row = { cell(0,0), cell(2,0) };

    REQUIRE( findNext(row[0], row, direction::right) == 1 );
    REQUIRE( findNext(row[1], row, direction::left) == 0 );
}

TEST_CASE("Focus prefers a far item in line to a near item off the line")
{
    rect from = cell(0,0);
    rect sameRow = cell(3,0);
    rect lowerRow = { 120, 0, 100, 100 };

    std::vector<rect> candidates = { lowerRow, sameRow };

    REQUIRE( findNext(from, candidates, direction::right) == 1 );
}

TEST_CASE("Focus picks the item most directly in line for different sized items")
{
    rect from = { 100, 400, 100, 100 };

    // two items below: one centred under, one off to the side but just as near
    rect under = { 100, 200, 100, 100 };
    rect aside = { 300, 200, 100, 100 };

    std::vector<rect> candidates = { aside, under };

    REQUIRE( findNext(from, candidates, direction::down) == 1 );
}

TEST_CASE("Focus ignores items behind the focused item")
{
    std::vector<rect> candidates = { cell(0,0), cell(1,0) };

    REQUIRE( findNext(candidates[0], candidates, direction::left) == -1 );
    REQUIRE( findNext(candidates[1], candidates, direction::right) == -1 );
}

TEST_CASE("Focus crosses a screen of unevenly placed icons")
{
    // left and right arrows at the sides of the screen, forward at the top, an
    // actions icon at the bottom right
    rect left    = { 40,  500, 100, 100 };
    rect right   = { 1780, 500, 100, 100 };
    rect forward = { 910, 900, 100, 100 };
    rect actions = { 1780, 20, 100, 100 };

    std::vector<rect> all = { left, right, forward, actions };

    REQUIRE( findNext(left, all, direction::right) == 1 );
    REQUIRE( findNext(left, all, direction::up) == 2 );
    REQUIRE( findNext(forward, all, direction::right) == 1 );
    REQUIRE( findNext(right, all, direction::down) == 3 );
    REQUIRE( findNext(actions, all, direction::up) == 1 );
    REQUIRE( findNext(right, all, direction::left) == 0 );
    REQUIRE( findNext(right, all, direction::up) == 2 );
}

TEST_CASE("Focus starts at the top left")
{
    std::vector<rect> candidates = { cell(2,2), cell(1,0), cell(0,0), cell(0,1) };

    REQUIRE( findFirst(candidates) == 2 );
}
