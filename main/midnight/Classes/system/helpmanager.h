//
//  help.h
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//

#ifndef help_h
#define help_h

#include "../library/libinc/mxtypes.h"
#include "storymanager.h"

FORWARD_REFERENCE(moonring);

enum HELPFLAGS
{
    hf_none         = 0,
    hf_important    = MXBIT(0),
    hf_always       = MXBIT(1),
};

typedef struct helpitem_t
{
    int     flags;
    const char*   text;
    
} helpitem_t ;


enum helpid_t {
    HELP_NONE=0,
    
#if defined(_LOM_)
    // lom
    HELP_TN_PLAINS,
    HELP_TN_CITADEL,
    HELP_TN_FOREST,
    HELP_TN_HENGE,
    HELP_TN_TOWER,
    HELP_TN_VILLAGE,
    HELP_TN_DOWNS,
    HELP_TN_KEEP,
    HELP_TN_SNOWHALL,
    HELP_TN_LAKE,
    HELP_TN_FROZENWASTE,
    HELP_TN_RUIN,
    HELP_TN_LITH,
    HELP_TN_CAVERN,
    HELP_TN_MOUNTAIN,
    HELP_TN_UNUSED1,
#endif
    
#if defined(_DDR_)
    // ddr
    HELP_TN_PLAINS2,
    HELP_TN_MOUNTAIN2,
    HELP_TN_FOREST2,
    HELP_TN_HILLS,
    HELP_TN_GATE,
    HELP_TN_TEMPLE,
    HELP_TN_PIT,
    HELP_TN_PALACE,
    HELP_TN_FORTRESS,
    HELP_TN_HALL,
    HELP_TN_HUT,
    HELP_TN_WATCHTOWER,
    HELP_TN_CITY,
    HELP_TN_FOUNTAIN,
    HELP_TN_STONES,
    HELP_TN_ICYWASTES,
    
    HELP_TN_MISTS,
    HELP_TN_TUNNEL,
#endif
    
    HELP_SELECTING_CHARACTER,
    HELP_LOOKING_AROUND,
    HELP_MOVEMENT,
    HELP_THINKING,
    
    HELP_OVERVIEW_MAP,
    HELP_DISCOVERY_MAP,
    HELP_CHOOSE,
    
    HELP_NASTIES,
    HELP_ARMIES,
    
#if defined(_LOM_)
    HELP_FEY,
    HELP_WISE,
    HELP_FREE,
#endif
    
    HELP_DAY1,
    HELP_GAME_WORKS,
    HELP_MOVING,
    HELP_PRESS_NIGHT,
    HELP_CHOICE_OF_GAMES,
    HELP_VICTORY,
#if defined(_LOM_)
    HELP_ICEFEAR,
#endif
    HELP_BATTLE,
    
    HELP_GROUPED,
    HELP_TUTORIAL,
    HELP_TUTORIAL_OFF,
    HELP_TUTORIAL_ON,
    
    HELP_MAX
};


class helpmanager {
public:
    helpmanager( moonring* mr );
    
    BOOL isShown ( helpid_t id );
    BOOL isImportant( helpid_t id);
    BOOL isAlways( helpid_t id);
    
    BOOL Save ( storyid_t id );
    BOOL Load ( storyid_t id );
    
    void Shown ( helpid_t id );
    
    LPCSTR Get( helpid_t id );
    
public:
    BOOL displayed[ HELP_MAX ];
    
    moonring*   mr;
    
};


#endif /* help_h */
