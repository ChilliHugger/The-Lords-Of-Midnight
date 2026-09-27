//
//  uihelplistwindow.h
//  midnight
//
//  Created by Chris Wild.
//

#pragma once

#include "../axmol_sdk.h"
#include "../library/inc/mxtypes.h"
#include "uielement.h"
#include "../system/helpmanager.h"
#include "../frontend/layout_id.h"
#include "uishortcutkeys.h"

using namespace chilli::types;

FORWARD_REFERENCE(moonring);
FORWARD_REFERENCE(uipanel);

class uihelplistwindow :
    public chilli::ui::Element,
    public uishortcutkeys
{
    using Widget = ax::ui::Widget;
    using Button = ax::ui::Button;
    using Layout = ax::ui::Layout;
    using ScrollView = ax::ui::ScrollView;

private:
    uihelplistwindow();
    ~uihelplistwindow();

public:
    static uihelplistwindow* create( uipanel* parent );

    void show( MXVoidCallback callback );
    void close();

protected:
    bool initWithParent( uipanel* parent );
    void createList();
    Button* createRow( helpid_t id, f32 width, f32 height );
    std::string titleFor( helpid_t id );

    void addTouchListener();
    void OnClose();

protected:
    moonring*       mr;
    uipanel*        parent;
    MXVoidCallback  closeCallback;
    Layout*         layout;
    ScrollView*     scrollView;
};
