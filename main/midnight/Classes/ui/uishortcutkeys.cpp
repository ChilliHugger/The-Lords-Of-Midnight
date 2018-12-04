//
//  uishortcutkeys.cpp
//  midnight
//
//  Created by Chris Wild on 03/12/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#include "uishortcutkeys.h"
#include "../system/moonring.h"
#include "uihelper.h"

USING_NS_CC;
//using namespace cocos2d::ui;

void uishortcutkeys::init( Node* node, cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback callback )
{
    this->dispatchNode = node;
    this->callback = callback;
    keys.clear();
}

void uishortcutkeys::addShortcutKey( layoutid_t id, KEY_MAP key)
{
    auto keyboard = moonring::mikesingleton()->keyboard;
    auto k = keyboard->getKeycode(key);
    auto d = keyboard->getKeyboardDescription(k);
    keys.pushBack( new keyinfo( d, k, id) );
}

void uishortcutkeys::addShortcutKey( layoutid_t id, keycode_t key)
{
    auto keyboard = moonring::mikesingleton()->keyboard;
    auto d = keyboard->getKeyboardDescription(key);
    keys.pushBack( new keyinfo( d, key, id) );
}


keyinfo* uishortcutkeys::getKeyboardShortcut( keycode_t key)
{
    for( auto k : keys ) {
        if ( k->key == key )
            return k;
    }
    return nullptr;
}

bool uishortcutkeys::dispatchShortcutKey( keycode_t keyCode )
{
    if ( callback == nullptr || dispatchNode == nullptr )
        return false;
    
    auto key = getKeyboardShortcut(keyCode);
    if ( key != nullptr ) {
        auto sender = uihelper::getChildByTagRecursively<ui::Widget*>(key->id,dispatchNode);
        if ( sender != nullptr && sender->isEnabled() ) {
            
            callback(sender);
            return true;
        }

    }
    
    return false;
}

void uishortcutkeys::addKeyboardListener(Node* node)
{
    if (node == nullptr)
        return;
    
    auto listener = EventListenerKeyboard::create();
    
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event)
    {
        moonring::mikesingleton()->keyboard->setModifierKey(keyCode, true) ;
    };
    
    listener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event)
    {
        moonring::mikesingleton()->keyboard->setModifierKey(keyCode, false) ;
        if ( uishortcutkeys::dispatchShortcutKey(keyCode) ) {
            event->stopPropagation();
        }
    };
    
    node->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,node);
    
}
