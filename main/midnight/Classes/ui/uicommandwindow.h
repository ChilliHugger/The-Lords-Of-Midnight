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
#include "../system/helpmanager.h"
#include "../frontend/layout_id.h"

using namespace chilli::types;

USING_NS_CC;

FORWARD_REFERENCE(moonring);
FORWARD_REFERENCE(uipanel);

class uicommandwindow : public uielement {
    
private:
    uicommandwindow();
    
public:
    uicommandwindow( uipanel* parent );
    
    void clearItems();
    void show( MXVoidCallback callback );
    void close();
    
    void updateElements();
    
protected:
    void OnClose();
    void initialiseCommands();
    void addItem( cocos2d::ui::Widget* item, u32 index );
    cocos2d::ui::Widget* findItemById( layoutid_t id );
    void enableItem( layoutid_t id, bool enable );
    void showItem( layoutid_t id, bool show );


protected:
    uipanel*        parent;
    helpid_t        id;
    MXVoidCallback  closeCallback;
    cocos2d::ui::Layout*         layout;
    size            grid;
    size            padding;
    
    Vector<cocos2d::ui::Widget*> elements;
};

