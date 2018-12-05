//
//  uishortcutkeys.h
//  midnight
//
//  Created by Chris Wild on 03/12/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#ifndef uishortcutkeys_h
#define uishortcutkeys_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../library/libinc/mxtypes.h"

#include "../system/keyboardmanager.h"
#include "../frontend/layout_id.h"
#include "../frontend/keyboard_id.h"
#include "../ui/uielement.h"

class uishortcutkeys
{
public:
    bool dispatchShortcutKey( keycode_t keyCode );
    void displayShortcuts();
    
protected:
    keyinfo* getKeyboardShortcut( keycode_t key);
    void addKeyboardListener(cocos2d::Node* node);
    void addShortcutKey( layoutid_t id, KEY_MAP key);
    void addShortcutKey( layoutid_t id, keycode_t key);
    void init( cocos2d::Node* node, WidgetClickCallback callback );
    void addShortcutLabel(cocos2d::Node* node, LPCSTR text);
    cocos2d::Node* checkValidTarget( layoutid_t id );

protected:
    cocos2d::Vector<keyinfo*>   keys;
    cocos2d::Node*              dispatchNode;
    WidgetClickCallback callback;
};


#endif /* uishortcutkeys_h */
