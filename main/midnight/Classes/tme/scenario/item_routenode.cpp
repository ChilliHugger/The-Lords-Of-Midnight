/*
 * FILE:    item_routenode.cpp
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

		mxroutenode::mxroutenode()
		{
			mxentity::type = IDT_ROUTENODE ;
			CLEARARRAY ( paths );
		}

		mxroutenode::~mxroutenode()
		{
		}

		void mxroutenode::Serialize ( archive& ar )
		{
			mxitem::Serialize(ar);
			if ( ar.IsStoring() ) {
				ar << paths[0] ;
				ar << paths[1] ;
			}else{
				ar >> paths[0] ;
				ar >> paths[1] ;
			}
		}

		archive& operator<<(archive& ar, mxroutenode* node)
		{
			return ar << ((u32)node->SafeId());
		}

		archive& operator>>( archive& ar, mxroutenode*& node)
		{
		int temp;
			ar >> temp ; node = (mxroutenode*)mx->RouteNodeById(temp);
			return ar ;
		}

		MXRESULT mxroutenode::FillExportData ( info_t* data )
		{
			defaultexport::routenode_t* out = (defaultexport::routenode_t*)data;

			VALIDATE_INFO_BLOCK(out,INFO_ROUTENODE,defaultexport::routenode_t);
			
			for ( u32 ii=0; ii<NUMELE(paths); ii++ )
				out->paths[ii] = paths[ii]->SafeIdt();

			return mxitem::FillExportData ( data );
		}


}
// namespace tme
