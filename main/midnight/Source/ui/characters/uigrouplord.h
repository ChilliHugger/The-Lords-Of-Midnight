//
//  uigrouplord.h
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#pragma once
#include "../uielement.h"
#include "uisinglelord.h"
#include "uigroupedlord.h"

class uigrouplord : public uisinglelord
{
    using Button = ax::ui::Button;
    using Camera = ax::Camera;
    using Vec2 = ax::Vec2;
    using Vec3 = ax::Vec3;
    template<class T> using Vector = ax::Vector<T>;

public:
    CREATE_FUNC(uigrouplord);
    virtual bool init() override;
    virtual bool setLord( mxid characterid ) override;
    static uigrouplord* createWithLord ( mxid characterid );
    
    void createFollowers();
    void createFollowers(const c_mxid& followers);
    void updateFollowers();
    void clearFollowers();
    
protected:
    uigrouplord();
    void addFollower( int pos, mxid id );
    
    Vec2 calcCirclePos ( f32 pos );
    virtual void setPage(page_t page) override;
    virtual UIMOUSEOVER MouseOverHotspot( Vec2 pos, UIMOUSEOVERHINT hint ) const override;
    virtual bool hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const override;
    virtual void refreshStatus() override;

//    void rotateEveryoneLeft();
//    void rotateEveryoneRight();
    
private:
    s32 follower_adjust;
    Button* i_group_left;
    Button* i_group_right;

    
public:
    Vector<uigroupedlord*> followers;
    bool possibleSwap;
};

static const s32 MAX_VISIBLE_LORDS = 8;
static const s32 GROUP_RADIUS = 95;
static const f32 GROUP_STEPS = 10.0;
static const f32 LEFT_BUTTON_POSITION = 7.9f;
static const f32 RIGHT_BUTTON_POSITION = 9.1f;
static const f32 DISBAND_BUTTON_POSITION = 8.5f;
static const f32 GROUPED_LORD_SCALE = scale_3qtr;

#define TO_PARENT_PAGE(x)   (x*-1)
#define FROM_PARENT_PAGE(x) (abs(x))
#define IS_PARENT_PAGE(x)   (x<InvalidPage)
#define TO_PAGE_INDEX(x)    ((s32)x-1)
#define FROM_PAGE_INDEX(x)    ((page_t)x+1)
