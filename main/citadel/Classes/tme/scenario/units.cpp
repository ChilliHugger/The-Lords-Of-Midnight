/*
 * FILE:    units.cpp
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

	//namespace units {
		mxunit::mxunit()
		{
			total=NONE;
			energy=NONE;
			lost=NONE;
			slew=NONE;
		//	race=RA_NONE ;
		}

		mxunit::~mxunit()
		{
		}

		const string& mxunit::Name() const
		{ 
			return mx->UnitById(Type())->Name() ;
		}

		/*
		* Function name	: base::Serialize
		* 
		* Return type		: archive&
		* 
		* Arguments		: archive& ar
		* 
		* Description	    : 
		* 
		*/

		archive& mxunit::Serialize ( archive& ar )
		{
			if ( ar.IsStoring() ) {
				ar << total;
				ar << energy;
				ar << lost;
				ar << slew;
			}else{
				ar >> total;
				ar >> energy;
				ar >> lost;
				ar >> slew;
                
                if ( (s32) energy < 0 )
                    energy=0;
                
			}

			return ar ;
		}


		/*
		* Function name	: base::BattleSuccess
		* 
		* Return type		: int
		* 
		* Arguments		: const mxlocinfo& locinfo
		* 
		* Description	    : 
		* 
		*/

		s32 mxunit::BattleSuccess ( const mxlocinfo& locinfo, const mxcharacter* parent )
		{
			return 0;
		}



		/*
		* Function name	: base::BaseRestModifier
		* 
		* Return type		: int
		* 
		* Arguments		: 
		* 
		* Description	    : 
		* 
		*/

		s32 mxunit::BaseRestModifier() const
		{
			return mx->UnitById(type)->BaseRestModifier();
		}


		mxwarriors::mxwarriors()
		{
			type = UT_WARRIORS ;
		}

		mxwarriors::~mxwarriors()
		{
		}


		/*
		* Function name	: warriors::BattleSuccess
		* 
		* Return type		: int
		* 
		* Arguments		: const mxlocinfo& locinfo
		* 
		* Description	    : 
		* 
		*/

		s32 mxwarriors::BattleSuccess ( const mxlocinfo& locinfo, const mxcharacter* parent )
		{
		s32 success ;
			
		// TODO this should be race dependent
			success =  locinfo.friends.adjustment + energy ;
			success = (success/2) + (s32)sv_success_base_level ;

			return success ;
		}


		mxriders::mxriders()
		{
			type = UT_RIDERS ;
		}

		mxriders::~mxriders()
		{
		}


		/*
		* Function name	: riders::BattleSuccess
		* 
		* Return type		: int
		* 
		* Arguments		: const mxlocinfo& locinfo
		* 
		* Description	    : 
		* 
		*/

		s32 mxriders::BattleSuccess ( const mxlocinfo& locinfo, const mxcharacter* parent )
		{
		s32 success ;

		// TODO this should be race dependent

		// TODO Should be table
			s32 energyadj = (locinfo.mapsqr.terrain == TN_MOUNTAIN) 
				? (s32)sv_success_riders_energy_mountain : (s32)sv_success_riders_energy_other ;
			
			success = locinfo.friends.adjustment + energyadj ;

		// TODO Should be table
			if ( locinfo.mapsqr.terrain == TN_FOREST && parent->Race() == RA_FEY && parent->IsRiding() )
				success += (s32)sv_success_fey_on_horse_adjuster;

			success = (success/2) + (s32)sv_success_base_level ;

			return success ;
		}

	//}
	// namespace units
}
// namespace tme
