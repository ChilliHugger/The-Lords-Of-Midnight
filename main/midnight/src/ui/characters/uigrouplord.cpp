//
//  uigrouplord.cpp
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//
#include "../uihelper.h"
#include "uigrouplord.h"
#include "../../tme/tme_interface.h"
#include "../../system/tmemanager.h"
#include "../../system/resolutionmanager.h"
#include "../../system/moonring.h"
#include "../../frontend/layout_id.h"

USING_NS_CC;

uigrouplord::uigrouplord() :
    i_group_left(nullptr),
    i_group_right(nullptr),
    follower_adjust(0),
    possibleSwap(false)
{
    lordtype = lordwidget::group;
}

bool uigrouplord::init()
{
    if ( uisinglelord::init() ) {
        
        auto circle = Sprite::createWithSpriteFrameName("circle_selector");
        setContentSize(circle->getContentSize());
        circle->setAnchorPoint(uihelper::AnchorCenter);
        circle->setIgnoreAnchorPointForPosition(true);
        circle->setLocalZOrder(ZORDER_FAR);
        addChild(circle);
        
//        auto box = DrawNode::create();
//        box->drawRect(Vec2::ZERO, Vec2(getContentSize().width,getContentSize().height), Color4F(_clrBlack) );
//        addChild(box);
        //        Vec2 pos2 = Vec2(getContentSize().width/4,getContentSize().height/4);
        //        bg->setPosition(pos2);

        
        Vec2 pos = Vec2(getContentSize().width/2,getContentSize().height/2);
        innerCircle->setPosition(pos);
        selectedNode->setPosition(pos);
        locationNode->setPosition(pos);
        
        // Left button
        i_group_left = uihelper::CreateImageButton("i_group_left", ID_GROUP_LEFT, [&] (Ref* ref ) {
            f32 min = ( MAX_VISIBLE_LORDS - followers.size()) ;
            if ( follower_adjust > min ) {
                follower_adjust-=1;
                updateFollowers();
            }
        });
        
        i_group_left->setPosition( calcCirclePos(LEFT_BUTTON_POSITION) );
        addChild(i_group_left);

        // Right Button
        i_group_right = uihelper::CreateImageButton("i_group_right", ID_GROUP_RIGHT, [&] (Ref* ref ) {
            if ( follower_adjust<0 ) {
                follower_adjust+=1;
                updateFollowers();
            }
        });
        i_group_right->setPosition( calcCirclePos(RIGHT_BUTTON_POSITION) );
        addChild(i_group_right);
        
        // Disband
        auto disband = uihelper::CreateImageButton("i_group_disband", ID_GROUP_DISBAND, [&](Ref* ref) {
            _clickEventListener(ref);
        });
        
        disband->setPosition( calcCirclePos(DISBAND_BUTTON_POSITION) );
        addChild(disband);
        
        return true;
    }
    
    return false;
}

//void uigrouplord::rotateEveryoneLeft() {
//
//
//
//    auto actionfloat = ActionFloat::create(0.25f, 0.0f, 1.0f, [=](float value) {
//        s32 index=0;
//        for ( auto lord : followers ) {
//            f32 pos = index+follower_adjust+value;
//            lord->setPosition( calcCirclePos( pos ) );
//            index++;
//        }
//    });
//
//    runAction(Sequence::createWithTwoActions( EaseSineInOut::create(actionfloat),
//                                             CallFunc::create( [=] { follower_adjust+=1; updateFollowers(); } )
//                                             ));
//
//
//}
//
//void uigrouplord::rotateEveryoneRight() {
//
//    auto actionfloat = ActionFloat::create(0.25f, 1.0f, 0.0f, [=](float value) {
//        s32 index=0;
//        for ( auto lord : followers ) {
//            f32 pos = index+follower_adjust+value;
//            lord->setPosition( calcCirclePos( pos ) );
//            index++;
//        }
//    });
//
//    runAction(Sequence::createWithTwoActions( EaseSineInOut::create(actionfloat),
//                                             CallFunc::create( [=] { follower_adjust-=1; updateFollowers(); } )
//                                             ));
//
//}


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
    createFollowers(followers);
}

void uigrouplord::createFollowers(const c_mxid& followers)
{
    for ( int ii=0; ii<followers.Count(); ii++) {
        addFollower(ii,followers[ii]);
    }
    follower_adjust=0;
    updateFollowers();
}

void uigrouplord::updateFollowers()
{
    f32 min = ( MAX_VISIBLE_LORDS - followers.size()) ;
    IF_NOT_NULL( i_group_left )->setVisible(follower_adjust > min);
    IF_NOT_NULL( i_group_right )->setVisible(follower_adjust<0);
    
    s32 index=0;
    for ( auto lord : followers ) {
        s32 pos = index+follower_adjust;
        lord->setPosition( calcCirclePos( pos ) );
        lord->setVisible( pos>=0 && pos<MAX_VISIBLE_LORDS );
        
        TextHAlignment align =  ( pos < 3 || pos == MAX_VISIBLE_LORDS-1 ) ? TextHAlignment::LEFT : TextHAlignment::RIGHT;
        lord->setTitleAlignment(align);
        index++;
    }
    
}

void uigrouplord::clearFollowers()
{
    for ( auto lord : followers ) {
        lord->removeFromParent();
    }
    followers.clear();
}

void uigrouplord::addFollower( int pos, mxid id )
{
    auto lord = uigroupedlord::createWithLord(id);
    layoutid_t tag = (layoutid_t) (ID_SELECT_CHAR+id);
    lord->setPosition( calcCirclePos(pos) );
    lord->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    lord->setTag(tag);
    lord->addClickEventListener( _clickEventListener );
    lord->drag_delegate = drag_delegate;
    lord->setPage( getPage()*-1 );
    lord->setScale(GROUPED_LORD_SCALE);
    addChild(lord);
    followers.pushBack(lord);
    
    if ( drag_delegate == nullptr ) {
        lord->disableDrag();
    }
    
}

Vec2 uigrouplord::calcCirclePos ( f32 pos )
{
    f32 step = 360 / GROUP_STEPS ;
    f32 a = step*pos;
    f32 r = RES(GROUP_RADIUS);
    
    auto size = getContentSize();
    auto center = Vec2( HALF(size.width), HALF(size.height));
    f32 x = center.x + r * cos(DEGREETORAD(a));
    f32 y = center.y - r * sin(DEGREETORAD(a));
    
    return Vec2(x,y);
}

void uigrouplord::setPage(page_t page)
{
    uilordselect::setPage(page);
    for ( auto lord : followers ) {
        lord->setPage( TO_PARENT_PAGE(page) );
    }
}

bool uigrouplord::hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const
{
    return cocos2d::ui::Widget::hitTest(pt, camera, p)
        && MouseOverHotspot(pt, MOUSE_OVER_HINT_DROP) == MOUSE_OVER_FACE ;
}

UIMOUSEOVER uigrouplord::MouseOverHotspot( Vec2 pos, UIMOUSEOVERHINT hint ) const
{
    if ( hint == MOUSE_OVER_HINT_DROP ) {

        f32 scale = getScale();
        
        f32 distance = getDistanceFromCentre(pos);
        
        if ( distance < (DISTANCE_SHIELD * scale) )
            return MOUSE_OVER_FACE ; // face
        
        if ( distance < (DISTANCE_INNER * scale) )
            return MOUSE_OVER_INNER ; // inner

        if ( distance < (DISTANCE_OUTER * scale) )
            return MOUSE_OVER_OUTER ; // outer
        
    }
    return MOUSE_OVER_NONE ;
}


void uigrouplord::refreshStatus()
{
    uisinglelord::refreshStatus();

    buttonNode->setOpacity( possibleSwap ? ALPHA(alpha_1qtr) : ALPHA(alpha_normal) );
    
}
