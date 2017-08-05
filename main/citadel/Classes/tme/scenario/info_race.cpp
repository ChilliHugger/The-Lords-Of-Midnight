/*
 * FILE:    info_race.cpp
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


		mxrace::mxrace()
		{
			mxentity::type = IDT_RACEINFO ;
		}

		mxrace::~mxrace()
		{
		}

		void mxrace::Serialize ( archive& ar )
		{
			mxinfo::Serialize ( ar );

			if ( ar.IsStoring() ) {
				ar << soldiersname;
				ar << success ;
				ar << initialmovement;
				ar << diagonalmodifier;
				ar << ridingmultiplier;
				ar << movementmax;
				ar << baserestamount;
				ar << strongholdstartups;

				ar << misttimeaffect ;
				ar << mistdespondecyaffect ;
				ar << baseenergycost ;
				ar << baseenergycosthorse ;

			}else{
				ar >> soldiersname;
				ar >> success ;
				ar >> initialmovement;
				ar >> diagonalmodifier;
				ar >> ridingmultiplier;
				ar >> movementmax;
				ar >> baserestamount;
				ar >> strongholdstartups;

				ar >> misttimeaffect ;
				ar >> mistdespondecyaffect ;
				ar >> baseenergycost ;
				ar >> baseenergycosthorse ;

#if defined(_DDR_)
                if ( IsSymbol("RA_MORKIN") ) {
                    baseenergycost=2;
                    baseenergycosthorse=4;
                }
                
                if ( mx->isDatabase() ) {
                    int temp = baseenergycost;
                    baseenergycost=baseenergycosthorse;
                    baseenergycosthorse=temp;
                }
#endif
                
			}
		}

		// TODO require race/terrain table here
		u32 mxrace::TerrainMovementModifier( mxterrain_t terrain ) const
		{
		// SHOULD BE DONE THROUGH THE DATABASE
			switch(terrain) {
				case TN_DOWNS:
					if ( id == RA_DOOMGUARD ) return 0;
					return 1;
				case TN_MOUNTAIN:
					return 4;
				case TN_FOREST:
					if ( id == RA_DOOMGUARD ) return 4;
					if ( id != RA_FEY ) return 3;
					break;
				default:
					break;
			}
			return 0 ;
		}

#if defined(_DDR_)
//    L7354  DB TERRAIN_CITY,   TERRAIN_CITY			;moonprince
//    DB TERRAIN_PLAINS, TERRAIN_PLAINS		;free
//    DB TERRAIN_TOWER,  TERRAIN_TOWER			;wise
//    DB TERRAIN_FOREST, TERRAIN_FOREST		;fey
//    DB TERRAIN_HILLS,  TERRAIN_HILLS			;barbarian
//    DB TERRAIN_PLAINS, TERRAIN_PLAINS		;icelord
//    DB TERRAIN_FOREST						;fey
//    DB TERRAIN_MOUNTAINS						;giant
//    DB TERRAIN_PLAINS						;heartstealer
//    DB TERRAIN_FROZENWASTE					;dwarf
    
//    L7354A DB TERRAIN_MOUNTAINS, TERRAIN_MOUNTAINS  ;moonprince
//    DB TERRAIN_PLAINS,    TERRAIN_PLAINS     ;free
//    DB TERRAIN_FOREST,    TERRAIN_FOREST     ;wise
//    DB TERRAIN_FOREST,    TERRAIN_FOREST     ;fey
//    DB TERRAIN_FOREST,    TERRAIN_FOREST     ;barbarian
//    DB TERRAIN_PLAINS,    TERRAIN_PLAINS     ;icelord
//    DB TERRAIN_FOREST						;fey
//    DB TERRAIN_MOUNTAINS						;giant
//    DB TERRAIN_PLAINS						;heartstealer
//    DB TERRAIN_FROZENWASTE					;dwarf
#endif

		string& mxrace::SoldiersName()
		{
			if ( ((LPSTR)soldiersname!=NULL) && chilli::lib::strlen(soldiersname) )
				return soldiersname; 
			return Name();

		}

		MXRESULT mxrace::FillExportData ( info_t* data )
		{
			defaultexport::raceinfo_t* out = (defaultexport::raceinfo_t*)data;

			VALIDATE_INFO_BLOCK(out,INFO_RACEINFO,defaultexport::raceinfo_t);

			out->type = (mxrace_t)Id();
			out->soldiersname = SoldiersName();;
			out->success = Success();
			out->initialmovement = InitialMovementValue();
			out->diagonalmodifier = DiagonalMovementModifier();
			out->ridingmultiplier = (s32)RidingMovementMultiplier();
			out->movementmax = MovementMax();
			out->baserestamount= BaseRestAmount();
			out->strongholdstartups = StrongholdStartups();

			out->baseenergycost = BaseEnergyCost() ;
			out->baseenergycosthorse = BaseEnergyCostHorse() ;
			out->misttimeaffect = MistTimeAffect() ;
			out->mistdespondecyaffect = MistDespondecyAffect() ;

			return mxinfo::FillExportData ( data );
		}

	
}
// namespace tme
