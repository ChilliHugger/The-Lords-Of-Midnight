//
//  keyboard_id.h
//  midnight
//
//  Created by Chris Wild on 07/12/2017.
//

#ifndef keyboard_id_h
#define keyboard_id_h

enum MOUSE_CURSOR {
    MOUSE_NORMAL=0,
    MOUSE_LOOK_LEFT,
    MOUSE_LOOK_RIGHT,
    MOUSE_MOVE,
};

enum KEY_MAP {
    K_LOOK,
    K_DAWN,
    K_NIGHT,
    K_PERSON,
    K_PLACE,
    K_ARMY,
    K_BATTLE,
    K_SELECT,
    K_THINK,
    K_CHOOSE,
#ifndef _DDR_
    K_SEEK,
    K_HIDE,
    K_UNHIDE,
    K_FIGHT,
#else
    K_GIVE,
    K_TAKE,
    K_REST,
    K_TUNNEL,
    K_USE,
#endif
    K_APPROACH,
    K_RECRUIT,
    K_POST,
    K_ATTACK,
    K_YES,
    K_NO,
    K_LEAVE,
    K_DISBAND,
    K_MAP_MAX,
};


#endif /* keyboard_id_h */
