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


namespace tme {

		mxterrain::mxterrain()
		{
			mxentity::type = IDT_TERRAININFO ;
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
			out->preposition = Preposition();
			out->description = Description();
			out->success = Success();
			out->visibility = Visibility();
			out->obstruction = Obstruction();
			out->movementcost = MovementCost();

			return mxinfo::FillExportData ( data );
		}


}
// namespace tme
