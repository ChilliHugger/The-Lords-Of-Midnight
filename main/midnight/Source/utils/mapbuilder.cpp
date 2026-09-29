//
//  mapbuilder.cpp
//  midnight
//
//  Created by Chris Wild on 09/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "../system/moonring.h"

#include "mapbuilder.h"
#include "../system/tmemanager.h"
#include "../system/resolutionmanager.h"
#include "../extensions/TMXTiledMap.h"
#include "../tme/tme_interface.h"

USING_NS_AX;
USING_NS_AX_UI;

static int density_mapping[] = { 10, 6, 11, 7, 14, 2, 15, 3, 9, 5, 8, 4, 13, 1, 12, 0 };

const s32 CELL_FIRST_TUNNEL             = (0*16) + 13;
const s32 CELL_FIRST_OBJECT             = (8*16) + 4;
const s32 CELL_FIRST_INCOMPLETE_TUNNEL  = (1*16) + 9;
const s32 CELL_FIRST_BLANK_TERRAIN      = (8*16) + 0;
const s32 CELL_FIRST_TUNNEL_END         = (0*16) + 9;
const s32 CELL_BLANK                    = (3*16) + 1;

#define INDEX(x1,y1)        (((y1)*mapsize.cx)+(x1))
#define CELL(x1,y1)         (mapdata+INDEX(x1,y1))
#define REMAP_DENSITY(x)    ((x/4) * 16 ) + (x%4)


map_object::~map_object()
{
    here.clear();
}


mapbuilder::mapbuilder() :
    tunnels(nullptr),
    terrain(nullptr),
    terrain_discovery(nullptr),
    critters(nullptr),
    tunnel_critters(nullptr),
    mapdata(nullptr),
    mapsize(0,0),
    truesize(0,0),
    screenTiles(0,0),
    screenAspect(1.0f),
    borderPadding(2)
{
    setFlags(mapflags::show_characters);
    setFlags(mapflags::show_critters);
    setFlags(mapflags::show_tunnels);
}

//void checkLeak(Vector<map_object*> &objects)
//{
//    for( auto o : objects)
//    {
//        if(o->getReferenceCount() > 1)
//        {
//            UIDEBUG("[LEAK] %x : %s = %d", o->id, TME_GetSymbol(o->id), o->getReferenceCount());
//        }
//    }
//
//}

mapbuilder::~mapbuilder()
{
    SAFEFREE(tunnels);
    SAFEFREE(terrain);
    SAFEFREE(terrain_discovery);
    SAFEFREE(critters);
    SAFEFREE(tunnel_critters);
    SAFEFREE(mapdata);
    
    drainCollection(characters);
    drainCollection(regiments);
    drainCollection(strongholds);
    drainCollection(places);
    
//    checkLeak(characters);
//    checkLeak(regiments);
//    checkLeak(strongholds);
//    checkLeak(places);
}

void mapbuilder::drainCollection(Vector<map_object*> &objects)
{
    for( auto o : objects)
    {
        o->here.clear();
    }
    objects.clear();
}


mapbuilder* mapbuilder::build ( void )
{
    mapsize = TME_MapSize();
    truesize = mapsize;

    if ( !TME_MapInfo(&info) )
        return nullptr;

    if ( isDebugMap() ) {
        info.top = loc_t(0,0) ;
        info.bottom = loc_t(mapsize.cx,mapsize.cy);
        info.size = mapsize ;
    }else{

        // discoverymap/gamemap Top()/Bottom() are both INCLUSIVE of the last seen/
        // visible location, unlike every other loc/size pair in this file (where
        // bottom is one-past-the-end) - convert once here so a character standing
        // right at the edge of the discovered area isn't cropped out downstream
        info.bottom.x += 1;
        info.bottom.y += 1;
        info.size.cx += 1;
        info.size.cy += 1;

        // pad the visible/discovered area out to match the screen's aspect ratio,
        // centring it, so unmapped space is built into the map itself as CELL_BLANK
        // terrain - and fills the screen exactly at the fitted scale - rather than
        // needing to be faked up afterwards on screen. screenTiles is also folded
        // in as a floor: early in a game the discovered area can be far smaller
        // than the screen, and the map must still be built with enough tiles to
        // cover it (rather than compensating by scaling the finished map up)
        // if the discovered area already fits within a single screen, the whole
        // map will be shown at once with no scrolling - so there's nothing for
        // the scroll-margin border below to help with, and it can be skipped
        bool needsScrollMargin = (info.size.cx > screenTiles.cx) || (info.size.cy > screenTiles.cy);

        u32 minWidth  = MAX(info.size.cx, screenTiles.cx);
        u32 minHeight = MAX(info.size.cy, screenTiles.cy);
        u32 targetWidth  = MAX(minWidth, (u32)ceil(minHeight*screenAspect));
        u32 targetHeight = MAX(minHeight, (u32)ceil(minWidth/screenAspect));

        if ( info.size.cx < targetWidth ) {
            u32 diffx = targetWidth - info.size.cx;
            u32 halfx = diffx/2;
            info.size.cx = targetWidth;
            info.bottom.x += halfx;
            info.top.x -= (diffx-halfx);

        }
        if ( info.size.cy < targetHeight ) {
            u32 diffy = targetHeight - info.size.cy;
            u32 halfy = diffy/2;
            info.size.cy = targetHeight;
            info.bottom.y += halfy;
            info.top.y -= (diffy-halfy);
        }

        // slide the (possibly padded) window back within the real map's bounds -
        // otherwise locations beyond the true edge come back as the world border,
        // which would show up as a solid band rather than blending in as unmapped
        if ( info.top.x < 0 ) { info.bottom.x -= info.top.x; info.top.x = 0; }
        if ( info.bottom.x > (s32)truesize.cx ) { info.top.x -= (info.bottom.x-(s32)truesize.cx); info.bottom.x = truesize.cx; }
        if ( info.top.x < 0 ) info.top.x = 0;

        if ( info.top.y < 0 ) { info.bottom.y -= info.top.y; info.top.y = 0; }
        if ( info.bottom.y > (s32)truesize.cy ) { info.top.y -= (info.bottom.y-(s32)truesize.cy); info.bottom.y = truesize.cy; }
        if ( info.top.y < 0 ) info.top.y = 0;

        // add a fixed scroll margin around the fitted window so the player can
        // scroll content that would otherwise sit under the map's fixed UI
        // (zoom/filter buttons) clear of it. Added after the slide-to-bounds
        // above so it isn't absorbed/truncated by it - this is deliberately
        // allowed to extend past the true map edge, since updateTerrain()
        // clamps those locations back onto the edge and repeats its terrain
        if ( needsScrollMargin ) {
            info.top.x    -= (s32)borderPadding;
            info.top.y    -= (s32)borderPadding;
            info.bottom.x += (s32)borderPadding;
            info.bottom.y += (s32)borderPadding;
        }

        info.size.cx = info.bottom.x - info.top.x;
        info.size.cy = info.bottom.y - info.top.y;

    }

    loc_start = info.top;
    
    mapsize.cx = (info.bottom.x - loc_start.x) ;
    mapsize.cy = (info.bottom.y - loc_start.y) ;
    
    // we need some memory to store the map
    SAFEFREE(mapdata);
    SAFEFREE(terrain);
    SAFEFREE(terrain_discovery);
    SAFEFREE(tunnels);
    SAFEFREE(critters);
    SAFEFREE(tunnel_critters);
    
    u32 size = mapsize.cx * mapsize.cy;
    
    mapdata = static_cast<loc_info_t*>(malloc(size*sizeof(loc_info_t)));
    terrain = static_cast<u32*>(malloc(size*sizeof(u32)));
    terrain_discovery = static_cast<u32*>(malloc(size*sizeof(u32)));
    critters = static_cast<u32*>(malloc(size*sizeof(u32)));
    tunnels = static_cast<u32*>(malloc(size*sizeof(u32)));
    tunnel_critters = static_cast<u32*>(malloc(size*sizeof(u32)));
    
    updateTerrain();
    
    updateDensityMap();
    
    updateLayers();
    
    updateCharacters();
    
    updateRegiments();
    
    updateStrongholds();
    
    updatePlaces();

#if defined(_DDR_)
    if ( isDebugMap() )
        updateSpecialObjects();
#endif
    
    return this;
}

mapbuilder* mapbuilder::updateTerrain()
{
    maplocation m;
    loc_info_t* dst = mapdata ;
    
    /*
     Discover map uses the following flags
        lf_seen             // 5
        lf_visited          // 16
        lf_looked_at        // 17
        lf_tunnel_looked_at // 18
        lf_tunnel_visited   // 19
     */
    
    bool debug_map = isDebugMap();

    u32 reset_discover_mask = (lf_seen|lf_visited|lf_looked_at|lf_tunnel_looked_at|lf_tunnel_visited);
    
    // grab terrain
    for  ( int y=0; y<mapsize.cy; y++ ) {
        for ( int x=0; x<mapsize.cx; x++ ) {
            
            loc_t loc(loc_start.x+x,loc_start.y+y);

            // locations beyond the true map edge (the scroll-margin border) are
            // clamped back onto the edge, so they pick up its terrain/seen state -
            // extending it outwards - rather than the default out-of-bounds cell
            bool extended = false;
            if ( !debug_map ) {
                loc_t clamped( MIN(MAX(loc.x,0), (s32)truesize.cx-1), MIN(MAX(loc.y,0), (s32)truesize.cy-1) );
                extended = (clamped.x != loc.x) || (clamped.y != loc.y);
                loc = clamped;
            }

            TME_GetLocation(m,loc);
            dst->flags = m.flags;
            dst->terrain = m.terrain ;
            dst->density=0;
            dst->tunnel=0;
            // don't repeat the edge cell's critters/objects/tunnels across the
            // whole extended border - only the terrain itself should stretch out
            dst->object = extended ? OB_NONE : GET_ID(m.object);
            dst->object_tunnel = extended ? OB_NONE : GET_ID(m.object_tunnel);
            if ( extended )
                dst->flags.Reset(lf_tunnel_flags_mask);
            dst->discovery_flags = m.discovery_flags ;
            dst->discovery_flags.Reset(~reset_discover_mask);
            if ( extended )
                dst->discovery_flags.Reset(lf_tunnel_flags_mask);
            dst->area = m.area ;
            dst++;
            
            if ( m.flags.Is(lf_looked_at) || m.flags.Is(lf_visited)
                || m.discovery_flags.Is(lf_looked_at) || m.discovery_flags.Is(lf_visited)
                || debug_map ) {
                if ( ( (m.flags.Is(lf_stronghold) && !m.flags.Is(lf_domain)) || (m.terrain == TN_TOWER || m.terrain == TN_WATCHTOWER || m.terrain == TN_GATE ) ) ) {
                    auto place = new map_object();
                    place->id = IDT_LOCATION;
                    place->location = loc;
                    place->visible = (m.flags.Is(lf_looked_at) || m.flags.Is(lf_visited) || m.flags.Is(lf_seen))
                        && (m.discovery_flags.Is(lf_looked_at) || m.discovery_flags.Is(lf_visited));
                    places.pushBack(place);
                    place->release();
                }
            }
        }
    }
    
    return this;
}


mapbuilder* mapbuilder::updateDensityMap()
{
    loc_info_t* dst = nullptr ;
    loc_info_t* src = nullptr;

    u8 density=0;
    
    bool debug_map = isDebugMap();
    
    // calculate gfx type
    for  ( int y=0; y<mapsize.cy; y++ ) {
        for ( int x=0; x<mapsize.cx; x++ ) {
            
            dst = CELL(x,y) ;
            
            density = 0;
        
            //
            // Check GFX type of terrain
            //
            
            if ( dst->flags.Is(lf_seen) || dst->discovery_flags.Is(lf_seen) || debug_map ) {
                
#define COMPARE(xx,yy,v) \
    src = CELL(xx,yy); \
    if ( src->terrain!=dst->terrain || (!src->flags.Is(lf_seen) && !src->discovery_flags.Is(lf_seen) && !debug_map) ) \
                density += v;
                
                // check above
                if ( y>0 ) {
                    COMPARE(x,y-1,1);
                }else
                    density+=1;
                
                // check down
                if ( y<mapsize.cy-1 ) {
                    COMPARE(x,y+1,4);
                }else
                    density+=4;
                
                // check left
                if ( x>0 ) {
                    COMPARE(x-1,y,8);
                }else
                    density+=8;
                
                // check right
                if ( x<mapsize.cx-1 ) {
                    COMPARE(x+1,y,2);
                }else
                    density+=2;
                
                dst->density = density_mapping[ density ];
                dst->density = REMAP_DENSITY(dst->density);
                
#undef COMPARE
                
            }else{
        
#define COMPARE(xx,yy,v) \
src = CELL(xx,yy); \
if ( src->flags.Is(lf_seen) || src->discovery_flags.Is(lf_seen) || debug_map ) \
    density += v;
                
                // check above
                if ( y>0 ) {
                    COMPARE(x,y-1,1);
                }
                // check down
                if ( y<mapsize.cy-1 ) {
                    COMPARE(x,y+1,4);
                }
                // check left
                if ( x>0 ) {
                    COMPARE(x-1,y,8);
                }
                // check right
                if ( x<mapsize.cx-1 ) {
                    COMPARE(x+1,y,2);
                }
                
                dst->density = density_mapping[ density ];
                dst->density = REMAP_DENSITY(dst->density);

#undef COMPARE
            }
            
            //
            // Check GFX type of tunnel
            //
            density=15;
            
            if ( dst->flags.Is(lf_tunnel_looked_at) || dst->discovery_flags.Is(lf_tunnel_visited) || ( dst->flags.Is(lf_tunnel) && debug_map ) ) {
                
                int flags=lf_tunnel_visited;
                if ( dst->flags.Is(lf_tunnel_visited))
                    flags = lf_tunnel_looked_at;
                
#define COMPARE(xx,yy,v) \
src = CELL(xx,yy); \
if ( src->flags.Is(flags) || src->discovery_flags.Is(flags) || (src->flags.Is(lf_tunnel) && debug_map) ) \
    density -= v;
                
                // check above
                if ( y>0 ) {
                    COMPARE(x,y-1,1);
                }
                // check down
                if ( y<mapsize.cy-1 ) {
                    COMPARE(x,y+1,4);
                }
                // check left
                if ( x>0 ) {
                    COMPARE(x-1,y,8);
                }
                // check right
                if ( x<mapsize.cx-1 ) {
                    COMPARE(x+1,y,2);
                }
                
                dst->tunnel = density_mapping[ density ];

#undef COMPARE
            }
        }
        
    }
    
    return this;
}


mapbuilder* mapbuilder::updateLayers()
{

    loc_first.x = 0 ;
    loc_first.y = 0;
    
    max_cells = mapsize.cx * mapsize.cy;

    memset(terrain, 0, max_cells);
    memset(terrain_discovery,0,max_cells);
    memset(tunnels, 0, max_cells);
    memset(critters, 0, max_cells);
    memset(tunnel_critters, 0, max_cells);

    bool debug_map = isDebugMap();
    bool show_tunnels = checkFlags(mapflags::show_tunnels);
    bool show_critters = checkFlags(mapflags::show_critters);
    bool show_all_critters = checkFlags(mapflags::show_all_critters);
    
    loc_info_t*    m = mapdata;
    terraininfo t;
    object o;
    
    for ( int ii=0; ii<max_cells; ii++ ) {
        terrain[ii] = 0;
        critters[ii] = 0;
        tunnel_critters[ii] = 0;
        tunnels[ii]=0;
        terrain_discovery[ii]=0;

        bool seen = m->flags.Is(lf_seen) || debug_map ;
        bool visited = m->flags.Is(lf_visited);
        bool looked_at = m->flags.Is(lf_looked_at);
        bool discovery_seen = m->discovery_flags.Is(lf_seen) ;
        bool discovery_visited = m->discovery_flags.Is(lf_visited);
        
        bool tunnelseen = m->flags.Is(lf_tunnel_looked_at) || debug_map;
        bool tunnelvisited = m->flags.Is(lf_tunnel_visited) || debug_map;
        
        bool discovery_tunnelseen =  m->discovery_flags.Is(lf_tunnel_looked_at);
        bool discovery_tunnelvisited = m->discovery_flags.Is(lf_tunnel_visited) ;
        bool tunnelobject = m->flags.Is(lf_tunnel_object);

        bool visible = seen || visited || discovery_seen || discovery_visited;
              
        mxthing_t thing = (mxthing_t)m->object ;
        if ( m->object_tunnel != OB_NONE && show_tunnels  )
            thing = (mxthing_t)m->object_tunnel ;

        if ( visible ) {
            
            // Terrain
            
            TME_GetTerrainInfo(t,MAKE_ID(IDT_TERRAININFO,m->terrain));
            terrain_data_t* d = (terrain_data_t*)t.userdata ;

            u32 cell = d == nullptr ? CELL_BLANK
                     : d->mapdensity ? d->mapcell + m->density : d->mapcell ;
            
            
            if ( seen ) {
                terrain[ii] = cell ;
            }else {
                if ( discovery_seen )
                    terrain_discovery[ii] = cell;
                terrain[ii] = CELL_BLANK;
            }
                
            
        } else {
            terrain[ii] = CELL_FIRST_BLANK_TERRAIN + m->density + 1 ;
        }
        
        mxthing_t show_thing = OB_NONE;
        
        // critters
        bool show_critter = ( tunnelobject && (tunnelseen||tunnelvisited) )
            || (!tunnelobject && (seen && (visited || looked_at))) ;
            
        if ( (show_critter || show_all_critters ) && show_critters  )  {
            show_thing = thing;
        }
        
        if ( !debug_map && show_thing > OB_WILDHORSES )
            show_thing = OB_NONE;
        
        if ( show_thing ) {
            TME_GetObject(o,MAKE_ID(IDT_OBJECT,show_thing));
            obj_data_t* d = (obj_data_t*)o.userdata ;
            if ( d ) {
                if ( show_thing == (mxthing_t)m->object_tunnel ) {
                    tunnel_critters[ii] = d->mapcell ;
                }else
                {
                    critters[ii] = d->mapcell ;
                }
            }
        }
                
        BOOL tunnel_visited = m->flags.Is(lf_tunnel_visited) || ( m->flags.Is(lf_tunnel) && debug_map ) || discovery_tunnelvisited ;
        BOOL tunnel_looked_at = m->flags.Is(lf_tunnel_looked_at) | discovery_tunnelseen  ;
        
        
        if ( (tunnel_visited||tunnel_looked_at) && show_tunnels ) {
            int d=m->tunnel;
            int tunnel_end = -1;
            
            // top = 0 / 1
            // left= 1 / 0
            // right=2 / 2
            // bottom=7 / 3
            if ( d == 3 ) // right
                tunnel_end=2;
            else if ( d == 1) // left
                tunnel_end=0;
            else if ( d == 4) // top
                tunnel_end=1;
            else if ( d == 12) // bottom
                tunnel_end=3;
            
            if ( m->flags.Is(lf_tunnel_entrance|lf_tunnel_exit)) {
                // tunnel ends
                if ( tunnel_end != -1 )
                    tunnels[ii] = CELL_FIRST_TUNNEL_END + tunnel_end;
            } else {
                if ( tunnel_visited ) {
                    tunnels[ii] = CELL_FIRST_TUNNEL + REMAP_DENSITY(d);
                } else
                    if ( tunnel_end!=-1 )
                        tunnels[ii] = CELL_FIRST_INCOMPLETE_TUNNEL + tunnel_end;
            }
            
        }
        
        m++;
    }
    
    return this;
}

loc_t mapbuilder::normaliseLocation( loc_t loc )
{
    return loc_t( loc.x-loc_start.x, loc.y - loc_start.y);
}

Vec2 mapbuilder::convertToPosition( loc_t loc )
{
    loc = normaliseLocation(loc);
    return Vec2( loc.x*RES(64), loc.y*RES(64) );
}


#if defined(_DDR_)
mapbuilder* mapbuilder::updateSpecialObjects()
{
    character c;
    object o;
    
    c_mxid objects;
    TME_GetAllObjects(objects);
    
    for( auto id : objects ) {
        TME_GetObject(o, id);

        CONTINUE_IF( !(o.flags.Is(of_unique)));

        loc_t loc = o.location;
        
        if ( o.carriedby != IDT_NONE ) {
            TME_GetCharacter(c, o.carriedby);
            loc = c.location;
        }
        
        CONTINUE_IF ( loc.x == 0 && loc.y == 0 );
        
        loc = normaliseLocation(loc);
        
        auto userdata = static_cast<obj_data_t*>(o.userdata) ;
        u32 cell = IS_NOT_NULL(userdata, userdata->mapcell, CELL_FIRST_OBJECT + o.type);
        
        int index = INDEX(loc.x,loc.y);
        if ( index < max_cells ) {
            //if ( mapdata[index].flags & lf_seen)
                critters[ index ] = cell ;
        }
    }
    
    
    return this;
}
#endif


mapbuilder* mapbuilder::updateCharacters()
{

    bool show_all_characters = checkFlags(mapflags::show_all_characters);
    
    // get lords
    c_mxid tme_characters;
    TME_GetAllCharacters(tme_characters);
    
    characters.clear();
    
    for ( auto id : tme_characters ) {
        maplocation m;
        character c;
        
        TME_GetCharacter(c, id );
        
        CONTINUE_IF(!Character_IsAlive(c));
        CONTINUE_IF ( !show_all_characters && !Character_IsControllable(c.id));
        CONTINUE_IF ( !show_all_characters && !Character_IsRecruited(c) );
        
        TME_GetLocation(m,c.location);
        
        bool seen = show_all_characters || m.flags&lf_seen ;
        CONTINUE_IF ( !seen && !Character_IsInTunnel(c));

        bool visited = (m.flags & (lf_looked_at|lf_visited|lf_tunnel_visited|lf_tunnel_looked_at));
        CONTINUE_IF ( !(show_all_characters || Character_IsRecruited(c) || visited) );
        
        auto object = new map_object();
        
        //object->selectable=FALSE;
        object->id = c.id ;
        object->location = c.location ;
//        object->soldiers = c.warriors+c.riders;
//        object->r = rect::EMPTY ;
//        object->selected = false ;
//        object->selectable=Character_IsRecruited(c) || show_all_characters;
//        object->multiple=false;
//#if defined(_DDR_)
//        object->tunnel=Character_IsInTunnel(c);
//        object->homelocation = c.homelocation;
//        object->lastlocation = c.lastlocation ;
//        object->targetlocation = c.targetlocation;
//#endif

        
        // at the same location
        for( auto o : characters ) {
            if ( o->location == object->location ) {
                o->here.pushBack(object);
                object->here.pushBack(o);
            }
        }
        
        characters.pushBack(object);
        object->release();
        
    }
    
    return this;
}

mapbuilder* mapbuilder::updateRegiments()
{
    regiment r;
    
    regiments.clear();
    
    if  (!checkFlags(mapflags::show_enemy_armies) )
       return this;
       
    // get armies
    c_mxid tme_regiments;
    TME_GetAllRegiments(tme_regiments);
    
    for ( auto id : tme_regiments ) {
        TME_GetRegiment(r, id );
        
        CONTINUE_IF( r.total == 0 );
        
        auto object = new map_object();
        regiments.pushBack(object);
        object->release();
        
        object->id = r.id ;
        object->location = r.location ;
//        object->soldiers = r.total ;
//        object->type = r.type;
//        object->lastlocation = r.lastlocation ;
//        object->selectable = false;
//        object->r = rect::EMPTY ;
//        object->targetlocation = r.targetlocation ;
//
    }

    return this;
}

mapbuilder* mapbuilder::updateStrongholds()
{
    stronghold s;
    
    // get armies
    c_mxid tme_strongholds;
    TME_GetAllStrongholds(tme_strongholds);

    for ( auto id : tme_strongholds ) {
        TME_GetStronghold(s, id );
        CONTINUE_IF ( s.loyalty != RA_MOONPRINCE );
        
        auto object = new map_object();
        strongholds.pushBack(object);
        object->release();
        
        object->id = s.id ;
        object->location = s.location ;
//        object->selectable = false;
//        object->r = rect::EMPTY ;
    }
 
    return this;
}

mapbuilder* mapbuilder::updatePlaces()
{
    for ( auto p : places ) {
        for( auto o : characters ) {
            if ( o->location == p->location ) {
                p->here.pushBack(o);
            }
        }
    }
    return this;
}

void mapbuilder::clearLayers()
{
    critters = nullptr;
    tunnel_critters = nullptr;
    tunnels = nullptr;
    terrain = nullptr;
    terrain_discovery = nullptr;
}
