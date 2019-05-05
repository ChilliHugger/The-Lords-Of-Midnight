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
#include "../system/helpmanager.h"
#include "uishortcutkeys.h"

using namespace chilli::types;

FORWARD_REFERENCE(moonring);
FORWARD_REFERENCE(uipanel);

class uihelpwindow :
    public uielement,
    public uishortcutkeys
{
    using Scene = cocos2d::Scene;
    using Layout = cocos2d::ui::Layout;
    
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
    Scene*          parent;
    helpid_t        id;
    MXVoidCallback  closeCallback;
    Layout*         layout;
    
};

