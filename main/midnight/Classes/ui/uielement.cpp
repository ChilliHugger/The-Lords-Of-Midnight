//
//  uielement.cpp
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "uielement.h"
#include "uihelper.h"

USING_NS_CC;
using namespace cocos2d::ui;

uielement::uielement()
{
}

uielement* uielement::create()
{
    uielement * ret = new (std::nothrow) uielement();
    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}

bool uielement::init()
{
    return Layer::init();
}


uidragevent::uidragevent() :
    element(nullptr),
    position(Vec2::ZERO),
    type(uidragevent::none),
    time(0)
{
}

uidragevent::uidragevent(Node* element, Vec2 p, uidragevent::drageventtype type) :
    element(element),
    position(p),
    type(type),
    time(0)
{
}


uidragelement::uidragelement() :
    dragging(false),
    drag_delegate(nullptr),
    drag_start(Vec2::ZERO),
    drag_stop(Vec2::ZERO),
    drag_current(Vec2::ZERO),
    drag_amount(Vec2::ZERO),
    drag_start_time(0),
    drag_stop_time(0),
    drag_touch_count(0)
{
}

uidragmoveelement::uidragmoveelement() :
    zorder(0),
    start_location(Vec2::ZERO)
{
}

void uidragelement::NotifyDragDelegate ( uidragevent* event )
{
    if ( drag_delegate )
        drag_delegate->OnDragDropNotification(this, event);
}

void uidragelement::OnSelectDrag(uidragevent *event)
{
}

void uidragelement::OnStartDrag(uidragevent* event)
{
    drag_start=event->position;
    drag_current = drag_start;
    drag_start_time = event->time;
    dragging=true;
}

void uidragelement::OnStopDrag(uidragevent* event)
{
    dragging=false;
    drag_stop_time = event->time;
    drag_stop=event->position;
}

void uidragelement::OnDrag(uidragevent* event)
{
    drag_amount = event->position - drag_current;
    drag_current=event->position;
}


void uidragmoveelement::OnSelectDrag(uidragevent *event)
{
    uidragelement::OnSelectDrag(event);
    
    if ( !event->element )
        return;
    
    start_location = event->element->getPosition();
    zorder=event->element->getLocalZOrder();
    event->element->setLocalZOrder(ZORDER_DRAG);
    NotifyDragDelegate( event );
}

void uidragmoveelement::OnStartDrag(uidragevent* event)
{
    uidragelement::OnStartDrag(event);
    
    if ( !event->element )
        return;
    
    //start_location = event->element->getPosition();
    //zorder=event->element->getLocalZOrder();
    //event->element->setLocalZOrder(ZORDER_DRAG);
    NotifyDragDelegate( event );
}

void uidragmoveelement::OnStopDrag(uidragevent* event)
{
    uidragelement::OnStopDrag(event);

    if ( !event->element )
        return;
    
    event->element->setLocalZOrder(zorder);
    
    NotifyDragDelegate( event );
}

void uidragmoveelement::OnDrag(uidragevent* event)
{
    uidragelement::OnDrag(event);

    if ( !event->element )
        return;
    
    Vec2 location = event->element->getPosition();
    
    event->element->setPosition( location + drag_amount );
    
    NotifyDragDelegate( event );
    
}

uidroptarget::uidroptarget() : isDropFocus(false)
{
}

void uidroptarget::OnDropStart()
{
    isDropFocus = true;
}

void uidroptarget::OnDropStop()
{
    isDropFocus = false;
}
