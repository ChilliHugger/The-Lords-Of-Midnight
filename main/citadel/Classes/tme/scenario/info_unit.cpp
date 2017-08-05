/*
 * FILE:    info_unit.cpp
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

		mxunitinfo::mxunitinfo()
		{
			mxentity::type = IDT_UNITINFO ;
		}

		mxunitinfo::~mxunitinfo()
		{
		}

		void mxunitinfo::Serialize ( archive& ar )
		{
			mxinfo::Serialize ( ar );

			if ( ar.IsStoring() ) {
				ar << success ;
				ar << baserestmodifier;
			}else{
				ar >> success ;
				ar >> baserestmodifier;
			}
		}

		MXRESULT mxunitinfo::FillExportData ( info_t* data )
		{
		defaultexport::unitinfo_t* out = (defaultexport::unitinfo_t*)data;

			VALIDATE_INFO_BLOCK(out,INFO_UNITINFO,defaultexport::unitinfo_t);

			out->type = (mxunit_t)Id();
			out->success = Success();
			out->baserestmodifier = BaseRestModifier();

			return mxinfo::FillExportData ( data );
		}


}
// namespace tme
