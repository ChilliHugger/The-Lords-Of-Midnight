//
//  uiselectlayout.cpp
//  midnight
//

#include "uiselectlayout.h"

#include <algorithm>
#include <cmath>

using namespace chilli::ui::select;

namespace {
    float distance( point a, point b )
    {
        return std::sqrt(((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)));
    }
}

point selectlayout::gridLocation( int index ) const
{
    index = index % lordsPerPage();

    int row = index / grid.columns;
    int column = index % grid.columns;

    point position;
    position.x = (column * grid.cellWidth) + grid.startX;
    position.y = grid.pageHeight - ((row * grid.cellHeight) + grid.startY);
    return position;
}

void selectlayout::tidy( std::vector<lordspot>& lords ) const
{
    const bool groupsFit = grid.columns >= 2 && grid.rows >= 2;

    // which squares are taken, a page at a time
    std::vector<std::vector<bool>> taken;

    auto isFree = [&](size_t page, int column, int row, int size) {
        if ( page >= taken.size() )
            return true;
        for ( int r = row; r < row + size; r++ ) {
            for ( int c = column; c < column + size; c++ ) {
                if ( taken[page][(r * grid.columns) + c] )
                    return false;
            }
        }
        return true;
    };

    for ( auto& lord : lords ) {
        int size = ( lord.group && groupsFit ) ? 2 : 1;

        size_t page = 0;
        int column = 0;
        int row = 0;
        bool found = false;
        while ( !found && (int)page < grid.maxPages ) {
            for ( row = 0; row + size <= grid.rows && !found; ) {
                for ( column = 0; column + size <= grid.columns && !found; ) {
                    if ( isFree(page, column, row, size) )
                        found = true;
                    else
                        column++;
                }
                if ( !found )
                    row++;
            }
            if ( !found )
                page++;
        }

        // no room anywhere: the last square of the last page
        if ( !found ) {
            page = grid.maxPages - 1;
            column = 0;
            row = 0;
        }

        if ( page >= taken.size() )
            taken.resize(page + 1, std::vector<bool>(grid.columns * grid.rows, false));
        for ( int r = row; r < row + size && r < grid.rows; r++ ) {
            for ( int c = column; c < column + size && c < grid.columns; c++ ) {
                taken[page][(r * grid.columns) + c] = true;
            }
        }

        // the middle of its squares
        int index = (int)(page * lordsPerPage()) + (row * grid.columns) + column;
        auto position = gridLocation(index);
        if ( size == 2 ) {
            auto opposite = gridLocation(index + grid.columns + 1);
            position.x = (position.x + opposite.x) / 2;
            position.y = (position.y + opposite.y) / 2;
        }

        lord.page = (int)page + 1;
        lord.position = position;
    }
}

float selectlayout::radius( const lordspot& lord ) const
{
    return lord.group ? grid.cellWidth : grid.cellWidth / 2;
}

// the nearest the middle of a lord can be to the position and have the whole of them on the page,
// and out of the strips of icons
point selectlayout::clamp( float radius, point position ) const
{
    float minX = radius;
    float maxX = std::max(minX, std::min(grid.pageWidth - radius, grid.pageWidth - grid.rightStrip));
    float minY = std::max(radius, grid.bottomStrip);
    float maxY = std::max(minY, grid.pageHeight - radius);

    position.x = std::max(minX, std::min(maxX, position.x));
    position.y = std::max(minY, std::min(maxY, position.y));
    return position;
}

bool selectlayout::isClear( const std::vector<lordspot>& lords, size_t index, int page, point position ) const
{
    float r = radius(lords[index]);
    if ( clamp(r, position) != position )
        return false;

    for ( size_t other = 0; other < lords.size(); other++ ) {
        if ( other == index || lords[other].page != page )
            continue;

        if ( distance(position, lords[other].position) < r + radius(lords[other]) )
            return false;
    }
    return true;
}

// Looks outwards from where the lord is, in rings, so the first clear place found is the one that
// moves them least, whichever way the edges and the other lords leave room.
bool selectlayout::findNearestClear( const std::vector<lordspot>& lords, size_t index, point& result ) const
{
    const float step = grid.cellWidth / 8.0f;
    const float pi = 3.14159265f;

    auto& lord = lords[index];
    auto start = clamp(radius(lord), lord.position);
    float reach = grid.pageWidth + grid.pageHeight;

    for ( float ring = 0; ring < reach; ring += step ) {
        int samples = ring == 0 ? 1 : std::min(96, std::max(16, (int)(2 * pi * ring / step)));

        bool found = false;
        float best = 0;
        for ( int ii = 0; ii < samples; ii++ ) {
            float angle = (2 * pi * ii) / samples;
            point candidate = { start.x + (std::cos(angle) * ring), start.y + (std::sin(angle) * ring) };
            if ( !isClear(lords, index, lord.page, candidate) )
                continue;

            float moved = distance(candidate, lord.position);
            if ( !found || moved < best ) {
                found = true;
                best = moved;
                result = candidate;
            }
        }
        if ( found )
            return true;
    }
    return false;
}

// the first square, over all the pages, where the lord fits
bool selectlayout::findFreeSpace( const std::vector<lordspot>& lords, size_t index, int& page, point& result ) const
{
    for ( int square = 0; square < grid.maxPages * lordsPerPage(); square++ ) {
        auto position = gridLocation(square);
        if ( lords[index].group && grid.columns >= 2 && grid.rows >= 2 ) {
            auto opposite = gridLocation(square + grid.columns + 1);
            position.x = (position.x + opposite.x) / 2;
            position.y = (position.y + opposite.y) / 2;
        }

        if ( isClear(lords, index, pageOf(square), position) ) {
            page = pageOf(square);
            result = position;
            return true;
        }
    }
    return false;
}

bool selectlayout::makeRoom( std::vector<lordspot>& lords ) const
{
    bool moved = false;

    for ( size_t index = 0; index < lords.size(); index++ ) {
        if ( !lords[index].group )
            continue;

        auto& group = lords[index];
        auto onPage = clamp(radius(group), group.position);
        if ( onPage != group.position ) {
            group.position = onPage;
            moved = true;
        }

        // nearest first, so that the ones that must move furthest see where the others went
        std::vector<size_t> covered;
        for ( size_t other = 0; other < lords.size(); other++ ) {
            if ( other == index || lords[other].page != group.page )
                continue;

            if ( distance(group.position, lords[other].position) < radius(group) + radius(lords[other]) )
                covered.push_back(other);
        }

        auto centre = group.position;
        std::sort(covered.begin(), covered.end(), [&](size_t a, size_t b) {
            return distance(centre, lords[a].position) < distance(centre, lords[b].position);
        });

        for ( auto other : covered ) {
            point clear;
            int page = lords[other].page;
            if ( findNearestClear(lords, other, clear) || findFreeSpace(lords, other, page, clear) ) {
                lords[other].page = page;
                lords[other].position = clear;
                moved = true;
            }
        }
    }
    return moved;
}
