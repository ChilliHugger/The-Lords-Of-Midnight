//
//  uihelpwindow.h
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//
//

#pragma once

#include "../cocos.h"
#include "../library/libinc/mxtypes.h"
#include "uielement.h"
#include "../system/keyboardmanager.h"
#include "../system/helpmanager.h"
#include "../frontend/layout_id.h"
#include "../frontend/keyboard_id.h"
#include "../ui/uishortcutkeys.h"

using namespace chilli::types;

FORWARD_REFERENCE(moonring);
FORWARD_REFERENCE(uipanel);

class uicommandwindow :
    public chilli::ui::Element,
    public uishortcutkeys
{
    using Widget = cocos2d::ui::Widget;
    using Layout = cocos2d::ui::Layout;
    
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
    void addItem( Widget* item, u32 index );
    void enableItem( layoutid_t id, bool enable );
    void showItem( layoutid_t id, bool show );
    Widget* findItemById( layoutid_t id );

    void addTouchListener();
    void addKeyboardListener();

#if defined(_DDR_)
    void setupGiveText();
#endif
    
protected:
    uipanel*        parent;
    helpid_t        id;
    MXVoidCallback  closeCallback;
    Layout*         layout;
    size            grid;
    size            padding;
    
    Vector<Widget*> elements;

};

