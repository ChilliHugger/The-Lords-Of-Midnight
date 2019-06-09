//
//  mapbuilder.cpp
//  midnight
//
//  Created by Chris Wild on 09/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "mapbuilder.h"
#include "../system/tmemanager.h"

static int density_mapping[] = { 10, 6, 11, 7, 14, 2, 15, 3, 9, 5, 8, 4, 13, 1, 12, 0 };

#define CELL(x1,y1) \
    (mapdata+((y1)*mapsize.cx)+(x1))

mapbuilder::mapbuilder() :
    tunnels(nullptr),
    terrain(nullptr),
    critters(nullptr),
    mapdata(nullptr),
    mapsize(0,0)
{
}

mapbuilder::~mapbuilder()
{
    SAFEFREE(tunnels);
    //SAFEFREE(terrain);
    SAFEFREE(critters);
    SAFEFREE(mapdata);
}

mapbuilder* mapbuilder::build ( void )
{
    mapsize = TME_MapSize();
    
    if ( !TME_MapInfo(&info) )
        return nullptr;
    
    if ( checkFlags(mapflags::debug_map) ) {
        info.top = loc_t(0,0) ;
        info.bottom = loc_t(mapsize.cx,mapsize.cy);
        info.size = mapsize ;
    }
    
    loc_start = info.top;
    
    loc_start -= loc_t(2,2);
    if ( loc_start.x<0 ) loc_start.x=0;
    if ( loc_start.y<0 ) loc_start.y=0;
    
    info.bottom += loc_t(2,2);
    if ( info.bottom.x>mapsize.cx ) info.bottom.x=mapsize.cx;
    if ( info.bottom.y>mapsize.cy ) info.bottom.y=mapsize.cy;
    
    mapsize.cx = info.bottom.x - loc_start.x;
    mapsize.cy = info.bottom.y - loc_start.y;
    
    // we need some memory to store the map
    SAFEFREE(mapdata);
    SAFEFREE(terrain);
    SAFEFREE(tunnels);
    SAFEFREE(critters);
    
    u32 size = mapsize.cx * mapsize.cy;
    
    mapdata = static_cast<maplocation*>(malloc(size*sizeof(maplocation)));
    terrain = static_cast<u32*>(malloc(size*sizeof(u32)));
    critters = static_cast<u32*>(malloc(size*sizeof(u32)));
    tunnels = static_cast<u32*>(malloc(size*sizeof(u32)));
    
    updateTerrain();
    
    updateDensityMap();
    
    updateLayers();
    
    return this;
}

mapbuilder* mapbuilder::updateTerrain()
{
    maplocation m;
    maplocation* dst = mapdata ;
    
    /*
     Discover map uses the following flags
        lf_seen             // 5
        lf_visited          // 16
        lf_looked_at        // 17
        lf_tunnel_looked_at // 18
        lf_tunnel_visited   // 19
     */
    
    bool debug_map = checkFlags(mapflags::debug_map);
    
    u32 reset_discover_mask = (lf_seen|lf_visited|lf_looked_at|lf_tunnel_looked_at|lf_tunnel_visited);
    
    // grab terrain
    for  ( int y=0; y<mapsize.cy; y++ ) {
        for ( int x=0; x<mapsize.cx; x++ ) {
            TME_GetLocation(m,mxgridref(loc_start.x+x,loc_start.y+y));
            
            dst->terrain = m.terrain ;
            dst->density=0;
            dst->tunnel=0;
            dst->flags=m.flags;
            dst->object=GET_ID(m.object);
            dst->object_tunnel = GET_ID(m.object_tunnel);
            dst->discovery_flags = m.discovery_flags ;
            dst->discovery_flags.Reset(~reset_discover_mask);
            dst->area = m.area ;
            
            if ( dst->flags.Is(lf_seen) || dst->discovery_flags.Is(lf_seen) || debug_map )
                dst->discovery_flags.Set(cf_visible);
            
            dst++;
        }
    }
    
    return this;
}


mapbuilder* mapbuilder::updateDensityMap()
{
    maplocation* dst = mapdata ;
    maplocation* src=NULL;

    u8 density=0;
    
    bool debug_map = checkFlags(mapflags::debug_map);
    
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
                dst->density = ((dst->density/4) * 16 ) + (dst->density%4);
                
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
                dst->density = ((dst->density/4) * 16 ) + (dst->density%4);

#undef COMPARE
            }
            
            
#if defined(_DDR_)
            //
            // Check GFX type of tunnel
            //
            density=15;
            
            if ( dst->flags.Is(lf_tunnel_looked_at) || ( dst->flags.Is(lf_tunnel) && debug_map ) ) {
                
                int flags=lf_tunnel_visited;
                if ( dst->flags.Is(lf_tunnel_visited))
                    flags = lf_tunnel_looked_at;
                
#define COMPARE(xx,yy,v) \
src = CELL(xx,yy); \
if ( src->flags.Is(flags) || src->flags.Is(lf_tunnel) || debug_map ) \
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
                dst->tunnel = ((dst->tunnel/4) * 16 ) + (dst->tunnel%4);

#undef COMPARE
            }
#endif
        }
        
    }
    
    return this;
}


mapbuilder* mapbuilder::updateLayers()
{
 
    
    int px,py=0;
    int x,y=0;
    
    loc_first.x = 0 ;
    loc_first.y = 0;
    
    int lastx = mapsize.cx+1;
    int lasty = mapsize.cy+1;
    
    u32 size = mapsize.cx * mapsize.cy;

    memset(terrain, 0, size);
    memset(tunnels, 0, size);
    memset(critters, 0, size);
    
//    for  ( py=0,y=loc_first.y; y<lasty; y++ ) {
//        for ( px=0,x=loc_first.x; x<lastx; x++ ) {
//
//            m = CELL(x,y);
//
//        }
//    }

    
    maplocation*    m = mapdata;
    terraininfo t;
    
    for ( int ii=0; ii<size; ii++ ) {
        terrain[ii] = 0;
        
        
        if ( m->discovery_flags & cf_visible ) {
            TME_GetTerrainInfo(t,MAKE_ID(IDT_TERRAININFO,m->terrain));
            terrain_data_t* d = (terrain_data_t*)t.userdata ;

            terrain[ii] = d->mapdensity ? d->mapcell + m->density : d->mapcell ;
        } else {
            terrain[ii] = (8*16) + m->density + 1 ;
        }
        m++;
    }
    
    
    
    return this;
}

