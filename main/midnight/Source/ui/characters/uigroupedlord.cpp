//
//  uigroupedlord.cpp
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//
#include "../uihelper.h"
#include "uigroupedlord.h"
#include "uisinglelord.h"
#include "../../tme/tme_interface.h"
#include "../../system/tmemanager.h"
#include "../../system/moonring.h"
#include "../../system/resolutionmanager.h"

USING_NS_AX;

uigroupedlord::uigroupedlord() :
    buttonNode(nullptr)
{
    lordtype = lordwidget::grouped;
}

bool uigroupedlord::init()
{
    if ( uilordselect::init() ) {
        
        setContentSize(Size(DISTANCE_INNER,DISTANCE_INNER));
        
//        auto bg = ax::LayerColor::create(Color4B(0,0,0, 25));
//        bg->setContentSize(this->getContentSize());
//        this->addChild(bg);

        setTouchEnabled(true);
        enableDrag();
        enableDrop();
        
        return true;
    }
    return false;
}

uigroupedlord* uigroupedlord::createWithLord ( mxid characterid )
{
    auto select = uigroupedlord::create();
    select->setLord( characterid );
    return select;
}

bool uigroupedlord::setLord(mxid characterid)
{
    uilordselect::setLord(characterid);
    
    character c;
    TME_GetCharacter(c, characterid );
    
    
    auto face = getFaceImage(c);
    if ( face == nullptr )
        face = Sprite::createWithSpriteFrameName("f_dead!");
    uihelper::AddCenter(this, face, 0, SHIELD_OFFSET_Y);
    
//    titleBackground = ax::LayerColor::create(Color4B(_clrRed));
//    titleBackground->setContentSize(Size(getContentSize().width,RES(16)));
//    titleBackground->setPosition(-(face->getContentSize().width/2),0);
//    face->addChild(titleBackground);
    
    // set name label
    title = Label::createWithTTF( uihelper::font_config_small, c.shortname );
    title->setTextColor(Color4B(_clrWhite));
    title->enableOutline(Color4B(_clrBlack),RES(1));
    //title->setHeight(RES(16));
    title->getFontAtlas()->setAntiAliasTexParameters();
    //title->setWidth(getContentSize().width);
    title->setHorizontalAlignment(TextHAlignment::CENTER);
    title->setVerticalAlignment(TextVAlignment::BOTTOM);
    title->setAnchorPoint(uihelper::AnchorCenter);
    title->setPosition( HALF(face->getContentSize().width),0);
    face->addChild(title);
    
    buttonNode = face;
    
    auto userdata = static_cast<char_data_t*>(getUserData());
    userdata->select_loc = point::ZERO;
    userdata->select_page = InvalidPage;
    
    refreshStatus();
    
    return true;
}

void uigroupedlord::setTitleAlignment(TextHAlignment align)
{
//    f32 x = (align == TextHAlignment::RIGHT) ? RES(-32) : RES(8);
//    //titleBackground->setPosition(Vec2(x,0));
//    title->setAnchorPoint(uihelper::AnchorBottomLeft);
//    title->setPosition(Vec2(x,RES(-8)));
//    title->setHorizontalAlignment(align);
    
}

Sprite* uigroupedlord::getFaceImage(character& c)
{
    if ( Character_IsDead(c) )
        return Sprite::createWithSpriteFrameName("f_dead!");
    
#ifdef _DDR_
    auto face = GetCharacterFace(c);
    return Sprite::create( face );
#else
    auto userdata = static_cast<char_data_t*>(getUserData());
    return Sprite::create( userdata->face );
#endif
    
}

void uigroupedlord::onPressStateChangedToNormal()
{
    if ( buttonNode ) {
        buttonNode->setScale(scale_normal);
        buttonNode->setOpacity(ALPHA(alpha_normal));
    }
}

void uigroupedlord::onPressStateChangedToPressed()
{
    if ( buttonNode ) {
        buttonNode->setScale(scale_3qtr);
        buttonNode->setOpacity(ALPHA(alpha_normal));
    }
}

bool uigroupedlord::hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const
{
    return ax::ui::Widget::hitTest(pt, camera, p)
        && MouseOverHotspot(pt, MOUSE_OVER_HINT_DROP) == MOUSE_OVER_FACE ;
}
