//
//  keyboardmanager.cpp
//  midnight
//
//  Created by Chris Wild on 15/12/2017.
//
//

#include "../axmol_sdk.h"
#include "keyboardmanager.h"
#include "../frontend/keyboard_id.h"


#if defined(_WIN32) || defined(_WIN64) 
#define strcasecmp _stricmp 
#endif

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
        , KEYCODE(9) // K_USE
#endif
#if defined(_TUNNELS_)
        , KEYCODE(8) // K_TUNNEL
#endif
        , KEYCODE(4) // K_APPROACH,
        , KEYCODE(5) // K_RECRUIT
        , KEYCODE(6) // K_POST
        , KEYCODE(7) // K_ATTACK
        , KEYCODE(G) // K_YES
        , KEYCODE(J) // K_NO
        , KEYCODE(L) // K_LEAVE
        , KEYCODE(D) // K_DISBAND
        , KEYCODE(TAB)  // K_MAP
        , KEYCODE(ESCAPE)  // K_ESC
        , KEYCODE(PG_UP) // K_PAGE_UP
        , KEYCODE(PG_DOWN) // K_PAGE_DOWN
        
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
        , KEYCODE(9) // K_USE
#endif
#if defined(_TUNNELS_)
        , KEYCODE(8) // K_TUNNEL
#endif
        , KEYCODE(P) // K_APPROACH,
        , KEYCODE(R) // K_RECRUIT
        , KEYCODE(G) // K_POST
        , KEYCODE(A) // K_ATTACK
        , KEYCODE(Y) // K_YES
        , KEYCODE(N) // K_NO
        , KEYCODE(L) // K_LEAVE
        , KEYCODE(D) // K_DISBAND
        , KEYCODE(TAB) // K_MAP
        , KEYCODE(ESCAPE)  // K_ESC
        , KEYCODE(PG_UP) // K_PAGE_UP
        , KEYCODE(PG_DOWN) // K_PAGE_DOWN
        
    }
};

const LPCSTR keyboard_descriptions[] = {
    "",
    "PAUSE", "SCR-LCK", "PRINT","SYSREQ", "BRK", "ESC",
    //"BACK",
    "BS","TAB","BACK-TAB","RET","CAPS-LCK","SHIFT",
    //"LSHIFT",
    "RSHIFT","CTRL",
    //"LCTRL",
    "RCTRL","ALT",
    //"LALT",
    "RALT","MENU","HYPER","INS","HOME","PGUP","DEL","END","PGDN",
    "LEFT","RIGHT","UP","DOWN","NUM-LCK",
    "KP+","KP-","KP*","KP/","KPENTER","KPHOME","KP-UP","KP-PGUP","KP-LEFT",
    "KP-FIVE","KP-RIGHT","KP-END","KP-DOWN","KP-PGDN","KP-INS","KP-DEL",
    "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12",
    "SPC","!","\"","#","$","%","^","&","'","[","]","*","+",",","-",".","/",
    "0","1","2","3","4","5","6","7","8","9",
    ":",";","<","=",">","?","@",
    "","","","","","","","","","","","","","","","","","","","","","","","","","", // CAPITAL A-Z
    "(","\\",")","_","`",
    "a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z",
    "{","|","}","~","€","£","","",
    "SRCH","D-LEFT","D-RIGHT","D-UP","D-DOWN","D-CENTER","ENTER","PLAY"
};

LPCSTR keyboardmanager::getKeyboardDescription ( keycode_t key )
{
    //key = key & ~(IS_UP_MASK|IS_SHIFT_MASK|IS_CTRL_MASK|IS_ALT_MASK);
    if ( (u32)key > NUMELE(keyboard_descriptions) )
        return "err";
    return keyboard_descriptions[(u32)key];
}

keycode_t keyboardmanager::getKeyboardValue ( std::string& key )
{
    for (u32 ii=0; ii<NUMELE(keyboard_descriptions); ii++) {
        if ( strcasecmp(key.c_str(), keyboard_descriptions[ii]) == 0 )
            return (keycode_t)ii;
    }
    return KEYCODE(NONE);
}

uikeyboardevent keyboardmanager::createEvent(keycode_t key, bool pressed )
{
    return uikeyboardevent(key,setModifierKey(key, pressed),pressed);
}

flags16_t keyboardmanager::getModifierKeys()
{
    return key_flags;
}

flags16_t keyboardmanager::setModifierKey( keycode_t key, bool pressed )
{
    keyflags_t flags = kf_none;
    
    if ( key == KEYCODE(SHIFT) || key == KEYCODE(RIGHT_SHIFT))
        flags = kf_shift;
    else if ( key == KEYCODE(ALT) || key == KEYCODE(RIGHT_ALT) )
        flags = kf_alt;
    else if ( key == KEYCODE(SHIFT) || key == KEYCODE(RIGHT_CTRL))
        flags = kf_ctrl;
    
    if ( pressed ) {
        key_flags.Set(flags);
    }else{
        key_flags.Reset(flags);
    }
    
    return key_flags;
}

keyboardmanager::keyboardmanager()
{
    key_mapping = &keyboard_map[0][0];
    key_flags.Clear();
}

void keyboardmanager::SetKeyboardMode ( CF_KEYBOARD mode )
{
    key_mapping = &keyboard_map[(int)mode][0];
}

keycode_t keyboardmanager::getKeycode( KEY_MAP key )
{
    return key_mapping[ key ];
}

uikeyboardevent::uikeyboardevent( keycode_t key, flags16_t flags, bool pressed )
{
    this->key = key;
    this->flags.Clear();
    this->flags.Set(flags);
    if ( pressed )
        this->flags.Set(kf_pressed);
    else
        this->flags.Reset(kf_pressed);
}

keycode_t uikeyboardevent::getKey()
{
    return key;
}

bool uikeyboardevent::isShift()
{
    return flags.Is(kf_shift);
}

bool uikeyboardevent::isAlt()
{
    return flags.Is(kf_alt);
}

bool uikeyboardevent::isCtrl()
{
    return flags.Is(kf_ctrl);
}

bool uikeyboardevent::isUp()
{
    return !flags.Is(kf_pressed);
}

bool uikeyboardevent::isDown()
{
    return flags.Is(kf_pressed);
}
