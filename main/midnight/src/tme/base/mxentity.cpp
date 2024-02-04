/*
 * FILE:    mxentity.cpp
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
#include <string>

namespace tme {

    mxentity::mxentity(void) :
        user_data(nullptr),
        idType(IDT_NONE),
        id(0)
    {
    }

    mxentity::~mxentity(void)
    {
    }

    MXRESULT mxentity::FillExportData ( info_t* data )
    {

        return MX_FAILED ;
    }
    
    u32 mxentity::SafeId(const mxentity* entity)
    {
        if ( entity==nullptr )
            return 0;
        return entity->Id();
    }

    mxid mxentity::SafeIdt(const mxentity* entity)
    {
        if ( entity==nullptr )
            return IDT_NONE;
        return MAKE_ID(entity->IdType(),entity->Id());
    }

    void mxentity::Serialize ( archive& ar )
    {
        if ( ar.IsStoring() ) {
            ar << symbol;
            ar << flags ;
        }else{
            ar >> symbol;
            ar >> flags ;
        }
    }

    archive& operator<<(archive& ar, mxentity* item)
    {
        return ar << ((u32)mxentity::SafeIdt(item));
    }

    archive& operator>>( archive& ar, mxentity*& item)
    {
    u32 temp;
        ar >> temp ; 
        item = (mxentity*)mx->EntityByIdt(temp);
        return ar ;
    }

    int mxentity::Compare ( const mxentity* o, int hint ) const
    {
        return Id() > (s32)o->Id();
    }

}
// namespace tme
