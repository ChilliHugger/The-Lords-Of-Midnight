//
//  uilordselect.cpp
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//
#include "../uihelper.h"
#include "uilordselect.h"
#include "../../tme_interface.h"
#include "../../system/tmemanager.h"
#include "../../system/moonring.h"
#include "../../system/resolutionmanager.h"

USING_NS_CC;

using DragEventType = chilli::ui::DragEvent::Type;

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

bool uilordselect::onTouchBegan(Touch *touch, Event* event )
{
    //auto userdata = static_cast<char_data_t*>(getUserData());
    //UIDEBUG("onTouchBegan %s", userdata->symbol.c_str() );
    
    if ( Widget::onTouchBegan(touch, event) ) {
        mouse_down_pos = touch->getLocation();
        mouse_last_position = mouse_down_pos;
        
        uidragevent    dev(this,touch->getLocation(), DragEventType::select);
        dev.lastposition = mouse_down_pos;
        dev.time = utils::getTimeInMilliseconds() ;
        OnSelectDrag(&dev);
        
        return true;
    }
    
    return false;
}

void uilordselect::onTouchMoved(Touch *touch, Event* event )
{
    //auto userdata = static_cast<char_data_t*>(getUserData());
    //UIDEBUG("onTouchMoved %s", userdata->symbol.c_str() );
    
    Widget::onTouchMoved(touch, event);
    
    if ( isDragEnabled() ) {
        Vec2 delta = touch->getLocation() - mouse_down_pos;

        if ( isDragging() ) {
            uidragevent    dev(this,touch->getLocation(),DragEventType::drag);
            dev.lastposition = mouse_last_position;
            dev.time = utils::getTimeInMilliseconds() ;
            OnDrag(&dev);
            setHighlighted(false);
            
        }else if ( ABS(delta.x) > MINIMUM_HORIZONTAL_DRAG_MOVEMENT
                || ABS(delta.y) > MINIMUM_VERTICAL_DRAG_MOVEMENT ) {
            uidragevent    dev(this,touch->getLocation(),DragEventType::start);
            dev.lastposition = mouse_down_pos;
            dev.time = utils::getTimeInMilliseconds() ;
            OnStartDrag(&dev);
            setHighlighted(false);
        }
        
        event->stopPropagation();
        
    }

    mouse_last_position = touch->getLocation();
    
    
}

void uilordselect::onTouchEnded(Touch *touch, Event* event )
{
    //auto userdata = userData();
    //UIDEBUG("onTouchEnded %s", userdata->symbol.c_str() );
    
    Widget::onTouchEnded(touch, event);
    
    if ( isDragging() ) {
        uidragevent    dev(this,touch->getLocation(),DragEventType::stop);
        dev.time = utils::getTimeInMilliseconds() ;
        OnStopDrag(&dev);
        event->stopPropagation();
        _highlight=false;
    }


    
    mouse_last_position = touch->getLocation();
}

void uilordselect::onTouchCancelled(Touch *touch, Event* event)
{
    //auto userdata = userData();
    //UIDEBUG("onTouchCancelled %s", userdata->symbol.c_str() );
    
    Widget::onTouchCancelled(touch, event);
}

void uilordselect::OnStartDrag(uidragevent *event)
{
    DragMoveElement::OnStartDrag(event);
    refreshStatus();
}

void uilordselect::OnStopDrag(uidragevent* event)
{
    DragMoveElement::OnStopDrag(event);
    
    char_data_t* userdata = static_cast<char_data_t*>( getUserData() );
    if ( userdata != nullptr ) {
        userdata->select_loc.x = getPosition().x;
        userdata->select_loc.y = getPosition().y;
    }
}

void uilordselect::OnDropStart()
{
    DropTarget::OnDropStart();
    refreshStatus();
}

void uilordselect::OnDropStop()
{
    DropTarget::OnDropStop();
    refreshStatus();
}

f32 uilordselect::getDistanceFromCentre( Vec2 pos ) const
{
    auto center = getWorldPosition(); //getCenter();

    // calc the distance from the center
    f32 dx = ABS(center.x - pos.x);
    f32 dy = ABS(center.y - pos.y);
    f32 distance = hypot(dx,dy);

    //UIDEBUG("getDistanceFromCentre: center(%f,%f) mouse(%f,%f) distance=%f", center.x, center.y, pos.x, pos.y, distance);
    
    return distance;
}

UIMOUSEOVER uilordselect::MouseOverHotspot( Vec2 pos, UIMOUSEOVERHINT hint ) const
{
    if ( hint == MOUSE_OVER_HINT_DROP ) {

        f32 scale = getScale();
        
        f32 distance = getDistanceFromCentre(pos);
        
        if ( distance < (DISTANCE_SHIELD * scale) )
            return MOUSE_OVER_FACE ; // face
        
        if ( distance < (DISTANCE_INNER * scale) )
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
