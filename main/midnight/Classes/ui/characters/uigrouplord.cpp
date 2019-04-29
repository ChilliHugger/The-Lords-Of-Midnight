//
//  uigrouplord.cpp
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "uigrouplord.h"
#include "uigroupedlord.h"
#include "../uihelper.h"
#include "../../tme_interface.h"
#include "../../system/tmemanager.h"
#include "../../system/resolutionmanager.h"
#include "../../system/moonring.h"
#include "../../frontend/layout_id.h"

USING_NS_CC;
using namespace cocos2d::ui;

const int MAX_VISIBLE_LORDS = 8;

uigrouplord::uigrouplord() :
    i_group_left(nullptr),
    i_group_right(nullptr),
    follower_adjust(0)
{
    lordtype = lordtype_group;
}

bool uigrouplord::init()
{
    if ( uisinglelord::init() ) {
        
        auto circle = Sprite::createWithSpriteFrameName("circle_selector");
        setContentSize(circle->getContentSize());
        circle->setAnchorPoint(uihelper::AnchorCenter);
        circle->setIgnoreAnchorPointForPosition(true);
        addChild(circle);
        
//        auto box = DrawNode::create();
//        box->drawRect(Vec2(0,0), Vec2(getContentSize().width,getContentSize().height), Color4F(_clrBlack) );
//        addChild(box);
        //        Vec2 pos2 = Vec2(getContentSize().width/4,getContentSize().height/4);
        //        bg->setPosition(pos2);

        
        Vec2 pos = Vec2(getContentSize().width/2,getContentSize().height/2);
        innerCircle->setPosition(pos);
        selectedNode->setPosition(pos);
        locationNode->setPosition(pos);
        
        // i_group_disband  calcCirclePos(8.5f)
        // i_group_left     calcCirclePos(7.9f)
        // i_group_right    calcCirclePos(9.1f)

        // Left button
        i_group_left = uihelper::CreateImageButton("i_group_left", ID_GROUP_LEFT, [&] (Ref* ref ) {
            f32 min = ( MAX_VISIBLE_LORDS - followers.size()) ;
            if ( follower_adjust > min ) {
                follower_adjust-=1;
                updateFollowers();
            }
        });
        
        i_group_left->setPosition( calcCirclePos(7.9f) );
        addChild(i_group_left);

        // Right Button
        i_group_right = uihelper::CreateImageButton("i_group_right", ID_GROUP_RIGHT, [&] (Ref* ref ) {
            if ( follower_adjust<0 ) {
                follower_adjust+=1;
                updateFollowers();
            }
        });
        i_group_right->setPosition( calcCirclePos(9.1f) );
        addChild(i_group_right);
        
        // Disband
        auto disband = uihelper::CreateImageButton("i_group_disband", ID_GROUP_DISBAND, [&](Ref* ref) {
            _clickEventListener(ref);
        });
        
        disband->setPosition( calcCirclePos(8.5f) );
        addChild(disband);
        
        
        return true;
    }
    
    return false;
}

uigrouplord* uigrouplord::createWithLord ( mxid characterid )
{
    auto select = uigrouplord::create();
    select->setLord( characterid );
    return select;
}

bool uigrouplord::setLord(mxid characterid)
{
    if ( uisinglelord::setLord(characterid) ) {
        return true;
    }
    return false;
}

void uigrouplord::createFollowers()
{
    c_mxid followers;
    TME_GetFollowers(characterId, followers);

    for ( int ii=0; ii<followers.Count(); ii++) {
        addFollower(ii,followers[ii]);
    }

//    for ( int ii=0; ii<16; ii++ )
//        addFollower(ii,followers[0]+ii);
    
    follower_adjust=0;
    updateFollowers();
}

void uigrouplord::updateFollowers()
{
    f32 min = ( MAX_VISIBLE_LORDS - followers.size()) ;
    IF_NOT_NULL( i_group_left )->setEnabled(follower_adjust > min);
    IF_NOT_NULL( i_group_right )->setEnabled(follower_adjust<0);
    
    s32 index=0;
    for ( auto lord : followers ) {
        s32 pos = index+follower_adjust;
        lord->setPosition( calcCirclePos( pos ) );
        lord->setVisible( pos>=0 && pos<MAX_VISIBLE_LORDS );
        
        TextHAlignment align =  ( pos < 3 || pos == MAX_VISIBLE_LORDS-1 ) ? TextHAlignment::LEFT : TextHAlignment::RIGHT;
        static_cast<uigroupedlord*>(lord)->setTitleAlignment(align);
        index++;
    }
    
}

void uigrouplord::addFollower( int pos, mxid id )
{
    auto lord = uigroupedlord::createWithLord(id);
    layoutid_t tag = (layoutid_t) (ID_SELECT_CHAR+id);
    lord->setAnchorPoint(uihelper::AnchorCenter);
    lord->setPosition( calcCirclePos(pos) );
    lord->setTag(tag);
    lord->addClickEventListener( _clickEventListener );
    lord->drag_delegate = drag_delegate;
    addChild(lord);
    followers.pushBack(lord);
}

Vec2 uigrouplord::calcCirclePos ( f32 pos )
{
    f32 steps= 10 ;
    f32 step = 360 / steps ;
    f32 a = step*pos;
    f32 r = RES(95);
    
    auto size = getContentSize();
    auto center = Vec2(size.width/2, size.height/2);
    f32 x = center.x + r * cos(DEGREETORAD(a));
    f32 y = center.y - r * sin(DEGREETORAD(a));
    
    return Vec2(x,y);
}

bool uigrouplord::hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const
{
    return MouseOverHotspot(pt, MOUSE_OVER_HINT_DROP) == MOUSE_OVER_FACE ;
}

