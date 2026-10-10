//
//  debugsetup.cpp
//  tme
//

#include "debugsetup.h"

#if defined(_DEBUG_NEW_GAME_GROUP_)

#include "../baseinc/tme_internal.h"

namespace tme {
namespace utils {

    int DebugCreateLargeGroup(int count)
    {
        auto leader = mx->CurrentChar();
        if ( leader == nullptr )
            return 0;

        auto here = leader->Location();

        FOR_EACH_CHARACTER(lord) {
            if ( leader->followers >= count || leader->followers >= MAX_CHARACTERS_FOLLOWING )
                break;

            if ( lord == leader || lord->IsDead() || lord->IsHidden() || lord->IsFollowing() )
                continue;

            // only characters on the leader's side
            if ( lord->NormalisedLoyalty() != leader->NormalisedLoyalty() )
                continue;

            lord->Location(here);
            lord->looking = leader->looking;
            lord->time = sv_time_dawn;
            lord->energy = 100;

            if ( !lord->IsRecruited() )
                lord->Recruited(leader);

            lord->Cmd_Follow(leader);
        }

        return leader->followers;
    }

}
}

#endif // _DEBUG_NEW_GAME_GROUP_
