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
    void clearFollowers();
    
protected:
    uigrouplord();
    void addFollower( int pos, mxid id );
    
    Vec2 calcCirclePos ( f32 pos );
    virtual void setPage(s32 page) override;
    virtual UIMOUSEOVER MouseOverHotspot( Vec2 pos, UIMOUSEOVERHINT hint ) const override;
    virtual bool hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const override;
    virtual void refreshStatus() override;

private:
    s32 follower_adjust;
    Button* i_group_left;
    Button* i_group_right;

    
public:
    Vector<Node*> followers;
    bool possibleSwap;
};

#define GROUPED_LORD_SCALE 0.75f 
