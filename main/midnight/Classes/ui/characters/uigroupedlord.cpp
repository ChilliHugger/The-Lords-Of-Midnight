//
//  uigroupedlord.cpp
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "uigroupedlord.h"
#include "uisinglelord.h"
#include "../uihelper.h"
#include "../../tme_interface.h"
#include "../../system/tmemanager.h"
#include "../../system/resolutionmanager.h"

USING_NS_CC;
using namespace cocos2d::ui;

uigroupedlord::uigroupedlord() :
    buttonNode(nullptr)
{
    lordtype = lordtype_grouped;
}

bool uigroupedlord::init()
{
    if ( uilordselect::init() ) {
        
        f32 width = DISTANCE_INNER;
        f32 height = DISTANCE_INNER;
        setContentSize(Size(width,height));
        
//        auto bg = cocos2d::LayerColor::create(Color4B(0,0,0, 25));
//        bg->setContentSize(this->getContentSize());
//        this->addChild(bg);

        this->setTouchEnabled(true);
        this->enableDrag();
        this->enableDrop();
        
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
    
//    titleBackground = cocos2d::LayerColor::create(Color4B(_clrRed));
//    titleBackground->setContentSize(Size(getContentSize().width,DIS(16)));
//    titleBackground->setPosition(-(face->getContentSize().width/2),0);
//    face->addChild(titleBackground);
    
    // set name label
    title = Label::createWithTTF( uihelper::font_config_small, c.shortname );
    title->setTextColor(Color4B(_clrWhite));
    title->enableOutline(Color4B(_clrBlack),RES(1));
    //title->setHeight(DIS(16));
    title->getFontAtlas()->setAntiAliasTexParameters();
    //title->setWidth(getContentSize().width);
    title->setHorizontalAlignment(TextHAlignment::CENTER);
    title->setVerticalAlignment(TextVAlignment::BOTTOM);
    title->setAnchorPoint(uihelper::AnchorCenter);
    title->setPosition(face->getContentSize().width/2,0);
    face->addChild(title);
    
    buttonNode = face;
    
    auto userdata = static_cast<char_data_t*>(getUserData());
    userdata->select_loc = PointZero;
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
        buttonNode->setScale(1.0f);
        buttonNode->setOpacity(ALPHA(1.0f));
    }
}

void uigroupedlord::onPressStateChangedToPressed()
{
    if ( buttonNode ) {
        buttonNode->setScale(0.75f);
        buttonNode->setOpacity(ALPHA(1.0f));
    }
}


bool uigroupedlord::hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const
{
    return Widget::hitTest(pt, camera, p)
        && MouseOverHotspot(pt, MOUSE_OVER_HINT_DROP) == MOUSE_OVER_FACE ;
}
