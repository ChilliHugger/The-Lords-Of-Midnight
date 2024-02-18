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
            mxentity::idType = IDT_RACEINFO ;
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

        std::string& mxrace::SoldiersName()
        {
            if ( !soldiersname.empty() )
                return soldiersname; 
            return Name();

        }

        MXRESULT mxrace::FillExportData ( info_t* data )
        {
            defaultexport::raceinfo_t* out = (defaultexport::raceinfo_t*)data;

            VALIDATE_INFO_BLOCK(out,INFO_RACEINFO,defaultexport::raceinfo_t);

            out->type = (mxrace_t)Id();
            out->soldiersname = std::string(SoldiersName());
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
