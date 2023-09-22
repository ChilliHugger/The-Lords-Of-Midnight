//
//  savegamemapping.cpp
//  tme
//
//  Created by Chris Wild on 01/09/2023.
//

#include "savegamemapping.h"

namespace tme {
namespace utils {


    void UpdateObjectThing(mxobject* object)
    {
        if(object->Id() <= TH_SPRINGS) {
            object->Flags().Set(of_thing);
            object->thing = (mxthing_t)object->Id();
        }
    }

    void UpdateStrongholdsOnMap()
    {
         for (int ii = 0; ii < sv_strongholds; ii++) {
            auto stronghold = static_cast<mxstronghold*>(mx->StrongholdById(ii+1));
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
        auto morkin = static_cast<mxcharacter*>(mx->EntityByName("CH_MORKIN", IDT_CHARACTER));
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
#endif

}
}
