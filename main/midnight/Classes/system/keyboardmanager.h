//
//  keyboardmanager.h
//  midnight
//
//  Created by Chris Wild on 15/12/2017.
//
//

#pragma once

#include "configmanager.h"
#include "../frontend/keyboard_id.h"
#include "../frontend/layout_id.h"

#define KEYCODE(x) \
    cocos2d::EventKeyboard::KeyCode::KEY_##x

typedef cocos2d::EventKeyboard::KeyCode keycode_t;


enum keyflags_t {
    kf_none=0,
    kf_pressed  = MXBIT(1),
    kf_alt      = MXBIT(2),
    kf_ctrl     = MXBIT(3),
    kf_shift    = MXBIT(4),
};

class uikeyboardevent
{
public:
    uikeyboardevent( keycode_t key, flags16_t flags, bool pressed );
    keycode_t getKey();
    bool isShift();
    bool isAlt();
    bool isCtrl();
    bool isUp();
    bool isDown();
    
protected:
    keycode_t   key;
    flags16     flags;
};


class keyinfo : public cocos2d::Ref
{
public:
    keyinfo( LPCSTR name, keycode_t key, layoutid_t id )
    {
        this->name=name;
        this->key=key;
        this->id = id;
        this->autorelease();
    }
    
public:
    LPCSTR      name;
    keycode_t   key;
    layoutid_t  id;
};


class keyboardmanager 
{
public:
	keyboardmanager();
    
    
public:
    void SetKeyboardMode ( CONFIG_KEYBOARD_MODE mode );
    keycode_t getKeycode( KEY_MAP key );
    LPCSTR getKeyboardDescription ( keycode_t key );
    keycode_t getKeyboardValue ( std::string& key );
    uikeyboardevent createEvent(keycode_t key, bool pressed );
    flags16_t setModifierKey( keycode_t key, bool pressed );
    flags16_t getModifierKeys();
    
private:
    keycode_t*  key_mapping;
    flags16     key_flags;
};
