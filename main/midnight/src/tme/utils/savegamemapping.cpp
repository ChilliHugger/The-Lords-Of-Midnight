//
//  savegamemapping.cpp
//  tme
//
//  Created by Chris Wild on 01/09/2023.
//

#include "savegamemapping.h"

#if defined(_DDR_)
#include "../scenarios/ddr/scenario_ddr_internal.h"
#endif

namespace tme {
namespace utils {


    void UpdateStrongholdsOnMap()
    {
        FOR_EACH_STRONGHOLD(stronghold) {
            mxloc& mapsqr = mx->gamemap->GetAt ( stronghold->Location() );
            if ( mapsqr.IsVisible() )
                mapsqr.flags |= lf_visited|lf_looked_at ;
        }
    }

#if defined(_DDR_)

    //
    // Old DDR flags in this section were 2 bits shifted down than LOM
    //

    flags32 UpdateDDRCharacterFlags(flags32 flags)
    {
        auto old_mask = OLD_DDR_CHARACTERFLAGS::cf_all ; // 0x003fc000
                        
        auto new_mask = cf_unused1 | cf_unused2 |
                        cf_resting | cf_inbattle | cf_wonbattle | cf_tunnel |
                        cf_usedobject | cf_followers | cf_preparesbattle | cf_approaching;
                        
        auto old_flags = (flags & old_mask) << 2;
        flags.Reset(new_mask);
        flags.Set(old_flags);
        return flags;
    }

    void FixMorkinFromBeingAIAfterRecruited()
    {
        auto morkin = mx->CharacterBySymbol("CH_MORKIN");
        morkin->race = RA_MORKIN;

        if ( !morkin->IsRecruited() )
            morkin->Flags().Set(cf_ai);

        // fix for morkin being AI character
        // after being recruited
        if ( morkin->IsRecruited() && morkin->IsAIControlled() ) {
            morkin->Flags().Reset(cf_ai);
            if ( morkin->IsInTunnel() )
                morkin->looking=DR_NORTH;
        }
    }
    
    void UpdateDDRObjects::FixInvalidTypes()
    {
        // 118 | OB_SWORD_THORTHAK > should be a SPEAR
        auto item1 = static_cast<ddr_object*>(mx->EntityByName("OB_SWORD_THORTHAK", IDT_OBJECT));
        item1->type = OT_SPEAR;
        item1->symbol = "OB_SPEAR_THORTHAK";
        
        // 124 | OB_HAMMER_TORORTHANE > should be a BOW 
        auto item2 = static_cast<ddr_object*>(mx->EntityByName("OB_HAMMER_TORORTHANE", IDT_OBJECT));
        item2->type = OT_BOW;
        item2->symbol = "OB_BOW_TORORTHANE";
    }
    
#endif

}
}
