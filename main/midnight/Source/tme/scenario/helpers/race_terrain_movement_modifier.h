//
//  race_terrain_movement_modifier.h
//  midnight
//
//  Created by Chris Wild on 06/09/2023.
//

#ifndef race_terrain_movement_modifier_h
#define race_terrain_movement_modifier_h

#include "../../baseinc/tme_internal.h"

namespace TME {
    namespace helpers {

        class RaceTerrainMovementModifier
        {
        public:
            static u32 Get( mxrace_t race, mxterrain_t terrain );
        };

    }
}

#endif /* race_terrain_movement_modifier_h */
