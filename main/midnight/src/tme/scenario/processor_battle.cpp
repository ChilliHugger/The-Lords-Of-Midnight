/*
 * FILE:    processor_battle.cpp
 * 
 * PROJECT: MidnightEngine
 * 
 * CREATED: 
 * 
 * AUTHOR:  Chris Wild
 * 
 * Copyright 2011 Chilli Hugger. All rights reserved.
 * 
 * PURPOSE: 
 * 
 * 
 */


#include "../baseinc/tme_internal.h"

#include "../baseinc/processor_battle.h"
#include "../baseinc/variables.h"

#include <algorithm>
#include <string>

namespace tme {

    void mxbattle::ProcessLocation ( mxgridref loc )
    {
    }

    void mxbattle::Process ()
    {
    }

    void mxbattle::Reset()
    {
        battles=0;
        if ( areas.Size() == 0 )
            areas.Create( sv_areas );

        areas.ClrAll();
    }
    
    void mxbattle::Announce( mxgridref loc )
    {
        int area = mx->gamemap->GetAt(loc).area;

        // The bloody sword of battle brings death in the domain of
        // ashimar and of Valethor
        mx->text->loc = loc;
        if ( battles==0 ) {
            mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookedSystemString(SS_BATTLE2)) ;
        } else {
            if ( !areas[area] )
                mx->SetLastActionMsg(mx->LastActionMsg() + mx->text->CookedSystemString(SS_BATTLE3) );
        }

        // flag a battle as taking place in this area
        if ( sv_collate_battle_areas )
            areas.Set ( area );

        mx->NightCallback(nullptr);
        battles++;
    }
}
// namespace tme
