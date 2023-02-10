//
//  mapmodel.cpp
//  midnight
//
//  Created by Chris Wild on 03/05/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "mapmodel.h"
#include "../frontend/panel_id.h"

void mapmodel::serialize( u32 version, lib::archive& ar )
{
    if ( ar.IsStoring() ) {

        ar << filters ;
        ar << mapscale ;
        ar << lastmapscale;
        ar << oldoffset;
        ar << currentMapPanel;

    }else{
        
        if ( version >=11 ) {
            ar >> filters ;
        }
        
        if ( version >=12 ) {
            ar >> mapscale;
            ar >> lastmapscale;
            ar >> oldoffset;
        }
        
        if ( version >= 20 ) {
            int temp;
            ar >> temp; currentMapPanel = (panelmode_t) temp;
        }
    }
}
        

void mapmodel::setDefaults ()
{
    filters.Set(map_filters::all);
    oldoffset = point::ZERO;
    mapscale = INITIAL_MAP_SCALE;
    lastmapscale = INITIAL_MAP_SCALE;
    currentMapPanel = MODE_MAP_OVERVIEW ;
}
