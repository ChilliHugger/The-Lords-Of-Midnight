//
//  keyboardmanager.h
//  midnight
//
//  Created by Chris Wild on 15/12/2017.
//
//

#pragma once

#include "configmanager.h"

#define KEYCODE(x) \
    cocos2d::EventKeyboard::KeyCode::KEY_##x

typedef cocos2d::EventKeyboard::KeyCode keycode_t;


class keyboardmanager 
{
public:
	keyboardmanager();
    
public:
    void SetKeyboardMode ( CONFIG_KEYBOARD_MODE mode );
private:
    keycode_t*                    key_mapping;
};
