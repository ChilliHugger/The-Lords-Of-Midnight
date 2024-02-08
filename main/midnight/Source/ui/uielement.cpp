//
//  uielement.cpp
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//
#include "../cocos.h"
#include "uielement.h"
#include "helper.h"

USING_NS_CC;
USING_NS_CC_UI;

namespace chilli {
    namespace ui {
        
        Element::Element()
        {
        }

        Element* Element::create()
        {
            Element * ret = new (std::nothrow) Element();
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

        bool Element::init()
        {
            return Layer::init();
        }


        DragEvent::DragEvent() :
            element(nullptr),
            position(Vec2::ZERO),
            type( DragEvent::Type::none),
            time(0)
        {
        }

        DragEvent::DragEvent(Node* element, Vec2 p, DragEvent::Type type) :
            element(element),
            position(p),
            type(type),
            time(0)
        {
        }


        DragElement::DragElement() :
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

        DragMoveElement::DragMoveElement() :
            zorder(0),
            start_location(Vec2::ZERO)
        {
        }

        void DragElement::NotifyDragDelegate ( DragEvent* event )
        {
            if ( drag_delegate )
                drag_delegate->OnDragDropNotification(this, event);
        }

        void DragElement::OnSelectDrag(DragEvent *event)
        {
        }

        void DragElement::OnStartDrag(DragEvent* event)
        {
            drag_start=event->position;
            drag_current = drag_start;
            drag_start_time = event->time;
            dragging=true;
        }

        void DragElement::OnStopDrag(DragEvent* event)
        {
            dragging=false;
            drag_stop_time = event->time;
            drag_stop=event->position;
        }

        void DragElement::OnDrag(DragEvent* event)
        {
            drag_amount = event->position - drag_current;
            drag_current=event->position;
        }


        void DragMoveElement::OnSelectDrag(DragEvent *event)
        {
            DragElement::OnSelectDrag(event);
            
            if ( !event->element )
                return;
            
            start_location = event->element->getPosition();
            zorder=event->element->getLocalZOrder();
            event->element->setLocalZOrder(ZORDER_DRAG);
            NotifyDragDelegate( event );
        }

        void DragMoveElement::OnStartDrag(DragEvent* event)
        {
            DragElement::OnStartDrag(event);
            
            if ( !event->element )
                return;
            
            //start_location = event->element->getPosition();
            //zorder=event->element->getLocalZOrder();
            //event->element->setLocalZOrder(ZORDER_DRAG);
            NotifyDragDelegate( event );
        }

        void DragMoveElement::OnStopDrag(DragEvent* event)
        {
            DragElement::OnStopDrag(event);

            if ( !event->element )
                return;
            
            event->element->setLocalZOrder(zorder);
            
            NotifyDragDelegate( event );
        }

        void DragMoveElement::OnDrag(DragEvent* event)
        {
            DragElement::OnDrag(event);

            if ( !event->element )
                return;
            
            Vec2 location = event->element->getPosition();
            
            event->element->setPosition( location + drag_amount );
            
            NotifyDragDelegate( event );
            
        }

        DropTarget::DropTarget() : isDropFocus(false)
        {
        }

        void DropTarget::OnDropStart()
        {
            isDropFocus = true;
        }

        void DropTarget::OnDropStop()
        {
            isDropFocus = false;
        }


    } // END ui
} // END chilli
