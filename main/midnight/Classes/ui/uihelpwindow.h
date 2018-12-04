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
#include "uishortcutkeys.h"

using namespace chilli::types;

USING_NS_CC;

FORWARD_REFERENCE(moonring);
FORWARD_REFERENCE(uipanel);

class uihelpwindow : public uielement, public uishortcutkeys {
    
private:
    uihelpwindow();
    ~uihelpwindow();
    
public:
    static uihelpwindow* create(uipanel* parent, helpid_t id);
    
    void Show( MXVoidCallback callback );
    void Close();
    
protected:
    bool initWithParent( uipanel* parent, helpid_t id );
    void addTouchListener();
    void addKeyboardListener();
    void OnClose();
    
protected:
    Scene*                  parent;
    helpid_t                id;
    MXVoidCallback          closeCallback;
    cocos2d::ui::Layout*    layout;
    
};

