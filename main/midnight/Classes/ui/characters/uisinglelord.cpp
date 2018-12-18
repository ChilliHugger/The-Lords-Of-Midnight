//
//  uisinglelord.cpp
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "uisinglelord.h"
#include "../uihelper.h"
#include "../../tme_interface.h"
#include "../../system/tmemanager.h"
#include "../../system/resolutionmanager.h"

USING_NS_CC;
using namespace cocos2d::ui;



uisinglelord::uisinglelord() :
    buttonNode(nullptr)
    //userdata(nullptr)
{
}

bool uisinglelord::init()
{
    if ( uilordselect::init() ) {
        f32 scale = resolutionmanager::getInstance()->phoneScale() ;
        setContentSize(Size(SELECT_ELEMENT_WIDTH*scale, SELECT_ELEMENT_HEIGHT*scale));
//        auto bg = cocos2d::LayerColor::create(Color4B(0,0,0, 25));
//        bg->setContentSize(this->getContentSize());
//        this->addChild(bg);
        
        this->setTouchEnabled(true);
        this->enableDragAndDrop();
        
        selectedNode = Sprite::createWithSpriteFrameName("lord_select_circle");
        selectedNode->setColor(_clrBlue);
        uihelper::AddCenter(this, selectedNode, 0, DIS(-9));
        selectedNode->setVisible(false);
        
        locationNode = Sprite::createWithSpriteFrameName("map_current_location");
        locationNode->setContentSize(this->getContentSize());
        uihelper::AddCenter(this, locationNode);
        locationNode->setVisible(false);
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
    
#if defined(_DDR_)
    if ( Character_IsInTunnel(c))
        status.Set(LORD_STATUS::status_tunnel);
#endif
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
    
#if defined(_DDR_)
    if ( Character_IsInTunnel(c) != Character_IsInTunnel(TME_CurrentCharacter()))
        status.Reset(LORD_STATUS::status_location);
#endif
    
}

void uisinglelord::refreshStatus()
{
    locationNode->setVisible( status.Is(LORD_STATUS::status_location) );
    selectedNode->setVisible( status.Is(LORD_STATUS::status_selected) );
}

Sprite* uisinglelord::getStatusImage()
{
    // TODO: status_tunnel
    
    if ( status.Is(LORD_STATUS::status_inbattle))
        return Sprite::createWithSpriteFrameName("lord_battle");

    if ( status.Is(LORD_STATUS::status_night))
        return Sprite::createWithSpriteFrameName("lord_night");

    if ( status.Is(LORD_STATUS::status_dawn))
        return Sprite::createWithSpriteFrameName("lord_dawn");
    
    return nullptr;
}

Sprite* uisinglelord::getFaceImage(character& c)
{
    if ( status.Is(LORD_STATUS::status_dead))
        return Sprite::createWithSpriteFrameName("f_dead!");
    
    // TODO:
    // 1. start with get shield
    // 2. if there is no shield the get a face
    // 3. then get a race
    
    
#ifdef _DDR_
    auto face = GetCharacterFace(c);
    return Sprite::create( face );
#else
    auto userdata = static_cast<char_data_t*>(getUserData());
    return Sprite::create( userdata->face );
#endif

}


bool uisinglelord::setLord( mxid characterid )
{
    auto userdata = (char_data_t*)TME_GetEntityUserData( characterid );
    setUserData(userdata);
    
    character c;
    TME_GetCharacter(c, characterid );
    
    updateStatus(c);

    auto face = getFaceImage(c);
    if ( face == nullptr )
        face = Sprite::createWithSpriteFrameName("f_dead!");
    
    uihelper::AddCenter(this, face);
    
    f32 scale = resolutionmanager::getInstance()->phoneScale() ;
    
    
    // set name label
    auto title = Label::createWithTTF( uihelper::font_config_small, c.longname );
    title->getFontAtlas()->setAntiAliasTexParameters();
    title->setTextColor(Color4B(_clrWhite));
    title->enableOutline(Color4B(_clrBlack),RES(1));
    title->setAnchorPoint(uihelper::AnchorCenter);
    title->setWidth(getContentSize().width-DIS(16));
    title->setLineSpacing(DIS(-2));
    title->setHeight(DIS(32)*scale);
    title->setHorizontalAlignment(TextHAlignment::CENTER);
    title->setVerticalAlignment(TextVAlignment::BOTTOM);
    uihelper::AddBottomCenter(face, title, RES(0), DIS(-16));
    

    // Add status image
    statusNode = getStatusImage();
    if ( statusNode!= nullptr ) {
        statusNode->setScale(0.5f);
        face->addChild(statusNode);
        uihelper::PositionCenterAnchor(statusNode, uihelper::AnchorTopLeft);
    }
    
    buttonNode = face;
    
    refreshStatus();
    
    return true;
}

void uisinglelord::setStatusImageVisible( bool visible )
{
    statusNode->setVisible(visible);
}

void uisinglelord::onPressStateChangedToNormal()
{
    if ( buttonNode ) {
        buttonNode->setScale(1.0f);
        buttonNode->setOpacity(ALPHA(1.0f));
        if ( statusNode )
            statusNode->setOpacity(ALPHA(1.0f));
    }
}

void uisinglelord::onPressStateChangedToPressed()
{
    if ( buttonNode ) {
        buttonNode->setScale(0.75f);
        buttonNode->setOpacity(ALPHA(1.0f));
        if ( statusNode )
            statusNode->setOpacity(ALPHA(1.0f));
    }
}

void uisinglelord::onPressStateChangedToDisabled()
{
    if ( buttonNode ) {
        buttonNode->setScale(1.0f);
        buttonNode->setOpacity(ALPHA(0.25f));
        if ( statusNode )
            statusNode->setOpacity(ALPHA(0.0f));
    }
}
