//
//  debugsetup.h
//  tme
//
//  Helpers for quickly putting a new game into a known state for manual testing.
//  Nothing here is compiled unless the define below is enabled.
//

#pragma once

// Start every new game with a large group of recruited lords following the starting lord
#define _DEBUG_NEW_GAME_GROUP_
#define DEBUG_NEW_GAME_GROUP_SIZE   12

#if defined(_DEBUG_NEW_GAME_GROUP_)

namespace tme {
namespace utils {

    // Moves up to 'count' living, friendly characters to the current character's location,
    // recruits them and makes them follow the current character. Returns the number of
    // characters now following.
    int DebugCreateLargeGroup(int count);

}
}

#endif
