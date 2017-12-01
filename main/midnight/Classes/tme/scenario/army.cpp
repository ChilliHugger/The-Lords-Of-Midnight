/*
 * FILE:    army.cpp
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
    
	mxarmy::mxarmy()
	{
		parent=NULL;
		armytype=AT_NONE;
		race=RA_NONE;
		type=UT_NONE;
		total=NONE;
		success=NONE;
		killed=NONE;
	}

	mxarmy::~mxarmy()
	{
	}

}
// namespace tme

