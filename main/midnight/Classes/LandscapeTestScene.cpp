

#include "LandscapeTestScene.h"
#include "landscaping/LandscapeSky.h"
#include "landscaping/LandscapeLand.h"
#include "landscaping/LandscapeTerrain.h"

#include "TMEMapBuilder.h"

USING_NS_CC;
using namespace tme;

#define _DEBUG_LANDSCAPE_
//#define _CITADEL_MAP_
#define _LOM_MAP_


Scene* Landscape::createScene()
{
    return Landscape::create();
}


bool Landscape::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(Landscape::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    
    // load terrain
    
#if defined(_LOM_MAP_)
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("terrain-0.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("terrain-1.plist");
#endif
    
#if defined(_CITADEL_MAP_)
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("terrain/citadel_M-0.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("terrain/citadel_M-1.plist");
#endif
    
    
    // load shader
    auto p1 = GLProgram::createWithFilenames("terrain/standard.vsh", "terrain/dayNight.fsh");
    glProgramState = GLProgramState::getOrCreateWithGLProgram( p1 );
    glProgramState->setUniformVec4("p_left", Vec4(0,0,165.0/255.0,1));      // outline
    glProgramState->setUniformVec4("p_right", Vec4(1,1,1,1));               // body
    glProgramState->setUniformFloat("p_alpha", 1.0f);               // alpha

    
    // initialise TME
    if ( !TME_Init() )
        return false;
    
//#ifdef _TME_CHEAT_MODE_
//    UIDEBUG("Global:: _TME_CHEAT_MODE_");
    variables::sv_cheat_armies_noblock = true;
    variables::sv_cheat_nasties_noblock = true;
    variables::sv_cheat_movement_free = true ;
    //variables::sv_cheat_movement_cheap = true ;
    //variables::sv_cheat_commands_free = true ;
//#endif
    


#if defined(_LOM_MAP_)
    auto builder = new TMEMapBuilder();
    mxmap* map = builder->Build( "lom_map.tmx" );
    TME_DebugInstallMap(map);
#endif

#if defined(_CITADEL_MAP_)
    auto builder = new TMEMapBuilderCitadel();
    mxmap* map = builder->Build( "citadel_wip.tmx" );
    TME_DebugInstallMap(map);
#endif
    
    
    
    options.generator = new LandscapeGenerator();
    
    InitKeyboard();
    
    scheduleUpdate();
    
    options.showWater = false;
    options.showLand  = false;
    options.showTerrain = true ;
    options.debugMode = 0;
    options.landScaleX = 1.6f;
    options.landScaleY = 2.15f;
    options.debugLand=false;

    options.isMoving = false;
    options.isLooking = false;
    
    character&	c = TME_CurrentCharacter();
    //Character_Place( c, loc_t(2,110) );
    Character_Place( c, loc_t(15,58) );
    
    
    Character_Look(c, DR_NORTH);

    
    SetViewForCurrentCharacter();
    
    return true;
}

void Landscape::InitKeyboard()
{
    
    auto eventListener = EventListenerKeyboard::create();
    
    //auto me = this;
    
    eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event){

        if ( options.isMoving || options.isLooking )
            return;
        
        Vec2 loc = event->getCurrentTarget()->getPosition();
        switch(keyCode){
                
            case cocos2d::EventKeyboard::KeyCode::KEY_F2:
                options.showLand = !options.showLand;
                UpdateLandscape();
                break;
                
            case cocos2d::EventKeyboard::KeyCode::KEY_F3:
                options.showWater = !options.showWater;
                UpdateLandscape();
                break;

            case cocos2d::EventKeyboard::KeyCode::KEY_F4:
                options.showTerrain = !options.showTerrain;
                UpdateLandscape();
                break;

            case cocos2d::EventKeyboard::KeyCode::KEY_F5:
                options.debugLand = !options.debugLand;
                UpdateLandscape();
                break;
                
            case cocos2d::EventKeyboard::KeyCode::KEY_F1:
                options.debugMode += 1;
                if ( options.debugMode > 4 )
                    options.debugMode = 0;
                UpdateLandscape();
                break;
                
            case cocos2d::EventKeyboard::KeyCode::KEY_O:
                options.landScaleX += 0.01f;
                UpdateLandscape();
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_P:
                options.landScaleX -= 0.01f;
                UpdateLandscape();
                break;

            case cocos2d::EventKeyboard::KeyCode::KEY_Q:
                options.landScaleY += 0.01f;
                UpdateLandscape();
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_A:
                options.landScaleY -= 0.01f;
                UpdateLandscape();
                break;
                
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


void Landscape::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();



}

void Landscape::UpdateLandscape()
{
    if ( landscapeView ) {
        this->removeChild(landscapeView);
        landscapeView->release();
        landscapeView=nullptr;
    }
    
    
    options.generator->horizontalOffset = options.lookAmount;

    
    landscapeView = new LandscapeView();
    landscapeView->programState = glProgramState;
    landscapeView->Init(&options);
    landscapeView->setAnchorPoint(Vec2(0.5,0.5));
    landscapeView->setPosition( Vec2(getContentSize().width/2, getContentSize().height/2));
    
    this->addChild(landscapeView);
    
    
}

void Landscape::update(float delta)
{
    Scene::update(delta);
    
}

void Landscape::SetViewForCurrentCharacter ( void )
{
    character&	c = TME_CurrentCharacter();
    
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


bool Landscape::StartLookLeft ( void )
{
    character&	c = TME_CurrentCharacter();
    Character_LookLeft ( c );
    
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

bool Landscape::StartLookRight ( void )
{
    character&	c = TME_CurrentCharacter();
    Character_LookRight ( c );
    
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

bool Landscape::StartMoving()
{
    character&	c = TME_CurrentCharacter();
    
    mxid			key;
    maplocation		map;
    key = MAKE_LOCID(location_infront.x, location_infront.y);
    TME_GetLocation( map, key );
    
//    bMoveLocationHasArmy=FALSE;
//    if ( map.flags&lf_army )
//        bMoveLocationHasArmy =  TRUE;
    
    
    loc_t moveFrom = c.location;
    if ( MXSUCCESS(Character_Move(c)) ) {
        
        loc_t moveTo = c.location;
        f32 target = DIR_STEPS ;
        
        options.isMoving = true;
        
        auto actionfloat = ActionFloat::create(0.5, 0, target, [=](float value) {

            options.here.x = moveFrom.x*(DIR_STEPS - value) + moveTo.x*value;
            options.here.y = moveFrom.y*(DIR_STEPS - value) + moveTo.y*value;
            
            if ( value >= target )
                StopMoving();
            
            options.movementAmount = (f32)(value-1) / (f32)DIR_STEPS ;
            
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

void Landscape::StopRotating(LANDSCAPE_MOVEMENT type)
{
    options.isLooking = false;
    
    SetViewForCurrentCharacter();
    
    //MovementNotification(type);
    
    UpdateLandscape();
    
}

void Landscape::StopMoving()
{
    options.isMoving = false;
    
    SetViewForCurrentCharacter();
    
    //MovementNotification(LM_MOVE_FORWARD);
    
    UpdateLandscape();
    
}


