/*
 * FILE:    info_terrain.cpp
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

        mxterrain::mxterrain()
        {
            mxentity::idType = IDT_TERRAININFO ;
        }

        mxterrain::mxterrain( mxterrain_t id, LPCSTR symbol )
        {
            mxentity::idType = IDT_TERRAININFO ;
            mxentity::symbol = symbol;
            mxentity::id = id ;
            
            mxentity::Flags().Set(tif_interesting);
        
        }
    
        mxterrain::~mxterrain()
        {
        }

        void mxterrain::Serialize ( archive& ar )
        {
            mxinfo::Serialize ( ar );

            if ( ar.IsStoring() ) {
                ar << preposition ;
                ar << description ;
                ar << success ;
                ar << visibility;
                ar << obstruction;
                ar << movementcost;
            }else{
                ar >> preposition ;
                ar >> description ;
                ar >> success ;
                ar >> visibility;
                ar >> obstruction;
                ar >> movementcost;
            }
        }


        MXRESULT mxterrain::FillExportData ( info_t* data )
        {
        defaultexport::terraininfo_t* out = (defaultexport::terraininfo_t*)data;

            VALIDATE_INFO_BLOCK(out,INFO_TERRAININFO,defaultexport::terraininfo_t);

            out->type = (mxterrain_t)Id();
            out->preposition = std::string(Preposition());
            out->description = std::string(Description());
            out->success = Success();
            out->visibility = Visibility();
            out->obstruction = Obstruction();
            out->movementcost = MovementCost();

            return mxinfo::FillExportData ( data );
        }


}
// namespace tme
