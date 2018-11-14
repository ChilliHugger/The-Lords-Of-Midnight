//
//  uielement.h
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//

#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../library/libinc/mxtypes.h"

FORWARD_REFERENCE(uieventargs);
FORWARD_REFERENCE(uielement);
FORWARD_REFERENCE(uinotificationinterface);

typedef std::function<void(uinotificationinterface* sender,uieventargs* event)> UINotificationCallback;

typedef cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback WidgetClickCallback;
typedef cocos2d::ui::AbstractCheckButton::ccWidgetEventCallback WidgetEventCallback;


enum ZSORT {
    ZORDER_FAR=0,
    ZORDER_DEFAULT=1000,
    ZORDER_NEAR=2000,
    ZORDER_UI=3000,
    ZORDER_POPUP=4000,
    ZORDER_DRAG=5000,
};


class uinotificationinterface
{
public:
    virtual void setNotificationCallback(const UINotificationCallback& callback)
        { notificationCallback = callback; }
    virtual void Notify( uieventargs* args )
        { notificationCallback(this,args); }
    
protected:
    UINotificationCallback      notificationCallback;
};


class uielement : public cocos2d::Layer, public uinotificationinterface
{
public:
    static uielement* create();
    virtual bool init() override;

protected:
        uielement();
        
protected:
    u32                         id;
    c_str                       name;
    flags32                     flags;

};
