//
//  selectmodel.cpp
//  midnight
//
//  Created by Chris Wild on 09/12/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#include "selectmodel.h"
#include "../system/tmemanager.h"
#include "../system/configmanager.h"
#include "../tme_interface.h"

using namespace chilli::lib;
using namespace tme;

void selectmodel::setDefaults()
{
    // all filters on
    filters.Set(select_filters::all);
    
    characters.Clear();
    
    page = 0;
    
    upgraded = true;
}

void selectmodel::updateCharacters()
{
    variant args[3];
    args[0] = &characters ;
    //args[1] = CMDG_ALL;
    args[1] = CMDG_LOYAL ;
    args[2] = MAKE_LOCID(loc.x,loc.y) ;
    mxi->GetProperties( "CharsForCommand", args, 3 );
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
                
                // Version 14
                ar << ud->select_page;
                
            }
        
        // Version 11
        ar << filters ;
        
        // Version 15
        ar << page;
        
    }else{
        
        upgraded =  ( version >= NEW_SAVE_GAME_VERSION );
        
        ar >> count ;
        for ( u32 ii=0; ii<count; ii++ ) {
            ar >> id ;
            TME_GetCharacter(c, id ) ;
            
            char_data_t* ud = (char_data_t*)c.userdata;
            ar >> ud->select_loc;
            
            if ( version >= 14 )
                ar >> ud->select_page;
            else
                ud->select_page = InvalidPage;
        }
        
        if ( version >=11 )
            ar >> filters ;
        
        if ( version >= 16 )
            ar >> page;
    }
    
}
