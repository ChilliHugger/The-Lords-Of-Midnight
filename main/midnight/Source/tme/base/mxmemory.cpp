/*
 * FILE:    mxmemory.cpp
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
#include <algorithm>


namespace tme {

    void mxmemory::Clear ( void )
    {
        fragments.clear();
    }

    void mxmemory::Update ( void )
    {
        fragments.erase(
            std::remove_if( fragments.begin(), fragments.end(),
                [](const mxfragment& f) { return f.Age() > sv_character_default_memory_age; } ),
            fragments.end() );
    }

    // Anything remembered at the location that is no longer there is forgotten, but a distant
    // sighting never loses a count taken earlier close up.
    void mxmemory::SeeLocation ( mxgridref loc, bool close, u32 army, const std::vector<u32>& lords )
    {
        auto existing = std::find_if( fragments.begin(), fragments.end(),
            [&](const mxfragment& f) { return f.type == MT_ARMY && f.location == loc; } );

        if ( army == 0 ) {
            if ( existing != fragments.end() )
                fragments.erase(existing);
        }else{
            if ( existing == fragments.end() ) {
                fragments.emplace_back();
                existing = fragments.end() - 1;
                existing->type = MT_ARMY;
                existing->location = loc;
            }
            existing->day = sv_days;
            if ( close ) {
                existing->counted = true;
                existing->total = army;
            }
        }

        // lords can't be seen from a distance
        if ( !close )
            return;

        fragments.erase(
            std::remove_if( fragments.begin(), fragments.end(),
                [&](const mxfragment& f) {
                    return f.type == MT_LORD && f.location == loc
                        && std::find(lords.begin(), lords.end(), f.lord) == lords.end();
                } ),
            fragments.end() );

        for ( auto id : lords ) {
            auto fragment = std::find_if( fragments.begin(), fragments.end(),
                [&](const mxfragment& f) { return f.type == MT_LORD && f.lord == id; } );
            if ( fragment == fragments.end() ) {
                fragments.emplace_back();
                fragment = fragments.end() - 1;
                fragment->type = MT_LORD;
                fragment->lord = id;
            }
            fragment->location = loc;
            fragment->day = sv_days;
        }
    }

    void mxmemory::ForgetLord ( u32 id )
    {
        fragments.erase(
            std::remove_if( fragments.begin(), fragments.end(),
                [&](const mxfragment& f) { return f.type == MT_LORD && f.lord == id; } ),
            fragments.end() );
    }

    const mxfragment* mxmemory::ArmyAt ( mxgridref loc ) const
    {
        for ( auto& f : fragments ) {
            if ( f.type == MT_ARMY && f.location == loc )
                return &f;
        }
        return nullptr;
    }

    const mxfragment* mxmemory::LordFragment ( u32 id ) const
    {
        for ( auto& f : fragments ) {
            if ( f.type == MT_LORD && f.lord == id )
                return &f;
        }
        return nullptr;
    }

    void mxmemory::Serialize ( archive& ar )
    {
        if ( ar.IsStoring() ) {

            ar << (u32)fragments.size() ;
            for ( auto& f : fragments )
                f.Serialize(ar);

        }else{

            u32 count;
            ar >> count ;

            fragments.clear();
            fragments.resize(count);
            for ( auto& f : fragments )
                f.Serialize(ar);

        }
    }

}
// namespace tme
