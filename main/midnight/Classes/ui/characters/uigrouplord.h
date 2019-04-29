//
//  uigrouplord.h
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../uielement.h"
#include "uisinglelord.h"

class uigrouplord : public uisinglelord
{
public:
    CREATE_FUNC(uigrouplord);
    virtual bool init() override;
    virtual bool setLord( mxid characterid ) override;
    static uigrouplord* createWithLord ( mxid characterid );
    
    void createFollowers();
    void updateFollowers();
    
protected:
    uigrouplord();
    void addFollower( int pos, mxid id );
    
    cocos2d::Vec2 calcCirclePos ( f32 pos );
    virtual bool hitTest(const cocos2d::Vec2 &pt, const cocos2d::Camera* camera, cocos2d::Vec3 *p) const override;

private:
    cocos2d::Vector<Node*> followers;
    s32 follower_adjust;
    
    cocos2d::ui::Button* i_group_left;
    cocos2d::ui::Button* i_group_right;
};
