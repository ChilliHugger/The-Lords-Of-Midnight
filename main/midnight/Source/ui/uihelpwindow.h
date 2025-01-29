//
//  uihelpwindow.h
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//
//

#pragma once

#include "../axmol_sdk.h"
#include "../library/inc/mxtypes.h"
#include "uielement.h"
#include "../system/helpmanager.h"
#include "uishortcutkeys.h"

using namespace chilli::types;

FORWARD_REFERENCE(moonring);
FORWARD_REFERENCE(uipanel);

class uihelpwindow :
    public chilli::ui::Element,
    public uishortcutkeys
{
    using Scene = ax::Scene;
    using Layout = ax::ui::Layout;
    
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
    moonring*       mr;
    uipanel*        parent;
    helpid_t        id;
    MXVoidCallback  closeCallback;
    Layout*         layout;
    
};

