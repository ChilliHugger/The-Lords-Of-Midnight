//
//  savegamemapping.cpp
//  tme
//
//  Created by Chris Wild on 01/09/2023.
//

#include "savegamemapping.h"

namespace tme {
namespace utils {

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

}
}
