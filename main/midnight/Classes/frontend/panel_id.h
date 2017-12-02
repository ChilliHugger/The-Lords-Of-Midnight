//
//  panel_id.h
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef panel_id_h
#define panel_id_h

enum panelmode_t {
    MODE_NONE=0,
    MODE_LOOK=1,
    MODE_THINK,
    MODE_THINK_RECRUITGUARD,
    MODE_THINK_SEEK,
    MODE_THINK_FIGHT,
    MODE_THINK_ARMY,
    MODE_THINK_PERSON,
    MODE_THINK_PLACE,
    MODE_THINK_BATTLE,
    MODE_THINK_APPROACH,
    
    MODE_SELECT,
    MODE_SELECT_RECRUITED,
    MODE_SELECT_WAITING,
    MODE_SELECT_HERE,
    MODE_SELECT_LOYAL,
    MODE_SELECT_LOC,
    MODE_NIGHT,
    MODE_MAP,
    MODE_DEBUG_MAP,
    MODE_LOAD,
    MODE_SAVE,
    
    MODE_WIN,
    MODE_LOSE,
    
    MODE_TITLE,
    MODE_CREDITS,
    MODE_OPTIONS,
    MODE_ADVERT,
    
    MODE_MAINMENU,
};

#endif /* panel_id_h */
