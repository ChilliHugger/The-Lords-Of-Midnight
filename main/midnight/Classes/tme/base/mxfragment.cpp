/*
 * FILE:    mxfragment.cpp
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

	mxfragment::mxfragment()
	{
		m_data = NULL;
		m_Next = NULL;
		m_Prev = NULL ;
		m_day=0;
	}

	mxfragment::~mxfragment()
	{
	}

	mxfragment::mxfragment( mxitem* item )
	{
		m_data = item;
		m_day = sv_days;
		m_location=item->Location();
		m_Next = NULL;
		m_Prev = NULL ;
	}


	void mxfragment::Serialize ( archive& ar )
	{

		if ( ar.IsStoring() ) {
		
			ar << m_data ;
			ar << m_day ;
			ar << m_location;
			ar << m_race ;
		
		}else{

			ar >> m_data ;
			ar >> m_day ;
			ar >> m_location;
			READ_ENUM ( m_race );

		}
	}

	u32 mxfragment::Age() const
	{
		return sv_days-m_day;
	}

}
// namespace tme
