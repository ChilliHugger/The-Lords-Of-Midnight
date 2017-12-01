/*
 * FILE:    mxinfo.cpp
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

	//namespace info {

		mxinfo::mxinfo()
		{
			user_data=NULL;
		}

		mxinfo::~mxinfo()
		{
		}

		void mxinfo::Serialize ( archive& ar )
		{
			mxentity::Serialize(ar);
			// ID will always be done elsewhere
			if ( ar.IsStoring() ) {
				//ar << id;
				ar << name;
			}else{
				//ar >> id;
				ar >> name;
			}
		}

		MXRESULT mxinfo::FillExportData ( info_t* data )
		{
		mxinfo_t* out = (mxinfo_t*)data;

			//VALIDATE_INFO_BLOCK(out,INFO_AREAINFO,areainfo_t);

			out->id = SafeIdt();
			out->flags = flags ;
			out->name = name ;
			out->userdata = (void*)user_data ;

			return MX_OK ;
		}


	//}
	// namespace info
}
// namespace tme
