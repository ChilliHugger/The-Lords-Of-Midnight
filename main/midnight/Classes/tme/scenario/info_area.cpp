/*
 * FILE:    info_area.cpp
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

	mxarea::mxarea()
	{
		mxentity::type = IDT_AREAINFO ;
	}

	mxarea::~mxarea()
	{
	}

	void mxarea::Serialize ( archive& ar )
	{
		mxinfo::Serialize ( ar );
		if ( ar.IsStoring() ) {
			ar << prefix ;
		}else{
			ar >> prefix ;
		}
	}

	MXRESULT mxarea::FillExportData ( info_t* data )
	{
		defaultexport::areainfo_t* out = (defaultexport::areainfo_t*)data;
		VALIDATE_INFO_BLOCK(out,INFO_AREAINFO,defaultexport::areainfo_t);
		out->prefix = prefix ;
		return mxinfo::FillExportData ( data );
	}


}
// namespace tme
