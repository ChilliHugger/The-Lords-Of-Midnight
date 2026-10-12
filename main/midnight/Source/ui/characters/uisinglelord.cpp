//
//  uisinglelord.cpp
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//
#include "../uihelper.h"
#include "uisinglelord.h"
#include "../../tme/tme_interface.h"
#include "../../system/tmemanager.h"
#include "../../system/moonring.h"
#include "../../system/resolutionmanager.h"

USING_NS_AX;

uisinglelord::uisinglelord() :
    buttonNode(nullptr),
    statusVisible(true)
{
    lordtype = lordwidget::single;
}

bool uisinglelord::init()
{
    if ( uilordselect::init() ) {
        
        f32 scale = scale_normal;
        f32 width = SELECT_ELEMENT_WIDTH*scale;
        f32 height = SELECT_ELEMENT_HEIGHT*scale;
        
        setContentSize(Size(width,height));

        setTouchEnabled(true);
        
        return true;
    }
    return false;
}

uisinglelord* uisinglelord::createWithLord ( mxid characterid )
{
    auto select = uisinglelord::create();
    select->setLord( characterid );
    return select;
}

void uisinglelord::updateStatus(character& c)
{
    status.Clear();
    
    if ( Character_IsInTunnel(c))
        status.Set(LORD_STATUS::status_tunnel);

    if ( Character_IsDawn(c) )
        status.Set(LORD_STATUS::status_dawn);
    
    if ( Character_IsInBattle(c) )
        status.Set(LORD_STATUS::status_inbattle);
    
#if defined(_DDR_)
    if ( Character_IsPreparingForBattle(c) )
        status.Set(LORD_STATUS::status_inbattle);
#endif
    
    if ( Character_IsNight(c) )
        status.Set(LORD_STATUS::status_night);
    
    if ( Character_IsDead(c) )
        status.Set(LORD_STATUS::status_dead);
    
    if ( c.id == TME_CurrentCharacter().id )
        status.Set(LORD_STATUS::status_selected|LORD_STATUS::status_location);
    
    if ( c.location == TME_CurrentCharacter().location )
        status.Set(LORD_STATUS::status_location);
    
    if ( Character_IsInTunnel(c) != Character_IsInTunnel(TME_CurrentCharacter()))
        status.Reset(LORD_STATUS::status_location);    
}

void uisinglelord::refreshStatus()
{
    f32 scale = PHONE_SCALE(1.0f) ;
    
    setScale( (isDropFocus ? (scale_normal + scale_1qtr) : scale_normal) * scale );

    renderLord();
}

std::string uisinglelord::statusImageName() const
{
    // TODO: status_tunnel

    if ( status.Is(LORD_STATUS::status_inbattle))
        return "battle";

    if ( status.Is(LORD_STATUS::status_night))
        return "night";

    if ( status.Is(LORD_STATUS::status_dawn))
        return "dawn";

    return "";
}

// the face, name and status badge are one picture; the badge is hidden while dragging and when disabled
void uisinglelord::renderLord()
{
    if ( faceArt.empty() )
        return;

    // the circles behind the lord and the badge are part of its picture; none show while dragging
    std::vector<std::string> show;
    if ( !isDragging() && statusVisible ) {
        if ( status.Is(LORD_STATUS::status_selected) )
            show.push_back("selected");
        if ( status.Is(LORD_STATUS::status_location) )
            show.push_back("location");
        if ( !dimmed && !statusImageName().empty() )
            show.push_back(statusImageName());
    }
    // a faint circle shows while another lord is being dragged over this one
    if ( isDropFocus )
        show.push_back("drop");
    renderLordImage("lord_select", faceArt, show, SHIELD_OFFSET_Y);
    buttonNode = lordImage;
}

bool uisinglelord::setLord( mxid characterid )
{
    uilordselect::setLord(characterid);
        
    character c;
    TME_GetCharacter(c, characterid );

    updateStatus(c);

    faceArt = faceName(c);
    renderLord();
    setLordTitle(c.longname, true);
    
    refreshStatus();
    
    return true;
}

void uisinglelord::setStatusImageVisible( bool visible )
{
    statusVisible = visible;
    refreshStatus();
}

void uisinglelord::onPressStateChangedToNormal()
{
    dimmed = false;
    renderLord();
    if ( buttonNode ) {
        buttonNode->setScale(scale_normal);
        buttonNode->setOpacity(ALPHA(1.0f));
    }
}

void uisinglelord::onPressStateChangedToPressed()
{
    dimmed = false;
    renderLord();
    if ( buttonNode ) {
        buttonNode->setScale(scale_3qtr);
        buttonNode->setOpacity(ALPHA(1.0f));
    }
}

void uisinglelord::onPressStateChangedToDisabled()
{
    dimmed = true;
    renderLord();
    if ( buttonNode ) {
        buttonNode->setScale(scale_normal);
        buttonNode->setOpacity(ALPHA(0.25f));
    }
}

void uisinglelord::OnStopDrag(uidragevent* event)
{
    uilordselect::OnStopDrag(event);
    setStatusImageVisible(true);
}

void uisinglelord::OnStartDrag(uidragevent* event)
{
    uilordselect::OnStartDrag(event);
    setStatusImageVisible(false);
}


