#ifndef __LANDSCAPE_SCENE_H__
#define __LANDSCAPE_SCENE_H__

#include "cocos2d.h"

#include "landscaping/LandscapeGenerator.h"
#include "landscaping/LandscapeView.h"

//#include "tme_interface.h"



USING_NS_CC;

typedef enum GESTURE_MODE {
    none=0,
    horizontal=1,
    vertical=2,
} GESTURE_MODE ;



typedef enum SNAP_MODE {
    sm_back_right=1,
    sm_forward_right=3,
} SNAP_MODE ;

enum LANDSCAPE_MOVEMENT
{
    LM_NONE=0,
    LM_ROTATE_LEFT=1,
    LM_ROTATE_RIGHT=2,
    LM_MOVE_FORWARD=3,
    LM_MOVE_BACKWARD=4,
#if defined(_DDR_)
    LM_MOVE_FORWARD_LEAVE_TUNNEL=5,
#endif
    LM_DRAG_START=10,
    LM_SHOW_COMPASS=11,
    LM_HIDE_COMPASS=12,
};


class Landscape : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(Landscape);

    
    Sprite* GetTerrainImage( mxterrain_t terrain );
    Sprite* GetFloorImage( floor_t floor );
    

    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchCancelled(Touch* touch, Event* event);

    void update(float delta);
    
    void Moving();
    void Rotating();
    
    bool StartMoving();
    void StopMoving();
    
    bool StartLookRight ( void );
    bool StartLookLeft ( void );
    void StopRotating(LANDSCAPE_MOVEMENT type);

    void SetViewForCurrentCharacter ( void );
    
    void BuildFloors( LandscapeItems* items );
    void BuildTerrain( LandscapeItems* items );
    void BuildDebug( LandscapeItems* items );
    f32 NormaliseXPosition(f32 x);
    
    void UpdateLandscape();
    void InitKeyboard();
    
    
private:
    GLProgramState* glProgramState;
    GLProgram*      glShaderProgram;

    
public:
    LandscapeView*      landscapeView;
    LandscapeOptions    options;
    bool                isLooking;
    bool                isMoving;
    
    
};

#endif // __LANDSCAPE_SCENE_H__
