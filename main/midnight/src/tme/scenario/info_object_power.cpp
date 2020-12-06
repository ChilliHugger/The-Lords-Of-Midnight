/*
 * FILE:    info_object_power.cpp
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
    
    
    mxobjectpower::mxobjectpower()
    {
        mxentity::type = IDT_OBJECT_POWER ;
    }
    
    mxobjectpower::~mxobjectpower()
    {
    }
    
    void mxobjectpower::Serialize ( archive& ar )
    {
        mxinfo::Serialize ( ar );
        if ( ar.IsStoring() ) {
        }else{
        }
    }
    
    MXRESULT mxobjectpower::FillExportData ( info_t* data )
    {
        return MX_NOT_IMPLEMENTED;
    }
    
}

// namespace tme

#endif