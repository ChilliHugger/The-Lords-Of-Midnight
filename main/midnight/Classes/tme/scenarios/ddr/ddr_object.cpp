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
        
    bool ddr_object::IsSpecial() const
    {
        if (IsSymbol("OB_CROWN_VARENAND"))
            return true;
        if (IsSymbol("OB_CROWN_CARUDRIUM"))
            return true;
        if (IsSymbol("OB_SPELL_THIGRORN"))
            return true;
        if (IsSymbol("OB_RUNES_FINORN"))
            return true;
        if (IsSymbol("OB_CROWN_IMIRIEL"))
            return true;
        return FALSE;
    }
    
    MXRESULT ddr_object::FillExportData ( info_t* data )
    {
        using export_t = tme::scenarios::ddr::exports::object_t;
        
        if ( mxobject::FillExportData(data) == MX_OK ) {
            
            VALIDATE_INFO_BLOCK(data,INFO_OBJECT,export_t);
    
            auto out = static_cast<export_t*>(data);
            out->type = type;
            out->power = power;
            //out->isspecial =;
            
            return MX_OK;
        }
        return MX_FAILED;
    }
    
    void ddr_object::Serialize ( archive& ar )
    {
        mxobject::Serialize ( ar );
        
        if ( ar.IsStoring() ) {
            WRITE_ENUM(type);
            WRITE_ENUM(power);
        }else{
            if ( tme::mx->SaveGameVersion() > 10 )
            {
                READ_ENUM(type);
                READ_ENUM(power);
            }
        }
    }
    
}

#endif // _DDR_
