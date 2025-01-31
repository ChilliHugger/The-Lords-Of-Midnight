//
//  savegamemapping.h
//  revenge
//
//  Created by Chris Wild on 01/09/2023.
//

#ifndef savegamemapping_h
#define savegamemapping_h

#include "../baseinc/tme_internal.h"
#if defined(_DDR_)
#include "../scenarios/ddr/scenario_ddr_internal.h"
#endif

namespace tme {
namespace utils {

#if defined(_DDR_)
    enum OLD_DDR_CHARACTERFLAGS {
        cf_resting          = MXBIT(14),    // currently resting
        cf_inbattle         = MXBIT(15),    // currently in battle
        cf_wonbattle        = MXBIT(16),    // just won a battle
        cf_tunnel           = MXBIT(17),    // currently in a tunnel (DDR)
        cf_usedobject       = MXBIT(18),    // has used his special object (DDR)
        cf_followers        = MXBIT(19),    // has followers
        cf_preparesbattle   = MXBIT(20),    // prepares to do battle
        cf_approaching      = MXBIT(21),    // we are approaching a lord (DDR)
        
        cf_all              =   cf_resting | cf_inbattle | cf_wonbattle | cf_tunnel |
                                cf_usedobject | cf_followers | cf_preparesbattle | cf_approaching
    };

    flags32 UpdateDDRCharacterFlags(flags32 flags);
    void FixMorkinFromBeingAIAfterRecruited();
    mxkilledby_t CalcKilledBy(const ddr_character* character);
    
    class UpdateDDRObjects
    {
    public:
        static void FixInvalidTypes();
        static void FixRecruitmentFlags();
    };
#endif
    
    void UpdateStrongholdsOnMap();

}
}

#endif /* savegamemapping_h */
