//
//  uielement.h
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//

// Windows build needs definition upfront
#ifdef _OS_WINDOWS_BUILD_
#include "../cocos.h"
namespace chilli {
    namespace ui {

        typedef cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback WidgetClickCallback;
        typedef cocos2d::ui::AbstractCheckButton::ccWidgetEventCallback WidgetEventCallback;
    }
}
#endif

#ifndef __UIELEMENT_H_INCLUDED__
#define __UIELEMENT_H_INCLUDED__

#include "../cocos.h"
#include "../library/inc/mxtypes.h"

FORWARD_REFERENCE(uieventargs);
FORWARD_REFERENCE(uielement);
FORWARD_REFERENCE(uinotificationinterface);


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

namespace chilli {
    namespace ui {

#ifndef _OS_WINDOWS_BUILD_
        typedef cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback WidgetClickCallback;
        typedef cocos2d::ui::AbstractCheckButton::ccWidgetEventCallback WidgetEventCallback;
#endif

    class NotificationInterface
    {
        using NotificationCallback = std::function<void(NotificationInterface* sender,uieventargs* event)>;
    public:
        virtual void setNotificationCallback(const NotificationCallback& callback)    { notificationCallback = callback; }
        virtual void Notify( uieventargs* args )                                        { notificationCallback(this,args); }
        
    protected:
        NotificationCallback      notificationCallback;
    };
        
    class Element :
        public cocos2d::Layer,
        public NotificationInterface
    {
    public:
        static Element* create();
        virtual bool init() override;

    protected:
        Element();
        
    protected:
        u32                         id;
        c_str                       name;
        flags32                     flags;

    };

    class DragEvent
    {
    protected:
        using Node = cocos2d::Node;
        using Vec2 = cocos2d::Vec2;
        
    public:
        enum class Type {
            none=0,
            select=1,
            start=2,
            stop=3,
            drag=4,
            
        };
        
    public:
        DragEvent();
        DragEvent(Node* element, Vec2 p, Type type );
        
    public:
        Node*  element;
        Vec2   lastposition;
        Vec2   position;
        Type   type;
        u64    time;
        
    };

    class DragElement;

    class uidragdropdelegate
    {
    public:
        virtual void OnDragDropNotification( DragElement* sender, DragEvent* event ) = 0 ;
    };



    class DragElement
    {
        using Vec2 = cocos2d::Vec2;
        
    public:
        DragElement();
        
        virtual void OnSelectDrag(DragEvent* event)=0;
        virtual void OnStartDrag(DragEvent* event)=0;
        virtual void OnStopDrag(DragEvent* event)=0;
        virtual void OnDrag(DragEvent* event)=0;
        virtual BOOL isDragging() {return dragging;}
        virtual void enableDrag() { drag_enabled = true;}
        virtual void disableDrag() { drag_enabled = false;}
        virtual bool isDragEnabled() { return drag_enabled;}
        virtual void NotifyDragDelegate ( DragEvent* event );
        
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

    class DragMoveElement : public DragElement
    {
        using Vec2 = cocos2d::Vec2;
    public:
        DragMoveElement();
        
        virtual void OnSelectDrag(DragEvent* event);
        virtual void OnStartDrag(DragEvent* event);
        virtual void OnStopDrag(DragEvent* event);
        virtual void OnDrag(DragEvent* event);
        
    public:
        u32     zorder;
        Vec2    start_location;
    };

    class DropTarget
    {
        using Vec2 = cocos2d::Vec2;
    public:
        DropTarget();
        
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

    }
}


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

#endif // __UIELEMENT_H_INCLUDED__


