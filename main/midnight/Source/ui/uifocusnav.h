//
//  uifocusnav.h
//  midnight
//
//  Spatial navigation for the d-pad focus system: given the focused rectangle and the
//  rectangles of everything else that can take focus, which one does a press of
//  left/right/up/down move to? Pure geometry with no engine dependency.
//
//  Rectangles are in world space with y pointing up, as in Axmol.
//

#pragma once

#include <vector>
#include <cmath>

namespace chilli {
    namespace ui {
        namespace focus {

        enum class direction {
            left    = 0,
            right   = 1,
            up      = 2,
            down    = 3,
        };

        struct rect {
            float x{};
            float y{};
            float w{};
            float h{};

            float minX() const { return x; }
            float maxX() const { return x + w; }
            float minY() const { return y; }
            float maxY() const { return y + h; }
            float cx() const { return x + (w / 2); }
            float cy() const { return y + (h / 2); }
        };

        //
        // A candidate has to be on the pressed side of the focused rectangle (judged by the
        // centres, so a larger neighbouring rectangle that merely overlaps does not count).
        //
        // The best candidate is the one closest in the direction of travel, but drifting off
        // the line costs more than distance does: in a grid, pressing right from the end of
        // a row prefers a far item on the same row to a near item on the row below. The
        // distance between the centres on the other axis breaks ties so a column of
        // different sized items still picks the one most directly in line.
        //
        constexpr float OFF_AXIS_WEIGHT = 3.0f;
        constexpr float CENTRE_WEIGHT   = 0.5f;

        inline bool isBeyond( const rect& from, const rect& to, direction dir )
        {
            switch ( dir ) {
                case direction::left:   return to.cx() < from.cx();
                case direction::right:  return to.cx() > from.cx();
                case direction::up:     return to.cy() > from.cy();
                case direction::down:   return to.cy() < from.cy();
            }
            return false;
        }

        // gap between two ranges, 0 when they overlap
        inline float gap( float minA, float maxA, float minB, float maxB )
        {
            if ( maxA < minB ) return minB - maxA;
            if ( maxB < minA ) return minA - maxB;
            return 0.0f;
        }

        inline float score( const rect& from, const rect& to, direction dir )
        {
            bool horizontal = (dir == direction::left || dir == direction::right);

            float along = horizontal
                ? gap(from.minX(), from.maxX(), to.minX(), to.maxX())
                : gap(from.minY(), from.maxY(), to.minY(), to.maxY());

            float off = horizontal
                ? gap(from.minY(), from.maxY(), to.minY(), to.maxY())
                : gap(from.minX(), from.maxX(), to.minX(), to.maxX());

            float centreOff = horizontal
                ? std::fabs(to.cy() - from.cy())
                : std::fabs(to.cx() - from.cx());

            return along + (OFF_AXIS_WEIGHT * off) + (CENTRE_WEIGHT * centreOff);
        }

        // index of the best candidate, or -1 if there is nothing in that direction
        inline int findNext( const rect& from, const std::vector<rect>& candidates, direction dir )
        {
            int best = -1;
            float bestScore = 0.0f;

            for ( int ii = 0; ii < (int)candidates.size(); ii++ ) {
                const rect& to = candidates[ii];
                if ( !isBeyond(from, to, dir) )
                    continue;

                float s = score(from, to, dir);
                if ( best == -1 || s < bestScore ) {
                    best = ii;
                    bestScore = s;
                }
            }
            return best;
        }

        // reading order: highest first, then leftmost. Where focus starts when a screen
        // does not ask for anything in particular.
        inline int findFirst( const std::vector<rect>& candidates )
        {
            int best = -1;
            for ( int ii = 0; ii < (int)candidates.size(); ii++ ) {
                if ( best == -1 ) {
                    best = ii;
                    continue;
                }
                const rect& a = candidates[ii];
                const rect& b = candidates[best];
                if ( a.maxY() > b.maxY() || (a.maxY() == b.maxY() && a.minX() < b.minX()) ) {
                    best = ii;
                }
            }
            return best;
        }

        }
    }
}
