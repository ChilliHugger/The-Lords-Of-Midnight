#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "panel_look.h"
#include "../system/moonring.h"

#include "../ui/uihelper.h"

#include "../landscaping/LandscapeSky.h"
#include "../landscaping/LandscapeLand.h"
#include "../landscaping/LandscapeTerrain.h"

#include "TMEMapBuilder.h"

USING_NS_CC;
using namespace tme;

bool panel_look::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    options.generator = new LandscapeGenerator();
    options.colour = new LandscapeColour();
    options.showWater = false;
    options.showLand  = false;
    options.showTerrain = true ;
    options.debugMode = 0;
    options.landScaleX = 1.6f;
    options.landScaleY = 2.15f;
    options.debugLand=false;
    options.isMoving=false;
    options.isLooking=false;
    
    options.colour->options = &options;

    InitKeyboard();
    
    SetViewForCurrentCharacter();
    
    return true;
}

void panel_look::SetViewForCurrentCharacter ( void )
{
    character&    c = TME_CurrentCharacter();
    
    options.colour->SetLookColour(c.time);
    
    options.here = c.location;
    options.here.x *= DIR_STEPS;
    options.here.y *= DIR_STEPS;
    
    // looking = (c.looking * DIR_AMOUNT) ;
    options.lookAmount = c.looking * 400;
    if ( options.lookAmount >= 3200 )
        options.lookAmount-=3200;
    
    options.currentLocation = c.location;
    options.currentDirection = c.looking;
    
    options.aheadLocation.x = options.currentLocation.x + mxgridref::DirectionLookTable[options.currentDirection*2];
    options.aheadLocation.y = options.currentLocation.y + mxgridref::DirectionLookTable[(options.currentDirection*2)+1];
    
    options.generator->Build(options.here, 0);
    
    UpdateLandscape();
    
}

void panel_look::UpdateLandscape()
{
    if ( landscapeView ) {
        removeChild(landscapeView);
        landscapeView->release();
        landscapeView=nullptr;
    }
    
    options.generator->horizontalOffset = options.lookAmount;
    
    landscapeView = new LandscapeView();
    landscapeView->programState = mr->glProgramState;
    landscapeView->Init(&options);
    landscapeView->setAnchorPoint(Vec2(0.5,0.5));
    landscapeView->setPosition( Vec2(getContentSize().width/2, getContentSize().height/2));
    
    addChild(landscapeView);
    
}

void panel_look::InitKeyboard()
{
    auto eventListener = EventListenerKeyboard::create();
    
    eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event){
        
        if ( options.isMoving || options.isLooking )
            return;
        
        Vec2 loc = event->getCurrentTarget()->getPosition();
        switch(keyCode){
                
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                StartLookLeft();
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                StartLookRight();
                break;
            case EventKeyboard::KeyCode::KEY_UP_ARROW:
                StartMoving();
                break;
            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
                break;
                
            default:
                break;
        }
    };
    
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener,this);
}

bool panel_look::StartLookLeft ( void )
{
    character&    c = TME_CurrentCharacter();
    Character_LookLeft ( c );
    
    options.colour->SetLookColour(c.time);
    
    f32 originalLooking = options.lookAmount;
    f32 target = -400 ;
    
    options.isLooking = true;
    
    auto actionfloat = ActionFloat::create(0.5, 0, target, [=](float value) {
        options.lookAmount = originalLooking + value;
        if ( value <= target) {
            StopRotating(LM_ROTATE_LEFT);
        }
        
        UpdateLandscape();
    });
    
    auto ease = new EaseSineInOut();
    ease->initWithAction(actionfloat);
    this->runAction(ease);
    
    return TRUE;
}

bool panel_look::StartLookRight ( void )
{
    character&    c = TME_CurrentCharacter();
    Character_LookRight ( c );
    
    options.colour->SetLookColour(c.time);
    
    f32 originalLooking = options.lookAmount;
    f32 target = 400 ;
    
    options.isLooking = true;
    
    auto actionfloat = ActionFloat::create(0.5, 0, target, [=](float value) {
        options.lookAmount = originalLooking + value;
        
        if ( value >= target) {
            StopRotating(LM_ROTATE_RIGHT);
        }
        
        UpdateLandscape();
    });
    
    auto ease = new EaseSineInOut();
    ease->initWithAction(actionfloat);
    this->runAction(ease);
    
    return TRUE;
    
}

bool panel_look::StartMoving()
{
    character&    c = TME_CurrentCharacter();
    
    mxtime_t startTime = c.time;
    
    
    mxid            key;
    maplocation        map;
    key = MAKE_LOCID(location_infront.x, location_infront.y);
    TME_GetLocation( map, key );
    
    //    bMoveLocationHasArmy=FALSE;
    //    if ( map.flags&lf_army )
    //        bMoveLocationHasArmy =  TRUE;
    
    
    loc_t moveFrom = c.location;
    if ( MXSUCCESS(Character_Move(c)) ) {
        
        options.colour->SetMovementColour(startTime,c.time);
        
        
        loc_t moveTo = c.location;
        f32 target = DIR_STEPS ;
        
        options.isMoving = true;
   
        auto actionfloat = ActionFloat::create(0.5, 0, target, [=](float value) {
            
            options.here.x = moveFrom.x*(DIR_STEPS - value) + moveTo.x*value;
            options.here.y = moveFrom.y*(DIR_STEPS - value) + moveTo.y*value;
            
            if ( value >= target )
                StopMoving();
        
            f32 result = value / target ;
            options.movementAmount = result;
            
            options.generator->Build(options.here, 0);
            UpdateLandscape();
            
            
        });
        
        auto ease = new EaseSineInOut();
        ease->initWithAction(actionfloat);
        this->runAction(ease);
        
        
        return true;
    }
    
    return false;
    
}

void panel_look::StopRotating(LANDSCAPE_MOVEMENT type)
{
    options.isLooking = false;
    
    SetViewForCurrentCharacter();
    
    //MovementNotification(type);
    
    UpdateLandscape();
    
}

void panel_look::StopMoving()
{
    options.isMoving = false;
    
    SetViewForCurrentCharacter();
    
    //MovementNotification(LM_MOVE_FORWARD);
    
    UpdateLandscape();
    
}



