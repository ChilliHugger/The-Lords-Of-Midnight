//
//  uiselectlayout.h
//  midnight
//
//  Where lords sit on the pages of the select screen: tidying them into a grid, and
//  making room when a lord becomes a group. Pure geometry with no engine dependency.
//
//  A single lord takes up one square of the grid and a group, a circle of lords, the
//  block of two by two squares around its middle. Positions are the middle of the lord,
//  on a page whose y points up. Pages count from 1.
//

#pragma once

#include <vector>

namespace chilli {
    namespace ui {
        namespace select {

        struct point {
            float x{};
            float y{};

            bool operator==( const point& other ) const { return x == other.x && y == other.y; }
            bool operator!=( const point& other ) const { return !(*this == other); }
        };

        struct lordspot {
            bool    group{};
            int     page{};
            point   position;
        };

        struct gridinfo {
            float   cellWidth{};
            float   cellHeight{};
            int     columns{};
            int     rows{};
            float   startX{};           // the middle of the first square, from the left
            float   startY{};           // and from the top
            float   pageWidth{};
            float   pageHeight{};
            float   rightStrip{};       // icons down the right and along the bottom, which lords keep out of
            float   bottomStrip{};
            int     maxPages{};
        };

        class selectlayout
        {
        public:
            explicit selectlayout( const gridinfo& grid ) : grid(grid) {}

            int lordsPerPage() const { return grid.columns * grid.rows; }
            int pageOf( int index ) const { return (index / lordsPerPage()) + 1; }
            point gridLocation( int index ) const;

            // every lord in the grid, in order, in the first place it fits
            void tidy( std::vector<lordspot>& lords ) const;

            // Makes each group whole on its page and clear of other lords: moved onto the page if it is
            // over an edge, and the lords it covers moved aside as little as possible, whichever way has
            // room. A lord with no room on the page goes to the next free space. Returns whether any
            // lord moved.
            bool makeRoom( std::vector<lordspot>& lords ) const;

        private:
            float radius( const lordspot& lord ) const;
            point clamp( float radius, point position ) const;
            bool isClear( const std::vector<lordspot>& lords, size_t index, int page, point position ) const;
            bool findNearestClear( const std::vector<lordspot>& lords, size_t index, point& result ) const;
            bool findFreeSpace( const std::vector<lordspot>& lords, size_t index, int& page, point& result ) const;

        private:
            gridinfo grid;
        };

        }
    }
}
