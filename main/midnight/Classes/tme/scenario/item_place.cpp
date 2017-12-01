/*
 * FILE:    item_place.cpp
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

		mxplace::mxplace()
		{
			mxentity::type = IDT_PLACE ;
		}

		mxplace::~mxplace()
		{
		}

		void mxplace::Serialize ( archive& ar )
		{
			mxitem::Serialize(ar);
			//if ( ar.IsStoring() ) {
			//}else{
			//}
		}

		archive& operator<<(archive& ar, mxplace* node)
		{
			return ar << ((u32)node->SafeId());
		}

		archive& operator>>( archive& ar, mxplace*& node)
		{
		int temp;
			ar >> temp ; node = (mxplace*)mx->PlaceById(temp);
			return ar ;
		}

		MXRESULT mxplace::FillExportData ( info_t* data )
		{
			defaultexport::place_t* out = (defaultexport::place_t*)data;

			VALIDATE_INFO_BLOCK(out,INFO_PLACE,defaultexport::place_t);


			return mxitem::FillExportData ( data );
		}



}
// namespace tme
