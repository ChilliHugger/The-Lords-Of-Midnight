//
//  race_terrain_movement_modifier.cpp
//  midnight
//
//  Created by Chris Wild on 06/09/2023.
//

#include "race_terrain_movement_modifier.h"

namespace TME {
    namespace helpers {

        u32 RaceTerrainMovementModifier::Get( mxrace_t race, mxterrain_t terrain )
        {
            // TODO require race/terrain table here
            // SHOULD BE DONE THROUGH THE DATABASE
            switch(terrain) {
                case TN_DOWNS:
                    if ( race == RA_DOOMGUARD ) return 0;
                    return 1;
                case TN_MOUNTAIN:
                    return 4;
                case TN_FOREST:
                    if ( race == RA_DOOMGUARD ) return 4;
                    if ( race != RA_FEY ) return 3;
                    break;
                default:
                    break;
            }
            return 0 ;
        }

    }
}

#if defined(_DDR_)
//    L7354  DB TERRAIN_CITY,   TERRAIN_CITY            ;moonprince
//    DB TERRAIN_PLAINS, TERRAIN_PLAINS        ;free
//    DB TERRAIN_TOWER,  TERRAIN_TOWER            ;wise
//    DB TERRAIN_FOREST, TERRAIN_FOREST        ;fey
//    DB TERRAIN_HILLS,  TERRAIN_HILLS            ;barbarian
//    DB TERRAIN_PLAINS, TERRAIN_PLAINS        ;icelord
//    DB TERRAIN_FOREST                        ;fey
//    DB TERRAIN_MOUNTAINS                        ;giant
//    DB TERRAIN_PLAINS                        ;heartstealer
//    DB TERRAIN_FROZENWASTE                    ;dwarf
    
//    L7354A DB TERRAIN_MOUNTAINS, TERRAIN_MOUNTAINS  ;moonprince
//    DB TERRAIN_PLAINS,    TERRAIN_PLAINS     ;free
//    DB TERRAIN_FOREST,    TERRAIN_FOREST     ;wise
//    DB TERRAIN_FOREST,    TERRAIN_FOREST     ;fey
//    DB TERRAIN_FOREST,    TERRAIN_FOREST     ;barbarian
//    DB TERRAIN_PLAINS,    TERRAIN_PLAINS     ;icelord
//    DB TERRAIN_FOREST                        ;fey
//    DB TERRAIN_MOUNTAINS                        ;giant
//    DB TERRAIN_PLAINS                        ;heartstealer
//    DB TERRAIN_FROZENWASTE                    ;dwarf
#endif
