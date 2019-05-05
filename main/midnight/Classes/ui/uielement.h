//
//  uielement.h
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//

#pragma once

#include "../cocos.h"
#include "../library/libinc/mxtypes.h"

FORWARD_REFERENCE(uieventargs);
FORWARD_REFERENCE(uielement);
FORWARD_REFERENCE(uinotificationinterface);

typedef std::function<void(uinotificationinterface* sender,uieventargs* event)> UINotificationCallback;

typedef cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback WidgetClickCallback;
typedef cocos2d::ui::AbstractCheckButton::ccWidgetEventCallback WidgetEventCallback;

#define QUICK_SWIPE_LIMIT                   500
#define MINIMUM_HORIZONTAL_SWIPE_MOVEMENT   25
#define MINIMUM_HORIZONTAL_DRAG_MOVEMENT    16
#define MINIMUM_VERTICAL_DRAG_MOVEMENT      16
#define MINIMUM_VERTICAL_DRAG_GESTURE       25

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


class uielement :
    public cocos2d::Layer,
    public uinotificationinterface
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

class uidragevent
{
protected:
    using Node = cocos2d::Node;
    
public:
    enum drageventtype {
        none=0,
        select=1,
        start=2,
        stop=3,
        drag=4,
        
    };
    
public:
    uidragevent();
    uidragevent(Node* element, Vec2 p, drageventtype type );
    
public:
    Node*           element;
    Vec2            lastposition;
    Vec2            position;
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
    
    virtual void OnSelectDrag(uidragevent* event)=0;
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
    Vec2                drag_start;
    Vec2                drag_stop;
    Vec2                drag_current;
    Vec2                drag_amount;
    u64                 drag_start_time;
    u64                 drag_stop_time;
    u32                 drag_touch_count;
};

class uidragmoveelement : public uidragelement
{
public:
    uidragmoveelement();
    
    virtual void OnSelectDrag(uidragevent* event);
    virtual void OnStartDrag(uidragevent* event);
    virtual void OnStopDrag(uidragevent* event);
    virtual void OnDrag(uidragevent* event);
    
public:
    u32     zorder;
    Vec2    start_location;
};

class uidroptarget {
public:
    uidroptarget();
    
    virtual UIMOUSEOVER MouseOverHotspot( Vec2 pos, UIMOUSEOVERHINT hint ) const = 0;
    
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

static const f32 scale_double = 2.0f;
static const f32 scale_normal = 1.0f;
static const f32 scale_3qtr = 0.75f;
static const f32 scale_half = 0.5f;
static const f32 scale_1qtr = 0.25f;
static const f32 scale_zero = 0.0f;

static const f32 alpha_normal = 1.0f;
static const f32 alpha_3qtr = 0.75f;
static const f32 alpha_half = 0.5f;
static const f32 alpha_1qtr = 0.25f;
static const f32 alpha_zero = 0.0f;

#define HALF(x) (x/2)



