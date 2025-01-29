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
    
    lom_stronghold::lom_stronghold()
    {
        mxentity::idType = IDT_STRONGHOLD ;
        race=RA_NONE;
        totaltroops=0;
        owner=NULL;

    }

    lom_stronghold::~lom_stronghold()
    {

    }

    u32 lom_stronghold::Add ( mxrace_t race, mxunit_t type, u32 amount )
    {
        if ( OccupyingRace() != race || Type() != type )
            return 0;

        return mxstronghold::Add(race, type, amount);
    }

    u32 lom_stronghold::Remove ( mxrace_t race, mxunit_t type, u32 amount )
    {
        if ( OccupyingRace() != race || Type() != type )
            return 0;

        return mxstronghold::Remove(race, type, amount);
    }
 
} // namespace tme
