//
//  uishortcutkeys.h
//  midnight
//
//  Created by Chris Wild on 03/12/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#ifndef uishortcutkeys_h
#define uishortcutkeys_h

#include "../cocos.h"
#include "../library/inc/mxtypes.h"

#include "../system/keyboardmanager.h"
#include "../frontend/layout_id.h"
#include "../frontend/keyboard_id.h"
#include "../ui/uielement.h"

class uishortcutkeys
{
    using Node = cocos2d::Node;
    using WidgetClickCallback = chilli::ui::WidgetClickCallback;
    template<class T> using Vector = cocos2d::Vector<T>;
public:
    bool dispatchShortcutKey( keycode_t keyCode );
    void displayShortcuts();
    
protected:
    keyinfo* getKeyboardShortcut( keycode_t key);
    void addKeyboardListener(Node* node);
    void addShortcutKey( layoutid_t id, KEY_MAP key);
    void addShortcutKey( layoutid_t id, keycode_t key);
    void addShortcutKey( Node* node, layoutid_t id, keycode_t key);
    void registerCallback( Node* node, WidgetClickCallback callback );
    void addShortcutLabel(Node* node, LPCSTR text);
    Node* checkValidTarget( layoutid_t id );

protected:
    Vector<keyinfo*>   keys;
    Node*              dispatchNode;
    WidgetClickCallback callback;
};


#endif /* uishortcutkeys_h */
