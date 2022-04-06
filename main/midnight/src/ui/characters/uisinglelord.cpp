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

USING_NS_CC;

uisinglelord::uisinglelord() :
    buttonNode(nullptr),
    selectedNode(nullptr),
    locationNode(nullptr),
    innerCircle(nullptr),
    statusNode(nullptr),
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

        selectedNode = Sprite::createWithSpriteFrameName("lord_select_circle");
        selectedNode->setColor(_clrBlue);
        selectedNode->setAnchorPoint(uihelper::AnchorCenter);
        selectedNode->setPosition(Vec2(width/2,height/2));
        selectedNode->setVisible(false);
        selectedNode->setScale(scale);
        addChild(selectedNode);
        
        locationNode = Sprite::createWithSpriteFrameName("map_current_location");
        locationNode->setContentSize(getContentSize());
        locationNode->setAnchorPoint(uihelper::AnchorCenter);
        locationNode->setPosition(Vec2(width/2,height/2));
        locationNode->setVisible(false);
        addChild(locationNode);
        
        innerCircle = DrawNode::create();
        innerCircle->setContentSize( getContentSize() );
        innerCircle->drawSolidCircle(Vec2(width/2,height/2), DISTANCE_INNER/2 , 0, 64, 1.0f, 1.0f, Color4F(0,0,0,0.1f));
        innerCircle->setAnchorPoint(uihelper::AnchorCenter);
        innerCircle->setPosition(Vec2(width/2,height/2));
        innerCircle->setVisible(true);
        addChild(innerCircle);
        

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
    f32 scale = PHONE_SCALE(1.0f) ;
    
    innerCircle->setVisible( isDropFocus );
    setScale( (isDropFocus ? (scale_normal + scale_1qtr) : scale_normal) * scale );

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
    
    f32 width = SELECT_ELEMENT_WIDTH;
    
    // set name label
    auto title = Label::createWithTTF( uihelper::font_config_small, c.longname );
    title->setName("title");
    title->setTextColor(Color4B(_clrWhite));
    title->enableOutline(Color4B(_clrBlack),RES(1));
    title->setLineSpacing(CONTENT_SCALE(0));
    title->setHeight(RES(FONT_SIZE_SMALL)*3); // three lines in case of very long names
    title->getFontAtlas()->setAntiAliasTexParameters();
    title->setAnchorPoint(uihelper::AnchorCenter);
    title->setWidth(width-CONTENT_SCALE(4)); // padding of 4 characters for very long names

    title->setHorizontalAlignment(TextHAlignment::CENTER);
    title->setVerticalAlignment(TextVAlignment::BOTTOM);
    uihelper::AddBottomCenter(face, title, RES(0), CONTENT_SCALE(0));
    

    // Add status image
    statusNode = getStatusImage();
    if ( statusNode!= nullptr ) {
        statusNode->setScale(scale_half);
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
        buttonNode->setScale(scale_normal);
        buttonNode->setOpacity(ALPHA(1.0f));
        if ( statusNode )
            statusNode->setOpacity(ALPHA(1.0f));
    }
}

void uisinglelord::onPressStateChangedToPressed()
{
    if ( buttonNode ) {
        buttonNode->setScale(scale_3qtr);
        buttonNode->setOpacity(ALPHA(1.0f));
        if ( statusNode )
            statusNode->setOpacity(ALPHA(1.0f));
    }
}

void uisinglelord::onPressStateChangedToDisabled()
{
    if ( buttonNode ) {
        buttonNode->setScale(scale_normal);
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


