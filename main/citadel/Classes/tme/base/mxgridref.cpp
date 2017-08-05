/*
 * FILE:    mxgridref.cpp
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

	// Table to work out directions
	static s32 DirectionLookTable[] = {
		0,-1,		// DR_NORTH,
		1,-1,		// DR_NORTHEAST,
		1,0,		// DR_EAST,
		1,1,		// DR_SOUTHEAST,
		0,1,		// DR_SOUTH,
		-1,1,		// DR_SOUTHWEST,
		-1,0,		// DR_WEST,
		-1,-1		// DR_NORTHWEST,
	};

	mxgridref::mxgridref()
	{
		x=y=0;
	}

	mxgridref::mxgridref( u32 x, u32 y )
	{
		this->x = x;
		this->y = y;
	}

	mxgridref::mxgridref( loc_t loc )
	{
		this->x = loc.x;
		this->y = loc.y;
	}

	mxgridref::~mxgridref()
	{
	}
	
	//
	// METHOD:	DirFromHere
	//			Returns the direction enum of the location being looked at
	// 
	// PARAMS:	loc			location looking at
	// 
	// REMARKS:	
	//
	// RETURNS:	mxdir_t
	//
	mxdir_t mxgridref::DirFromHere ( mxgridref loc ) const
	{
		int dx = loc.x-x;
		if ( dx ) dx = SIGN(dx);
		int dy = loc.y-y;
		if ( dy ) dy = SIGN(dy);
		int dir = 0;

		for ( u32 ii=0; ii<NUMELE(DirectionLookTable);ii+=2,dir++) {
			if ( (DirectionLookTable[ii] == dx) && (DirectionLookTable[ii+1] == dy) )
				return (mxdir_t) dir ;
		}
		return DR_NORTH ;
	}

	// METHOD:	AddDirection
	//			Adds one grid reference in the direction given
	// 
	// PARAMS:	dir			direction
	// 
	// REMARKS:	
	//
	// RETURNS:	none
	//
	void mxgridref::AddDirection( mxdir_t dir )
	{
	int offset;
		offset = dir * 2;
		y += DirectionLookTable[offset+1];
		x += DirectionLookTable[offset];
	}

	//
	// METHOD:	operator +
	//			Adds one grid reference in the direction given
	//			and returns a new reference
	// 
	// PARAMS:	dir			direction
	// 
	// REMARKS:	
	//
	// RETURNS:	mxgridref - new grid reference
	//
	mxgridref mxgridref::operator + ( mxdir_t dir )
	{ 
	mxgridref loc;
		loc = *this;
		loc += dir ;
		return loc ;
	}

	
	archive& mxgridref::Serialize ( archive& ar )
	{ 
		if ( ar.IsStoring() ) {
			ar << x ; 
			ar << y ; 
		}else{
			ar >> x ; 
			ar >> y ; 
		}
		return ar; 
	}


	//
	// METHOD:	operator - 
	//			returns a value which is used as the distance between two references
	// 
	// PARAMS:	loc			grid reference to calc distance from
	// 
	// REMARKS:	it is not mathematically correct ie: Pythagoras
	//
	// RETURNS:	s32 - distance
	//
	s32 mxgridref::operator- ( mxgridref loc )
	{
	s32 high, low;
		high = ABS((s32)x - (s32)loc.x);
		low = ABS((s32)y - (s32)loc.y);
		high += low;
		return high;
	}

	//
	// METHOD:	operator ==
	//			checks is this grid reference the same as another
	// 
	// PARAMS:	loc			grid reference to compare against
	// 
	// REMARKS:	
	//
	// RETURNS:	true/false
	//
	BOOL mxgridref::operator == ( mxgridref& loc )
	{
		return (loc.x==x) && (loc.y==y) ;
	}

	//
	// METHOD:	operator ==
	//			checks is this grid reference different from another
	// 
	// PARAMS:	loc			grid reference to compare against
	// 
	// REMARKS:	
	//
	// RETURNS:	true/false
	//
	BOOL mxgridref::operator != ( mxgridref& loc )
	{
		return (loc.x!=x) || (loc.y!=y) ;
	}

}
// namespace tme


