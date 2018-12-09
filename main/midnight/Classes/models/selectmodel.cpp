//
//  selectmodel.cpp
//  midnight
//
//  Created by Chris Wild on 09/12/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#include "selectmodel.h"
#include "../system/tmemanager.h"
#include "../tme_interface.h"

using namespace chilli::lib;

void selectmodel::setDefaults()
{
    // all filters on
    filters.Set(0xffffffff);
    
    characters.Clear();
}

void selectmodel::serialize( u32 version, lib::archive& ar )
{
    
    character   c;
    u32         count=0;
    mxid        id;
    
    
    if ( ar.IsStoring() ) {
        
            ar << characters.Count();
            
            for ( u32 ii=0; ii<characters.Count(); ii++ ) {
                
                TME_GetCharacter(c, characters[ii]) ;
                char_data_t* ud = (char_data_t*)c.userdata;
                
                ar << characters[ii]  ;
                ar << ud->select_loc;
            }
        
        ar << filters ;
        
        
    }else{
        
        ar >> count ;
        for ( u32 ii=0; ii<count; ii++ ) {
            ar >> id ;
            TME_GetCharacter(c, id ) ;
            char_data_t* ud = (char_data_t*)c.userdata;
            
            point p;
            
            ar >> p;
            ud->select_loc = p ;
        }
        
        if ( version >=11 )
            ar >> filters ;
        
        
    }
    
}
