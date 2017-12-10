/*
 * FILE:    mxentity.cpp
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

	mxentity::mxentity(void)
	{
		type = IDT_NONE ;
		id = 0 ;
	}

	mxentity::~mxentity(void)
	{
	}

	MXRESULT mxentity::FillExportData ( info_t* data )
	{

		return MX_FAILED ;
	}

	u32 mxentity::SafeId() const
	{
		if ( this==NULL )
			return 0;
		return Id();
	}

	mxid mxentity::SafeIdt() const
	{
		if ( this==NULL )
			return IDT_NONE;
		return MAKE_ID(type,Id());
	}

	void mxentity::Serialize ( archive& ar )
	{
		if ( ar.IsStoring() ) {
			ar << symbol;
			ar << flags ;
		}else{
			ar >> symbol;
			ar >> flags ;
		}
	}

	archive& operator<<(archive& ar, mxentity* item)
	{
		return ar << ((u32)item->SafeIdt());
	}

	archive& operator>>( archive& ar, mxentity*& item)
	{
	u32 temp;
	ar >> temp ; 
		item = (mxentity*)mx->EntityByIdt(temp);
		//if ( item == NULL ) {
		//	item = (entity*)mx->entityByIdt(temp);
		//}
		return ar ;
	}

	int mxentity::Compare ( mxentity* o, int hint ) const
	{
		return chilli::lib::Compare((s32)Id(),(s32)o->Id());
	}

}
// namespace tme
