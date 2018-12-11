//
//  uilordselect.cpp
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "uilordselect.h"

#include "../../tme_interface.h"
#include "../../system/tmemanager.h"
#include "../../system/moonring.h"


USING_NS_CC;
using namespace cocos2d::ui;


uilordselect::uilordselect()
{
}

bool uilordselect::init()
{
    return Widget::init();
}

void uilordselect::updateStatus(character& c)
{
}

void uilordselect::refreshStatus()
{
}

void uilordselect::enableDragAndDrop()
{
    //addTouchListener();
}

bool uilordselect::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event* event )
{
    auto userdata = static_cast<char_data_t*>(getUserData());
    UIDEBUG("onTouchBegan %s", userdata->symbol.c_str() );
    
    if ( Widget::onTouchBegan(touch, event) ) {
        mouse_down_pos = touch->getLocation();
        mouse_last_position = mouse_down_pos;
        return true;
    }
    
    return false;
}

void uilordselect::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event* event )
{
    auto userdata = static_cast<char_data_t*>(getUserData());
    UIDEBUG("onTouchMoved %s", userdata->symbol.c_str() );
    
    Widget::onTouchMoved(touch, event);
    
    Vec2 delta = touch->getLocation() - mouse_down_pos;

    if ( isDragging() ) {
        uidragevent    dev(this,touch->getLocation(),uidragevent::drag);
        dev.lastposition = mouse_last_position;
        dev.time = utils::getTimeInMilliseconds() ;
        OnDrag(&dev);
        event->stopPropagation();
        setHighlighted(false);

    }else

    if ( ABS(delta.x) > MINIMUM_HORIZONTAL_DRAG_MOVEMENT
        || ABS(delta.y) > MINIMUM_VERTICAL_DRAG_MOVEMENT ) {
        uidragevent    dev(this,touch->getLocation(),uidragevent::start);
        dev.lastposition = mouse_down_pos;
        dev.time = utils::getTimeInMilliseconds() ;
        OnStartDrag(&dev);
        event->stopPropagation();
        setHighlighted(false);
    }

    mouse_last_position = touch->getLocation();
    
    
}

void uilordselect::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event* event )
{
    auto userdata = static_cast<char_data_t*>(getUserData());
    UIDEBUG("onTouchEnded %s", userdata->symbol.c_str() );
    
    
    
    if ( isDragging() ) {
        uidragevent    dev(this,touch->getLocation(),uidragevent::stop);
        dev.time = utils::getTimeInMilliseconds() ;
        OnStopDrag(&dev);
        event->stopPropagation();
        _highlight=false;
    }

    Widget::onTouchEnded(touch, event);
    
    mouse_last_position = touch->getLocation();
}

void uilordselect::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event* event)
{
    auto userdata = static_cast<char_data_t*>(getUserData());
    UIDEBUG("onTouchCancelled %s", userdata->symbol.c_str() );
    
    Widget::onTouchCancelled(touch, event);
}

void uilordselect::OnStopDrag(uidragevent* event)
{
    uidragmoveelement::OnStopDrag(event);
    
    char_data_t* userdata = static_cast<char_data_t*>( getUserData() );
    if ( userdata != nullptr ) {
        userdata->select_loc.x = getPosition().x;
        userdata->select_loc.y = getPosition().y;
    }
    
}


//void uilordselect::addTouchListener()
//{
//    // TODO: Turn these into gestures
//    // we need swipe, drag, pinch zoom
//    //
//    // mouse events
//    auto listener = EventListenerTouchOneByOne::create();
//
//    // trigger when you push down
//    listener->onTouchBegan = [=](Touch* touch, Event* event){
//
//        auto p = touch->getLocation();
//        p = this->getParent()->convertToNodeSpace(p);
//        cocos2d::Rect rect = this->getBoundingBox();
//
//        auto userdata = static_cast<char_data_t*>(getUserData());
//        UIDEBUG("TouchDown %s [tx=%f] x=%f", userdata->symbol.c_str(), p.x, rect.getMinX()  );
//
//
//        if(rect.containsPoint(p))
//        {
//              UIDEBUG("TouchDown %s", userdata->symbol.c_str() );
//            mouse_down_pos = touch->getLocation();
//            mouse_last_position = mouse_down_pos;
//            //OnMouseEvent(touch,event,true);
//            return true;
//        }
//
//        return false;
//
//    };
//
//    // trigger when moving touch
//    listener->onTouchMoved = [=](Touch* touch, Event* event){
//
//        Vec2 delta = touch->getLocation() - mouse_down_pos;
//
//        if ( isDragging() ) {
//            uidragevent    dev(this,touch->getLocation(),uidragevent::drag);
//            dev.lastposition = mouse_last_position;
//            dev.time = utils::getTimeInMilliseconds() ;
//            OnDrag(&dev);
//            event->stopPropagation();
//            return;
//        }
//
//        if ( ABS(delta.x) > MINIMUM_HORIZONTAL_DRAG_MOVEMENT
//            || ABS(delta.y) > MINIMUM_VERTICAL_DRAG_MOVEMENT ) {
//            uidragevent    dev(this,touch->getLocation(),uidragevent::start);
//            dev.lastposition = mouse_down_pos;
//            dev.time = utils::getTimeInMilliseconds() ;
//            OnStartDrag(&dev);
//            event->stopPropagation();
//        }
//
//        mouse_last_position = touch->getLocation();
//    };
//
//    // trigger when you let up
//    listener->onTouchEnded = [=](Touch* touch, Event* event){
//
//        if ( isDragging() ) {
//            uidragevent    dev(this,touch->getLocation(),uidragevent::stop);
//            dev.time = utils::getTimeInMilliseconds() ;
//            OnStopDrag(&dev);
//            event->stopPropagation();
//            return;
//        }
//
//        mouse_last_position = touch->getLocation();
//          //OnMouseEvent(touch,event,false);
//    };
//
//    // Add listener
//    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
//    //_eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
//
//}
