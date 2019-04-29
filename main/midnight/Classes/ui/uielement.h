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
    ZORDER_FAR                  =0,
    ZORDER_DEFAULT              =1000,
    ZORDER_NEAR                 =2000,
    ZORDER_UI                   =3000,
    ZORDER_POPUP                =4000,
    ZORDER_DRAG                 =5000,
};

enum UIMOUSEOVER {
    MOUSE_OVER_NONE             =0,
    MOUSE_OVER_ALL              =1,
    MOUSE_OVER_DRAG             =2,
    MOUSE_OVER_DROP             =3,
    MOUSE_OVER_USER             =100
} ;

enum UIMOUSEOVERHINT {
    MOUSE_OVER_HINT_ALL         =0,
    MOUSE_OVER_HINT_DRAG        =1,
    MOUSE_OVER_HINT_DROP        =2,
    MOUSE_OVER_HINT_ELEMENT     =3,
} ;

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

#define QUICK_SWIPE_LIMIT                   500
#define MINIMUM_HORIZONTAL_SWIPE_MOVEMENT   25
#define MINIMUM_HORIZONTAL_DRAG_MOVEMENT    16
#define MINIMUM_VERTICAL_DRAG_MOVEMENT      16
#define MINIMUM_VERTICAL_DRAG_GESTURE       25

class uidragevent
{
public:
    enum drageventtype {
        none=0,
        start=1,
        stop=2,
        drag=3,
    };
    
public:
    uidragevent();
    uidragevent(cocos2d::Node* element, cocos2d::Vec2 p, drageventtype type );
    
public:
    cocos2d::Node*  element;
    cocos2d::Vec2   lastposition;
    cocos2d::Vec2   position;
    drageventtype   type;
    u64             time;
    
};

class uidragelement;

class uidragdropdelegate
{
public:
    virtual void OnDragDropNotification( uidragelement* sender, uidragevent* event ) = 0 ;
};



class uidragelement
{
public:
    uidragelement();
    
    virtual void OnStartDrag(uidragevent* event)=0;
    virtual void OnStopDrag(uidragevent* event)=0;
    virtual void OnDrag(uidragevent* event)=0;
    virtual BOOL isDragging() {return dragging;}
    virtual void enableDrag() { drag_enabled = true;}
    virtual void disableDrag() { drag_enabled = false;}
    virtual bool isDragEnabled() { return drag_enabled;}
    virtual void NotifyDragDelegate ( uidragevent* event );
    
public:
    uidragdropdelegate* drag_delegate;
    bool                dragging;
    bool                drag_enabled;
    cocos2d::Vec2       drag_start;
    cocos2d::Vec2       drag_stop;
    cocos2d::Vec2       drag_current;
    cocos2d::Vec2       drag_amount;
    u64                 drag_start_time;
    u64                 drag_stop_time;
    u32                 drag_touch_count;
};

class uidragmoveelement : public uidragelement
{
public:
    uidragmoveelement();
    
    virtual void OnStartDrag(uidragevent* event);
    virtual void OnStopDrag(uidragevent* event);
    virtual void OnDrag(uidragevent* event);
    
public:
    u32     zorder;
    cocos2d::Vec2    start_location;
};

class uidroptarget {
public:
    uidroptarget();
    
    virtual UIMOUSEOVER MouseOverHotspot( cocos2d::Vec2 pos, UIMOUSEOVERHINT hint ) const = 0;
    
    virtual void endDropTarget() { OnDropStop(); }
    virtual void startDropTarget() { OnDropStart(); }
    
    virtual void OnDropStart();
    virtual void OnDropStop();

    virtual void enableDrop() { drop_enabled = true;}
    virtual void disableDrop() { drop_enabled = false;}
    virtual bool isDropEnabled() { return drop_enabled;}
    
public:
    bool isDropFocus;
    bool drop_enabled;
};
