//
//  mapmodel.cpp
//  midnight
//
//  Created by Chris Wild on 03/05/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "mapmodel.h"

void mapmodel::serialize( u32 version, lib::archive& ar )
{
    if ( ar.IsStoring() ) {

        ar << filters ;
        ar << mapscale ;
        ar << lastmapscale;
        ar << oldoffset;

    }else{
        
        if ( version >=11 ) {
            ar >> filters ;
        }
        
        if ( version >=12 ) {
            ar >> mapscale;
            ar >> lastmapscale;
            ar >> oldoffset;
        }
    }
}
        

void mapmodel::setDefaults ()
{
    filters.Set(map_filters::all);
    filters.Reset(map_filters::show_critters);
    oldoffset = point::ZERO;
    mapscale=1.0f;
    lastmapscale=0.5f;
    
}
