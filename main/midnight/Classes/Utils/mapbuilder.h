//
//  mapbuilder.h
//  midnight
//
//  Created by Chris Wild on 09/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef mapbuilder_h
#define mapbuilder_h

#include "../library/libinc/mxtypes.h"
#include "../tme_interface.h"

using namespace tme;

enum class mapflags {
    show_stronghold_names       = MXBIT(0),
    show_current_location       = MXBIT(1),
    show_enemy_armies           = MXBIT(2),
    show_enemy_movement         = MXBIT(3),
    show_characters             = MXBIT(4),
    show_characters_movement    = MXBIT(5),
    show_all_characters         = MXBIT(6),
    show_all_critters           = MXBIT(7),
    show_mist                   = MXBIT(8),
    show_objects                = MXBIT(9),
    show_grid                   = MXBIT(10),
    show_visited                = MXBIT(11),
    show_looked_at              = MXBIT(12),
    show_critters               = MXBIT(13),
    show_tunnels                = MXBIT(14),
    
    debug_map                   = MXBIT(16),
    
};

enum CELLFLAGS {
    cf_visible                  = MXBIT(0)
};

//enum LAYERS {
//    LAYER_TERRAIN,
//    LAYER_CRITTERS,
//    LAYER_TUNNELS,
//    MAX_LAYERS
//};


class mapbuilder
{
public:
    mapbuilder();
    virtual ~mapbuilder();
    
    mapbuilder* build( void );
    bool checkFlags( mapflags fields ) { return flags.Is((u32)fields); }

    mapbuilder* updateTerrain();
    mapbuilder* updateDensityMap();
    mapbuilder* updateLayers();
    
private:
    flags32         flags;
    MapInfo_t       info;
    
    loc_t           loc_start;
    loc_t           loc_first;
    
    maplocation*    mapdata;
    
    //u32*            layers[MAX_LAYERS];
    
public:
    size            mapsize;
    u32*            terrain;
    u32*            tunnels;
    u32*            critters;
};


#endif /* mapbuilder_h */
