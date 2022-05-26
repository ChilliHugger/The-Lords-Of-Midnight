//
//  ddr_stronghold.cpp
//  ddr
//
//  Created by Chris Wild on 11/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "../../baseinc/tme_internal.h"


#if defined(_DDR_)

#include "scenario_ddr.h"
#include "scenario_ddr_internal.h"

using namespace tme::scenarios ;

namespace tme {
   
    ddr_stronghold::ddr_stronghold() :
        energy(0)
    {
    }
    
    ddr_stronghold::~ddr_stronghold()
    {
    }
    
    void ddr_stronghold::MakeChangeSides( mxrace_t newrace, mxcharacter* newoccupier )
    {
        // if the new occupier is allowed an army then
        // the stronghold army will be his race
        if ( newoccupier && newoccupier->IsAllowedArmy() )
            newrace = newoccupier->Race();
        
        if ( newrace != OccupyingRace() )    {
            occupyingrace = newrace ;
            
            totaltroops = 0 ;//Respawn() ; // respawn will happen at night
            owner = newoccupier ;
            
            occupier = newoccupier ;
            
            type = static_cast<ddr_character*>(newoccupier)->getArmyType();
        }
        
    }
    
    void ddr_stronghold::Serialize ( archive& ar )
    {
        mxstronghold::Serialize(ar);
        if ( ar.IsStoring() ) {
            ar << energy;
            
        }else{
            if ( tme::mx->SaveGameVersion() > 9 )
                ar >> energy ;
            else
                energy = 180 ;
        }
    }

    void ddr_stronghold::OnRespawn ( void )
    {
        Add(Race(), Type(), respawn);
    }
    
    mxrace_t ddr_stronghold::Loyalty ( void )
    {
        if ( Occupier() == nullptr )
            return mxstronghold::Loyalty();
        return Occupier()->NormalisedLoyalty();
    }
    
    MXRESULT ddr_stronghold::FillExportData ( info_t* data )
    {
        using export_t = tme::scenarios::ddr::exports::stronghold_t;
        
        if ( mxstronghold::FillExportData(data) == MX_OK ) {
            if ( (u32)ID_TYPE(data->id) != (u32)INFO_ARMY ) {
                
                VALIDATE_INFO_BLOCK(data,INFO_STRONGHOLD,export_t);

                auto out = static_cast<export_t*>(data);
                out->energy = energy;
            }
            return MX_OK;
        }
        return MX_FAILED;
    }
    
    
}

#endif // _DDR_
