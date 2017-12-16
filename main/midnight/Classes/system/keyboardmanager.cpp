//
//  keyboardmanager.cpp
//  midnight
//
//  Created by Chris Wild on 15/12/2017.
//
//

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "keyboardmanager.h"
#include "../frontend/keyboard_id.h"

// c luxor
// v morkin
// b corleth
// n rorthron

static keycode_t keyboard_map[][K_MAP_MAX] = {
    {
          KEYCODE(E) // K_LOOK
        , KEYCODE(E) // K_DAWN
        , KEYCODE(U) // K_NIGHT
        , KEYCODE(U) // K_PERSON,
        , KEYCODE(R) // K_PLACE,
        , KEYCODE(Y) // K_ARMY,
        , KEYCODE(T) // K_BATTLE,
        , KEYCODE(M) // K_SELECT
        , KEYCODE(R) // K_THINK
        , KEYCODE(T) // K_CHOOSE
#if defined(_LOM_)
        , KEYCODE(1) // K_SEEK
        , KEYCODE(2) // K_HIDE,
        , KEYCODE(2) // K_UNHIDE,
        , KEYCODE(3) // K_FIGHT,
#endif
#if defined(_DDR_)
        , KEYCODE(3) // K_GIVE
        , KEYCODE(2) // K_TAKE
        , KEYCODE(1) // K_REST
        , KEYCODE(4) // K_TUNNEL
        , KEYCODE(9) // K_USE
#endif
        , KEYCODE(4) // K_APPROACH,
        , KEYCODE(5) // K_RECRUIT
        , KEYCODE(6) // K_POST
        , KEYCODE(7) // K_ATTACK
        , KEYCODE(G) // K_YES
        , KEYCODE(J) // K_NO
        , KEYCODE(L) // K_LEAVE
        , KEYCODE(D) // K_DISBAND
    },
    {
          KEYCODE(L) // K_LOOK
        , KEYCODE(D) // K_DAWN
        , KEYCODE(N) // K_NIGHT
        , KEYCODE(F4) // K_PERSON,
        , KEYCODE(F3) // K_PLACE,
        , KEYCODE(F2) // K_ARMY,
        , KEYCODE(F1) // K_BATTLE,
        , KEYCODE(S) // K_SELECT
        , KEYCODE(T) // K_THINK
        , KEYCODE(C) // K_CHOOSE
#if defined(_LOM_)
        , KEYCODE(K) // K_SEEK
        , KEYCODE(H) // K_HIDE,
        , KEYCODE(U) // K_UNHIDE,
        , KEYCODE(F) // K_FIGHT,
#endif
#if defined(_DDR_)
        , KEYCODE(3) // K_GIVE
        , KEYCODE(2) // K_TAKE
        , KEYCODE(1) // K_REST
        , KEYCODE(4) // K_TUNNEL
        , KEYCODE(9) // K_USE
#endif
        , KEYCODE(P) // K_APPROACH,
        , KEYCODE(R) // K_RECRUIT
        , KEYCODE(G) // K_POST
        , KEYCODE(A) // K_ATTACK
        , KEYCODE(Y) // K_YES
        , KEYCODE(N) // K_NO
        , KEYCODE(L) // K_LEAVE
        , KEYCODE(D) // K_DISBAND
        
    }
};

keyboardmanager::keyboardmanager()
{
    key_mapping = &keyboard_map[0][0];
}

void keyboardmanager::SetKeyboardMode ( CONFIG_KEYBOARD_MODE mode )
{
    key_mapping = &keyboard_map[mode][0];
    
}

