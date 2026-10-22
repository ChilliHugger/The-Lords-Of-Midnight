/*
 * FILE:    mxfragment.cpp
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


namespace tme {

    void mxfragment::Serialize ( archive& ar )
    {
        if ( ar.IsStoring() ) {

            ar << (u32)type ;
            ar << location ;
            ar << day ;
            ar << (u32)(counted ? 1 : 0) ;
            ar << total ;
            ar << lord ;

        }else{

            u32 temp;
            ar >> temp ;        type = (mxmemory_type_t)temp ;
            ar >> location ;
            ar >> day ;
            ar >> temp ;        counted = temp != 0 ;
            ar >> total ;
            ar >> lord ;

        }
    }

    u32 mxfragment::Age() const
    {
        return sv_days > day ? sv_days - day : 0;
    }

    mxmemory_size_t mxfragment::Size() const
    {
        if ( type != MT_ARMY || !counted )
            return MS_UNKNOWN;
        if ( total <= SMALL_MAX )
            return MS_SMALL;
        if ( total <= MEDIUM_MAX )
            return MS_MEDIUM;
        return MS_LARGE;
    }

}
// namespace tme
