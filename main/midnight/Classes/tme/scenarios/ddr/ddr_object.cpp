//
//  ddr_object.cpp
//  TME
//
//  Created by Chris Wild on 11/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#if defined(_DDR_)

#include "../../baseinc/tme_internal.h"
#include "scenario_ddr.h"
#include "scenario_ddr_internal.h"

using namespace tme::scenarios ;

namespace tme {

    ddr_object::ddr_object() 
    {
    }
   
    ddr_object::~ddr_object()
    {
    }
    
    
    MXRESULT ddr_object::FillExportData ( info_t* data )
    {
        using export_t = tme::scenarios::ddr::exports::object_t;
        
        if ( mxobject::FillExportData(data) == MX_OK ) {
            
            VALIDATE_INFO_BLOCK(data,INFO_OBJECT,export_t);
    
            auto out = static_cast<export_t*>(data);
            out->type = type;
            out->power = power;
            
            return MX_OK;
        }
        return MX_FAILED;
    }

}

#endif // _DDR_
