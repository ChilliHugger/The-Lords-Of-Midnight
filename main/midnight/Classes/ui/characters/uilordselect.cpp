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
#include "../../system/resolutionmanager.h"



USING_NS_CC;
using namespace cocos2d::ui;


uilordselect::uilordselect()
{
    disableDrag();
    disableDrop();
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

Vec2 uilordselect::getCenter() const
{
    auto rect = getBoundingBox();
    return Vec2( rect.getMidX(), rect.getMidY() );
}

bool uilordselect::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event* event )
{
    //auto userdata = static_cast<char_data_t*>(getUserData());
    //UIDEBUG("onTouchBegan %s", userdata->symbol.c_str() );
    
    if ( Widget::onTouchBegan(touch, event) ) {
        mouse_down_pos = touch->getLocation();
        mouse_last_position = mouse_down_pos;
        return true;
    }
    
    return false;
}

void uilordselect::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event* event )
{

    //auto userdata = static_cast<char_data_t*>(getUserData());
    //UIDEBUG("onTouchMoved %s", userdata->symbol.c_str() );
    
    Widget::onTouchMoved(touch, event);
    
    if ( isDragEnabled() ) {
        Vec2 delta = touch->getLocation() - mouse_down_pos;

        if ( isDragging() ) {
            uidragevent    dev(this,touch->getLocation(),uidragevent::drag);
            dev.lastposition = mouse_last_position;
            dev.time = utils::getTimeInMilliseconds() ;
            OnDrag(&dev);
            event->stopPropagation();
            setHighlighted(false);
            
        }else if ( ABS(delta.x) > MINIMUM_HORIZONTAL_DRAG_MOVEMENT
                || ABS(delta.y) > MINIMUM_VERTICAL_DRAG_MOVEMENT ) {
            uidragevent    dev(this,touch->getLocation(),uidragevent::start);
            dev.lastposition = mouse_down_pos;
            dev.time = utils::getTimeInMilliseconds() ;
            OnStartDrag(&dev);
            event->stopPropagation();
            setHighlighted(false);
        }
    }

    mouse_last_position = touch->getLocation();
    
    
}

void uilordselect::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event* event )
{
    auto userdata = static_cast<char_data_t*>(getUserData());
    UIDEBUG("onTouchEnded %s", userdata->symbol.c_str() );
    
    Widget::onTouchEnded(touch, event);
    
    if ( isDragging() ) {
        uidragevent    dev(this,touch->getLocation(),uidragevent::stop);
        dev.time = utils::getTimeInMilliseconds() ;
        OnStopDrag(&dev);
        event->stopPropagation();
        _highlight=false;
    }


    
    mouse_last_position = touch->getLocation();
}

void uilordselect::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event* event)
{
    auto userdata = static_cast<char_data_t*>(getUserData());
    UIDEBUG("onTouchCancelled %s", userdata->symbol.c_str() );
    
    Widget::onTouchCancelled(touch, event);
}

void uilordselect::OnStartDrag(uidragevent *event)
{
    uidragmoveelement::OnStartDrag(event);
    refreshStatus();
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

void uilordselect::OnDropStart()
{
    uidroptarget::OnDropStart();
    refreshStatus();
}

void uilordselect::OnDropStop()
{
    uidroptarget::OnDropStop();
    refreshStatus();
}


UIMOUSEOVER uilordselect::MouseOverHotspot( Vec2 pos, UIMOUSEOVERHINT hint ) const
{
    if ( hint == MOUSE_OVER_HINT_DROP ) {

        auto center = getCenter();
        
        // calc the distance from the center
        double dx = ABS(center.x - pos.x);
        double dy = ABS(center.y - pos.y);
        double distance = hypot(dx,dy);
        
        //UIDEBUG("check drop: center(%d,%d) mouse(%d,%d) distance=%d", center.x, center.y, pos.x, pos.y, distance);
        if ( distance < DISTANCE_SHIELD )
            return MOUSE_OVER_FACE ; // face
        
        if ( distance < DISTANCE_INNER )
            return MOUSE_OVER_INNER ; // inner
        
    }
    return MOUSE_OVER_NONE ;
}


bool uilordselect::setLord( mxid id )
{
    characterId = id;
    auto userdata = (char_data_t*)TME_GetEntityUserData( characterId );
    setUserData(userdata);
    return true;
}
