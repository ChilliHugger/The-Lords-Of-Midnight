//
//  uishortcutkeys.cpp
//  midnight
//
//  Created by Chris Wild on 03/12/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#include "uishortcutkeys.h"
#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include "uihelper.h"

USING_NS_CC;
//using namespace cocos2d::ui;

void uishortcutkeys::init( Node* node, cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback callback )
{
    this->dispatchNode = node;
    this->callback = callback;
    keys.clear();
}

Node* uishortcutkeys::checkValidTarget( layoutid_t id )
{
    if ( dispatchNode == nullptr )
        return nullptr;
    
    return uihelper::getChildByTagRecursively<ui::Widget*>(id,dispatchNode);
}

void uishortcutkeys::addShortcutKey( layoutid_t id, KEY_MAP key)
{
    Node* node = checkValidTarget(id);
    if ( node == nullptr )
        return;
    
    auto keyboard = moonring::mikesingleton()->keyboard;
    auto k = keyboard->getKeycode(key);
    auto d = keyboard->getKeyboardDescription(k);
    keys.pushBack( new keyinfo( d, k, id) );
    
    addShortcutLabel(node, d);
}

void uishortcutkeys::addShortcutKey( layoutid_t id, keycode_t key)
{
    Node* node = checkValidTarget(id);
    if ( node == nullptr )
        return;
    
    auto keyboard = moonring::mikesingleton()->keyboard;
    auto d = keyboard->getKeyboardDescription(key);
    keys.pushBack( new keyinfo( d, key, id) );
    
    addShortcutLabel(node, d);
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
        }else{
            uishortcutkeys::displayShortcuts();
        }
    };
    
    node->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,node);
    
}

void uishortcutkeys::addShortcutLabel(Node* node, LPCSTR text)
{
    auto title = Label::createWithTTF( uihelper::font_config_shortcut, text );
    title->setName("shortcut_label");
    title->getFontAtlas()->setAntiAliasTexParameters();
    title->setTextColor(Color4B(_clrYellow));
    title->enableOutline(Color4B(_clrBlack),RES(2));
    //title->setAnchorPoint(uihelper::AnchorCenter);
    title->setLocalZOrder(ZORDER_POPUP);
    title->setVisible(false);
    
    // TODO: Work out the best place for the shortcut key to be displayed
    // where it can actually be seend
    uihelper::AddTopLeft(node, title, RES(4), RES(0));
}

void uishortcutkeys::displayShortcuts()
{
    for( auto k : keys ) {

        auto sender = uihelper::getChildByTagRecursively<ui::Widget*>(k->id,dispatchNode);
        if ( sender && sender->isVisible() && sender->isEnabled() ) {
            auto shortcut = sender->getChildByName<Label*>("shortcut_label");
            if ( shortcut == nullptr )
                continue;
            
            shortcut->stopAllActions();
            shortcut->setOpacity(ALPHA(1.0f));
            shortcut->setVisible(true);
            shortcut->runAction(
                                Sequence::create( DelayTime::create(2.0f),
                                                  FadeOut::create(2.0f),
                                                  CallFunc::create( [shortcut] { shortcut->setVisible(false); }),
                                                  nullptr
                                                 )
                                );
        }
        
    }
}
