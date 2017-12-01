/*
 * FILE:    info_gender.cpp
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


		mxgender::mxgender()
		{
			mxentity::type = IDT_GENDERINFO ;
		}

		mxgender::~mxgender()
		{
		}

		void mxgender::Serialize ( archive& ar )
		{
			mxinfo::Serialize ( ar );
			if ( ar.IsStoring() ) {
				ar << pronoun1 ;
				ar << pronoun2 ;
				ar << pronoun3 ;
			}else{
				ar >> pronoun1 ;
				ar >> pronoun2 ;
				ar >> pronoun3 ;
			}
		}

		MXRESULT mxgender::FillExportData ( info_t* data )
		{
			defaultexport::genderinfo_t* out = (defaultexport::genderinfo_t*)data;
			VALIDATE_INFO_BLOCK(out,INFO_GENDERINFO,defaultexport::genderinfo_t);
			out->pronoun[0] = pronoun1 ;
			out->pronoun[1] = pronoun2 ;
			out->pronoun[2] = pronoun3 ;
			return mxinfo::FillExportData ( data );
		}

	
}
// namespace tme
