/*
 * FILE:    info_direction.cpp
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


	mxdirection::mxdirection()
	{
		mxentity::type = IDT_DIRECTIONINFO ;
	}

	mxdirection::~mxdirection()
	{
	}

	void mxdirection::Serialize ( archive& ar )
	{
		mxinfo::Serialize ( ar );
		if ( ar.IsStoring() ) {
		}else{
		}
	}

	MXRESULT mxdirection::FillExportData ( info_t* data )
	{
		defaultexport::directioninfo_t* out = (defaultexport::directioninfo_t*)data;
		VALIDATE_INFO_BLOCK(out,INFO_DIRECTIONINFO,defaultexport::directioninfo_t);
		out->type = (mxdir_t)Id() ;
		return mxinfo::FillExportData ( data );
	}

}
// namespace tme
