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
    buttonNode(nullptr),
    selectedNode(nullptr),
    locationNode(nullptr),
    innerCircle(nullptr),
    statusNode(nullptr),
    statusVisible(true)
{
    lordtype = lordtype_single;
}

bool uisinglelord::init()
{
    if ( uilordselect::init() ) {
        
        f32 scale = resolutionmanager::getInstance()->phoneScale() ;
        f32 width = SELECT_ELEMENT_WIDTH*scale;
        f32 height = SELECT_ELEMENT_HEIGHT*scale;
        
        setContentSize(Size(width,height));

//        bg = cocos2d::LayerColor::create(Color4B(0,0,0, 25));
//        bg->setContentSize(this->getContentSize());
//        bg->setAnchorPoint(uihelper::AnchorCenter);
//        bg->setPosition(Vec2(0,0));
//        addChild(bg);
        
        selectedNode = Sprite::createWithSpriteFrameName("lord_select_circle");
        selectedNode->setColor(_clrBlue);
        selectedNode->setAnchorPoint(uihelper::AnchorCenter);
        selectedNode->setPosition(Vec2(width/2,height/2));
        selectedNode->setVisible(false);
        addChild(selectedNode);
        
        locationNode = Sprite::createWithSpriteFrameName("map_current_location");
        locationNode->setContentSize(this->getContentSize());
        locationNode->setAnchorPoint(uihelper::AnchorCenter);
        locationNode->setPosition(Vec2(width/2,height/2));
        locationNode->setVisible(false);
        addChild(locationNode);
        
        innerCircle = DrawNode::create();
        innerCircle->setContentSize( this->getContentSize() );
        innerCircle->drawSolidCircle(Vec2(width/2,height/2), DISTANCE_INNER/2 , 0, 64, 1.0, 1.0, Color4F(0,0,0,0.1));
        innerCircle->setAnchorPoint(uihelper::AnchorCenter);
        innerCircle->setPosition(Vec2(width/2,height/2));
        innerCircle->setVisible(false);
        addChild(innerCircle);
        
        //        auto outer_circle = DrawNode::create();
        //        outer_circle->setContentSize( this->getContentSize() );
        //        outer_circle->drawSolidCircle(Vec2(width/2,height/2), width/2 , 0, 64, 1.0, 1.0, Color4F(0,0,0, 0.25f));
        //        addChild(outer_circle);
        
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
    innerCircle->setVisible( isDropFocus );
    setScale( isDropFocus ? 1.25 : 1.0 );

    locationNode->setVisible( status.Is(LORD_STATUS::status_location) && !isDragging() && statusVisible );
    selectedNode->setVisible( status.Is(LORD_STATUS::status_selected) && !isDragging() && statusVisible );
    if ( statusNode )
        statusNode->setVisible(statusVisible);
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
    uilordselect::setLord(characterid);
        
    character c;
    TME_GetCharacter(c, characterid );

    updateStatus(c);

    removeChildByName("face");
    removeChildByName("title");
    
    auto face = getFaceImage(c);
    if ( face == nullptr )
        face = Sprite::createWithSpriteFrameName("f_dead!");
    face->setName("face");
    
    
    uihelper::AddCenter(this, face, 0, SHIELD_OFFSET_Y);
    
    f32 scale = resolutionmanager::getInstance()->phoneScale() ;
    f32 width = SELECT_ELEMENT_WIDTH*scale;
    
    // set name label
    auto title = Label::createWithTTF( uihelper::font_config_small, c.longname );
    title->setName("title");
    title->setTextColor(Color4B(_clrWhite));
    title->enableOutline(Color4B(_clrBlack),RES(1));
    title->setLineSpacing(DIS(-2));
    title->setHeight(DIS(32)*scale);
    title->getFontAtlas()->setAntiAliasTexParameters();
    title->setAnchorPoint(uihelper::AnchorCenter);
    title->setWidth(width-DIS(16));

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
    statusVisible = visible;
    refreshStatus();
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


