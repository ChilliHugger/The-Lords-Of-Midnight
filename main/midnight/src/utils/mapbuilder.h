//
//  mapbuilder.h
//  midnight
//
//  Created by Chris Wild on 09/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef mapbuilder_h
#define mapbuilder_h

#include "../cocos.h"
#include "../library/inc/mxtypes.h"
#include "../tme/tme_interface.h"

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

class map_object : public cocos2d::Ref
{
    template<class T> using Vector = cocos2d::Vector<T>;
    
public:
    virtual ~map_object() override;
    
public:
    mxid        id;
    loc_t       location;
    bool        visible;
    
//    loc_t       lastlocation;
//    loc_t       targetlocation;
//#if defined(_DDR_)
//    loc_t       homelocation;
//#endif
//    u32         soldiers;
//    mxunit_t    type;
//    rect        r;
    
//    BOOL        selected;
//    BOOL        selectable;
//    BOOL        multiple;
//    BOOL        popup;
//    BOOL        tunnel;
    BOOL        processed;
    
    Vector<map_object*>   here;
};


class mapbuilder : public cocos2d::Ref
{
public:
    template<class T> using Vector = cocos2d::Vector<T>;
    using Vec2 = cocos2d::Vec2;
    
public:
    mapbuilder();
    virtual ~mapbuilder() override;
    
    mapbuilder* build( void );
    bool checkFlags( mapflags fields ) { return flags.Is((u32)fields); }
    void setFlags( mapflags fields ) { flags.Set((u32)fields); }

    bool isDebugMap() { return checkFlags(mapflags::debug_map); }
    
    loc_t normaliseLocation( loc_t loc );
    Vec2 convertToPosition( loc_t loc );

    mapbuilder* updateTerrain();
    mapbuilder* updateDensityMap();
    mapbuilder* updateLayers();
    mapbuilder* updateObjects();

#if defined(_DDR_)
    mapbuilder* updateSpecialObjects();
#endif
    
    mapbuilder* updateCharacters();
    mapbuilder* updateRegiments();
    mapbuilder* updateStrongholds();
    mapbuilder* updatePlaces();

    void clearLayers();
    
    void drainCollection(Vector<map_object*> &objects);
    
private:
    flags32                 flags;
    MapInfo_t               info;
    maplocation*            mapdata;
public:
    size                    mapsize;
    size                    screensize;
    loc_t                   loc_start;
    loc_t                   loc_first;
    u32                     max_cells;
    u32*                    terrain;
    u32*                    terrain_discovery;
    u32*                    tunnels;
    u32*                    critters;
    u32*                    tunnel_critters;
    Vector<map_object*>     characters;
    Vector<map_object*>     regiments;
    Vector<map_object*>     strongholds;
    Vector<map_object*>     places;
};


#endif /* mapbuilder_h */
