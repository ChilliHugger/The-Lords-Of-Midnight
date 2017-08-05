/*
 * FILE:    mxitem.cpp
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
	//namespace item {

		mxitem::mxitem()
		{
			//ItemType( LI_NONE );
			flags.Reset(0xffffffff);
			user_data=NULL;
			//type=NONE;
		}

		mxitem::~mxitem()
		{
		}

		mxitem::mxitem( mxgridref loc )
		{
			//ItemType( LI_NONE );
			location = loc ;
			flags.Reset(0xffffffff);
		}


		void mxitem::Serialize ( archive& ar )
		{
			mxentity::Serialize(ar);
			if ( ar.IsStoring() ) {
				ar << location;
			}else{
				ar >> location;
			}
		}

		MXRESULT mxitem::FillExportData ( info_t* data )
		{
		mxitem_t* out = (mxitem_t*)data;

			//VALIDATE_INFO_BLOCK(out,INFO_AREAINFO,tme::areainfo_t);

			out->id = SafeIdt();
			out->flags = flags ;
			out->location.x = location.x ;
			out->location.y = location.y ;
			out->userdata = (void*)user_data ;

			return MX_OK ;
		}

		archive& operator<<(archive& ar, mxitem* item)
		{
			return ar << ((u32)item->SafeIdt());
		}

		archive& operator>>( archive& ar, mxitem*& item)
		{
		u32 temp;
		ar >> temp ; 
			item = (mxitem*)mx->EntityByIdt(temp);
			//if ( item == NULL ) {
			//	item = (item::base*)mx->EntityByIdt(temp);
			//}
			return ar ;
		}
	//}
	// namespace item
}
// namespace tme

