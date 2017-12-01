/*
 * FILE:    info_command.cpp
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


	mxcommand::mxcommand()
	{
		mxentity::type = IDT_COMMANDINFO ;
	}

	mxcommand::~mxcommand()
	{
	}

	void mxcommand::Serialize ( archive& ar )
	{
		mxinfo::Serialize ( ar );

		if ( ar.IsStoring() ) {
			ar << successtime ;
			ar << failuretime;
		}else{
			ar >> successtime ;
			ar >> failuretime;
		}
	}

	MXRESULT mxcommand::FillExportData ( info_t* data )
	{
	defaultexport::commandinfo_t* out = (defaultexport::commandinfo_t*)data;

		VALIDATE_INFO_BLOCK(out,INFO_COMMANDINFO,defaultexport::commandinfo_t);

		out->type = (command_t)Id();
		out->successtime = successtime;
		out->failuretime = failuretime;

		return mxinfo::FillExportData ( data );
	}


}
// namespace tme
