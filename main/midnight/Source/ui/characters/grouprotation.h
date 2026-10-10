//
//  grouprotation.h
//  midnight
//
//  Pure helpers for rotating followers around the group circle.
//

#pragma once

namespace grouprotation
{
    // Duration of the rotate animation in seconds
    constexpr float ROTATE_DURATION = 0.25f;

    // The followers can be rotated left until the last follower is in the last visible slot
    inline bool canRotateLeft( int adjust, int followerCount, int maxVisible )
    {
        return adjust > (maxVisible - followerCount);
    }

    // The followers can be rotated right until the first follower is back in the first slot
    inline bool canRotateRight( int adjust )
    {
        return adjust < 0;
    }

    // A follower is shown while any part of it is within the visible slots [0..maxVisible-1],
    // including the slot either side so lords can slide in and out when rotating
    inline bool isVisible( float pos, int maxVisible )
    {
        return pos > -1.0f && pos < (float)maxVisible;
    }

    // Fully opaque in the visible slots, fading out across the slot either side
    inline float opacity( float pos, int maxVisible )
    {
        if ( !isVisible(pos, maxVisible) )
            return 0.0f;
        if ( pos < 0.0f )
            return 1.0f + pos;
        if ( pos > (float)(maxVisible - 1) )
            return (float)maxVisible - pos;
        return 1.0f;
    }
}
