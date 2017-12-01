/*
 * FILE:    info_object_type.cpp
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

#if defined(_DDR_)

namespace tme {
    
    
    mxobjecttype::mxobjecttype()
    {
        mxentity::type = IDT_OBJECT_TYPE ;
    }
    
    mxobjecttype::~mxobjecttype()
    {
    }
    
    void mxobjecttype::Serialize ( archive& ar )
    {
        mxinfo::Serialize ( ar );
        if ( ar.IsStoring() ) {
        }else{
        }
    }
    
    MXRESULT mxobjecttype::FillExportData ( info_t* data )
    {
        return MX_NOT_IMPLEMENTED;
    }
    
}
// namespace tme

#endif