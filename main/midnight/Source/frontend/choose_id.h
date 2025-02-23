//
//  choose_id.h
//  midnight
//
//  Created by Chris Wild on 15/10/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#ifndef choose_id_h
#define choose_id_h

enum CHOOSE_INDEX {
    CHOOSE_THINK    = 0,
    
#if defined(_LOM_)
    CHOOSE_SEEK     = 1,
    CHOOSE_HIDE     = 2,
    CHOOSE_FIGHT    = 3,
#endif
    
#if defined(_DDR_)
    CHOOSE_REST     = 1,
    CHOOSE_GIVE     = 2,
    CHOOSE_TAKE     = 3,
    CHOOSE_USE      = 4,
    CHOOSE_FIGHT    = 12,
#endif

    CHOOSE_APPROACH = 9,
    CHOOSE_BATTLE   = 5,
    CHOOSE_RECRUIT  = 6,
    CHOOSE_GUARD    = 7,

    CHOOSE_TUNNEL   = 8,

    CHOOSE_MAP      = 13,
    CHOOSE_NIGHT    = 14,
    
    CHOOSE_UNDO_LAST= 10,
    CHOOSE_UNDO_DAWN= 11,
    
    CHOOSE_CHAR     = 15,
    CHOOSE_EXIT     = 19,
};


#endif /* choose_id_h */
