//
//  uihelpwindow.h
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//
//

#pragma once

#include "cocos2d.h"
#include "../library/libinc/mxtypes.h"
#include "uielement.h"
#include "../system/keyboardmanager.h"
#include "../system/helpmanager.h"
#include "../frontend/layout_id.h"
#include "../frontend/keyboard_id.h"
#include "../ui/uishortcutkeys.h"


using namespace chilli::types;

USING_NS_CC;

FORWARD_REFERENCE(moonring);
FORWARD_REFERENCE(uipanel);

class uicommandwindow : public uielement, public uishortcutkeys {
    
private:
    uicommandwindow();
    ~uicommandwindow();
    
public:
    
    static uicommandwindow* create( uipanel* parent );
    
    void clearItems();
    void show( MXVoidCallback callback );
    void close();
    void updateElements();
    
protected:
    bool initWithParent( uipanel* parent );
    void OnClose();
    void initialiseCommands();
    void addItem( cocos2d::ui::Widget* item, u32 index );
    void setupGiveText();
    void enableItem( layoutid_t id, bool enable );
    void showItem( layoutid_t id, bool show );
    cocos2d::ui::Widget* findItemById( layoutid_t id );

    void addTouchListener();
    void addKeyboardListener();
    
protected:
    uipanel*        parent;
    helpid_t        id;
    MXVoidCallback  closeCallback;
    cocos2d::ui::Layout*         layout;
    size            grid;
    size            padding;
    
    Vector<cocos2d::ui::Widget*> elements;

};

