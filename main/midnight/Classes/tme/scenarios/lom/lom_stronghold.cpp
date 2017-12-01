/*
 * FILE:    lom_stronghold.cpp
 * 
 * PROJECT: lom
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

#include "../../baseinc/tme_internal.h"
#include "scenario_lom_internal.h"

using namespace tme::scenarios ;

namespace tme {
	
//namespace lom {

	
	lom_stronghold::lom_stronghold()
	{
		mxentity::type = IDT_STRONGHOLD ;
		race=RA_NONE;
		total=NONE;
		owner=NULL;

	}

	lom_stronghold::~lom_stronghold()
	{

	}



	/*
	 * Function name	: lom_tronghold::Remove
	 * 
	 * Return type		: int
	 * 
	 * Arguments		: mxrace_t race
	 *                  : mxunit_t type
	 *                  : int amount
	 * 
	 * Description	    : 
	 * 
	 */

	u32 lom_stronghold::Remove ( mxrace_t race, mxunit_t type, u32 amount )
	{
		if ( OccupyingRace() != race || Type() != type )
			return 0;

	// TODO this is a LOM scenario override
	//	if ( (Total()-amount) < Min() )
	//		amount = Total()-Min();
	//
		total-= amount;
		return amount;
	}

//} // namespace lom_x
	
} // namespace tme
