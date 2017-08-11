
#include "TMEMapBuilder.h"
#include "LandscapeScene2.h"
#include "LandscapeGenerator.h"


USING_NS_CC;



const std::string terrain_graphics[] = {
    ""
    , "t_citadel0"
    , "t_forest0"
    , "t_henge0"
    , "t_tower0"
    , "t_village0"
    , "t_downs0"
    , "t_keep0"
    , "t_snowhall0"
    , "t_lake0"
    , "t_frozenwaste0"
    , "t_ruin0"
    , "t_lith0"
    , "t_cavern0"
    , "t_mountain0"
};




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
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("terrain/lom_L-0.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("terrain/lom_L-1.plist");

    
    // load shader
    auto p1 = GLProgram::createWithFilenames("terrain/standard.vsh", "terrain/dayNight.fsh");
    glProgramState = GLProgramState::getOrCreateWithGLProgram( p1 );
    glProgramState->setUniformVec4("p_left", Vec4(0,0,165.0/255.0,1));      // outline
    glProgramState->setUniformVec4("p_right", Vec4(1,1,1,1));               // body

    
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
    
    
    //auto builder = new TMEMapBuilder();
    //mxmap* map = builder->Build( "lom_map_test.tmx" );
    //TME_DebugInstallMap(map);
    
    
    landscapeGen = new LandscapeGenerator();
    
    
    SetViewForCurrentCharacter();
    
    
    
    
    //auto touchListener = EventListenerTouchOneByOne::create();
    
    //touchListener->onTouchBegan = CC_CALLBACK_2(Landscape::onTouchBegan, this);
    //touchListener->onTouchEnded = CC_CALLBACK_2(Landscape::onTouchEnded, this);
    //touchListener->onTouchMoved = CC_CALLBACK_2(Landscape::onTouchMoved, this);
    //touchListener->onTouchCancelled = CC_CALLBACK_2(Landscape::onTouchCancelled, this);
    
    //_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    
    InitKeyboard();
    
    scheduleUpdate();
    
    
    right = DIR_LEFT ;
    left = DIR_RIGHT;
    movement_amount=0;
    
    return true;
}

void Landscape::InitKeyboard()
{
    auto eventListener = EventListenerKeyboard::create();

    auto me = this;
    
    eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event){
        
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
    
    if ( current_landscape_node ) {
        this->removeChild(current_landscape_node);
        current_landscape_node->release();
        current_landscape_node=nullptr;
    }
    
    current_landscape_node = new Node();
    
    float scalex = 1024.0 / 128.0 ;
    
    auto floor = Sprite::createWithSpriteFrameName( "floor" );
    floor->setScale(scalex,0.5);
    floor->setPosition(0, 0);
    floor->setAnchorPoint( Vec2(0,0) );
    current_landscape_node->addChild(floor);
    
    auto sky = Sprite::createWithSpriteFrameName( "sky_day" );
    sky->setScale(scalex,0.5);
    sky->setPosition(0,floor->getContentSize().height*0.5);
    sky->setAnchorPoint( Vec2(0,0) );
    current_landscape_node->addChild(sky);

    
    landscapeGen->Build( here, looking );
    
    
//    Sprite* image;
//        image = Sprite::create("terrain/t_land0.png");
//    image->setScale(1.5);
//    image->setPosition(this->getContentSize().width/2, 0);
//    current_landscape_node->addChild(image);
    
    
    
    
    this->addChild(current_landscape_node);
    
}

Sprite* Landscape::GetTerrainImage( mxterrain_t terrain )
{
    if ( terrain == TN_PLAINS || terrain == TN_PLAINS2 || terrain == TN_PLAINS3 )
        return nullptr;
    
    
    auto image = Sprite::createWithSpriteFrameName( terrain_graphics[terrain] );
    
    image->setScale(0.5);
    image->setBlendFunc(cocos2d::BlendFunc::ALPHA_NON_PREMULTIPLIED);
    image->setGLProgramState( glProgramState );
    
    return image;
}

void Landscape::update(float delta)
{
    Scene::update(delta);
    
    f32 whole = 1024 ;
    f32 half = whole / 2 ;
    
//    if ( ui->focus!=this && mouse_down ) {
//        mouse_down=FALSE;
//    }
    
//    if ( mouse_down ) {
//        if ( ui->TouchCount()>1 )
//            mouse_down=0;
//    }
    
    //
    // Should we show the compass
    //
//    if ( mouse_down ) {
//        if ( compass_delay && ui->TimerGetMs()-mouse_down_time > compass_delay ) {
//            mouse_down=FALSE;
//            MovementNotification(LM_SHOW_COMPASS);
//            return uielement::update();
//        }
//    }
//    
    
    Moving();
    
    Rotating();
    
    // update the PeopleInFront
    
    if ( snapback ) {
        
        //UPDATE_DISPLAY;
        
        f32 amount=movement_amount;
        
        int target=0;
        switch (snapback) {
                
            case 1:
            case 2:
                target=0;
                break;
                
            case 3:
            case 5:
                target=half;
                break;
                
            case 4:
            case 6:
                target=half*-1;
                break;
                
            case 7:
                target=whole;
                break;
                
            case 8:
                target=whole*-1;
                break;
                
            default:
                break;
        }
        
        if ( ABS(ABS(target)-ABS(dragged)) < 32 ) {
            amount/=4;
        }
        
        
        dragged += amount ;
        
        switch (snapback) {
                
            case 1:
            case 4:
            case 5:
            case 8:
                if ( dragged <= target ) {
                    dragged=0;
                    snapback=0;
                }
                break;
                
            case 2:
            case 3:
            case 6:
            case 7:
                if ( dragged >= target ) {
                    dragged=0;
                    snapback=0;
                }
                break;
                
            default:
                break;
        }
        
        if ( movement_amount > 0 ) {
            movement_amount *= 0.9f;
            if ( movement_amount <4)
                movement_amount=4;
        }
        else if ( movement_amount < 0 ) {
            movement_amount *= 0.9f;
            if ( movement_amount >-4)
                movement_amount=-4;
        }
        
        
        
        looking += amount;
        //ParallaxCharacters();
        
        
        if ( snapback == 0 ) {
            
            movement_amount = 0;
            
            character&	c = TME_CurrentCharacter();
            if ( looking<0 )
                looking+=4096;
            
            if ( looking>4096 )
                looking-=4096;
            
            mxdir_t dir = (mxdir_t) (int)((looking + (DIR_AMOUNT/2)) / (f32)DIR_AMOUNT) ;
            
            if ( c.looking != dir ) {
                // work out what direction we are now looking
                Character_Look( c, dir );
                
                
                //MovementNotification(LM_NONE);
            }
            
            //SetViewForCurrentCharacter();
            //Enable();
        }
        
        
    }
    
    UpdateLandscape();
    
}

void Landscape::Moving()
{
    if ( moveCount ) {
        here.x = moveFrom.x*(DIR_STEPS - moveCount) + moveTo.x*moveCount;
        here.y = moveFrom.y*(DIR_STEPS - moveCount) + moveTo.y*moveCount;
        moveCount = (moveCount + 1)&15;
        if ( moveCount == 0 )
            StopMoving();
        ;
    }
    
    
}

void Landscape::SetViewForCurrentCharacter ( void )
{
    character&	c = TME_CurrentCharacter();
    
    here = c.location;
    looking = c.looking * DIR_AMOUNT;
    //SetTimeOfDay(c.time);
    
    //current_info=info[0];
    //next_info=info[1];
    //prev_info=info[2];
    
    
#if defined(_DDR_)
    BOOL isInTunnel = Character_IsInTunnel(c) ;
#else
    BOOL isInTunnel = FALSE;
#endif
    
    
    
    //if ( current_info ) {
    //    current_info->Initialise(c);
    //    current_info->location.x=0;
    //}
    
    //if ( next_info ) {
    //    next_info->Initialise(c.location,NEXT_DIRECTION(c.looking), isInTunnel);
    //    next_info->location.x = Width();
    //}
    
    //if ( prev_info ) {
    //    prev_info->Initialise(c.location,PREV_DIRECTION(c.looking), isInTunnel);
    //    prev_info->location.x = -(s32)Width();
    //}
    
    
    UpdateLandscape();
    
}


bool Landscape::StartLookLeft ( void )
{
    rotate_look = 0; //512;
    turning = DIR_LEFT;					//	-45 degrees / 16
    mouse_down=false;
    
    character&	c = TME_CurrentCharacter();
    Character_LookLeft ( c );
    
    //startTint[0]  = CreateTimeBrightness(c.time,0);
    //endTint[0] = startTint[0];
    
#if defined(_DAY_NIGHT_SHADER_)
    //startTint[1]  = CreateTimeBrightness(c.time,1);
    //endTint[1] = startTint[1];
    //startTint[2]  = CreateTimeBrightness(c.time,2);
    //endTint[2] = startTint[2];
#endif
    
    //next_info->Initialise(c);
    //next_info->StartSlideFromLeft();
    //current_info->StartSlideOffRight();
    
    dragged=0;
    
    UpdateLandscape();
    
    return TRUE;
}

bool Landscape::StartLookRight ( void )
{
    rotate_look=0;
    turning = DIR_RIGHT;					//	45 degrees / 16
    mouse_down=false;
    
    character&	c = TME_CurrentCharacter();
    Character_LookRight ( c );
    
    //startTint[0]  = CreateTimeBrightness(c.time,0);
    //endTint[0] = startTint[0];
    
#if defined(_DAY_NIGHT_SHADER_)
    //startTint[1]  = CreateTimeBrightness(c.time,1);
    //endTint[1] = startTint[1];
    //startTint[2]  = CreateTimeBrightness(c.time,2);
    //endTint[2] = startTint[2];
#endif
    
    //next_info->Initialise(c);
    //next_info->StartSlideFromRight();
    //current_info->StartSlideOffLeft();
    
    dragged=0;
    
    UpdateLandscape();
    
    return TRUE;
    
}

bool Landscape::StartMoving()
{
    mouse_down=false;
    
    character&	c = TME_CurrentCharacter();
    
    // remember time of day for start colour
    
    //startTint[0] =  CreateTimeBrightness(c.time,0);
    
#if defined(_DAY_NIGHT_SHADER_)
    //startTint[1] =  CreateTimeBrightness(c.time,1);
    //startTint[2] =  CreateTimeBrightness(c.time,2);
#endif
    // check if the location we are going to move to
    // has an army
    
    mxid			key;
    maplocation		map;
    key = MAKE_LOCID(location_infront.x, location_infront.y);
    TME_GetLocation( map, key );
    
    bMoveLocationHasArmy=FALSE;
    if ( map.flags&lf_army )
        bMoveLocationHasArmy =  TRUE;
    
    
    
    moveFrom = c.location;
    if ( MXSUCCESS(Character_Move(c)) ) {
        
        moveTo = c.location;
        moveCount = 1;
        
        // get time of day for target colour
        //endTint[0] = CreateTimeBrightness(c.time,0);
#if defined(_DAY_NIGHT_SHADER_)
        //endTint[1] = CreateTimeBrightness(c.time,1);
        //endTint[2] = CreateTimeBrightness(c.time,2);
#endif
        //next_info->Initialise(c);
        //next_info->StartFadeIn();
        
        //current_info->StartFadeOut();
        
        UpdateLandscape();
        
        return true;
    }
    
    return false;
    
}

void Landscape::StopRotating(LANDSCAPE_MOVEMENT type)
{
    rotate_look=0;
    turning = 0;
    
    // swap info
    //PeopleInFront* temp = next_info;
    //next_info=current_info;
    //current_info=temp;
    
    //next_info->Clear();
    //current_info->StopAnim();
    //prev_info->Clear();
    
    SetViewForCurrentCharacter();
    
    //MovementNotification(type);
    
    UpdateLandscape();
    
}

void Landscape::StopMoving()
{
    
    // swap info
    //PeopleInFront* temp = next_info;
    //next_info=current_info;
    //current_info=temp;
    
    //next_info->Clear();
    //current_info->StopAnim();
    //prev_info->Clear();
    
    SetViewForCurrentCharacter();
    
    //MovementNotification(LM_MOVE_FORWARD);
    
    
    UpdateLandscape();
    
}



void Landscape::Rotating ( void )
{
    
    looking = looking+turning;

    
    if ( turning ) {
        
        rotate_look += fabsf(turning);
        
        if ( turning>0 )
            turning-=1;
        
        if ( turning<0)
            turning+=1;
        
        //f32 distance = rotate_look / (f32)DIR_AMOUNT;
        //if ( current_info )
        //    current_info->AdjustMovement( distance );
        //if (next_info)
        //    next_info->AdjustMovement( distance );
        UpdateLandscape();
    }
    
    
    if ( turning < 0 ) {
        if ( rotate_look >= DIR_AMOUNT ) {
            StopRotating(LM_ROTATE_LEFT);
        }
    }
    
    if ( turning > 0 ) {
        if ( rotate_look >= DIR_AMOUNT ) {
            StopRotating(LM_ROTATE_RIGHT);
        }
    }
    
    
    
}








bool Landscape::onTouchBegan(Touch* touch, Event* event)
{
    
    mouse_down=false;
    
    
    drag_start=touch->getLocation();
    drag_current=drag_start;
    
    dragged=0;
    
    // drag_start_time = event->time;
    
    //drag_touch_count = ui->TouchCount();
    
    //MovementNotification(LM_DRAG_START);
    

    
    
    return true;
}

void Landscape::onTouchEnded(Touch* touch, Event* event)
{
    cocos2d::log("touch ended");
    
    drag_stop=touch->getLocation();
    //drag_stop_time = event->time;
    // reset the lanscape view
    
    Vec2 delta = drag_start-drag_stop;
    
    f32 xscale = (f32)BASE_WIDTH / this->getContentSize().width ;
    f32 yscale = (f32)BASE_HEIGHT / this->getContentSize().height ;
    
    f32 whole = RES(1024) ;
    f32 qtr = whole / 4 ;
    f32 half = whole / 2 ;
    f32 qtr3 = qtr+half;
    
    
    f32 dx = delta.x * xscale;
    f32 dy = delta.y * yscale;
    
    f32 adx = dx * ( 512.0f / 400.0f );
    
    
    right = DIR_LEFT ;
    left = DIR_RIGHT;
    
    // check for drag / up down
//    if ( fingermode==vertical && ABS(dy) > RES(MINIMUM_VERTICAL_DRAG_GESTURE) && ABS(dx)<50 ) {
//        
//        uievent ev(ET_MOUSE,EB_MOUSE_LEFT,0,0);
//        ev.m_mouse = point(0,0);
//        ev.m_time_start = event->time ;
//        ev.m_time_current = event->time ;
//        if ( delta.y > 0 )
//            ev.m_key|=IS_UP_MASK;
//        
//        NotifyDelegate(&ev);
//        fingermode=none;
//        return;
//    }
//    
//    if ( ! allowDragLook() ) {
//        fingermode=none;
//        return;
//    }
//    
    
    //u32 time_taken = drag_stop_time - drag_start_time ;
    int distance=0;
    
    //if ( fingermode == horizontal ) {
        
        right = DIR_LEFT ;
        left = DIR_RIGHT;
        
        // check right to left
        if ( dx > 0 ) {
            
            
//            if ( ABS(dx)>MINIMUM_HORIZONTAL_SWIPE_MOVEMENT && time_taken<QUICK_SWIPE_LIMIT ) {
//                // snap forward left
//                movement_amount=left*2;
//                snapback=3;
//                fingermode=none;
//                Disable();
//                return;
//            }
            
            
            
            if ( adx < qtr ) {               // 0 - 256
                // snap back right
                movement_amount=right;
                distance=adx;
                snapback=1;
            }else if ( adx < half ) {        // 256 - 512
                // snap forward left
                movement_amount=left;
                snapback=3;
                distance=half-adx;
            }else if ( adx < qtr3 ) {        // 512 - 768
                // snap back right
                movement_amount=right;
                snapback=5;
                distance=adx-half;
            }else {                         // 1024
                // snap forward left
                movement_amount=left;
                snapback=7;
                distance=whole-adx;
            }
            //Disable();
            //if ( distance<64 )
            //    movement_amount/=4;
            
        }
        
        // check left to right
        if ( dx < 0 ) {
            
//            if ( ABS(dx)>MINIMUM_HORIZONTAL_SWIPE_MOVEMENT && time_taken<QUICK_SWIPE_LIMIT ) {
//                // snap forward right
//                movement_amount=right*2;
//                snapback=4;
//                fingermode=none;
//                Disable();
//                return;
//            }
            
            adx = ABS(adx);
            
            if ( adx < qtr ) {               // 0 - 256
                // snap back left
                movement_amount=left;
                snapback=2;
            }else if ( adx < half ) {        // 256 - 512
                // snap forward right
                movement_amount=right;
                snapback=4;
            }else if ( adx < qtr3 ) {        // 512 - 768
                // snap back left
                movement_amount=left;
                snapback=6;
            }else{                          // 1024
                // snap forward right
                movement_amount=right;
                snapback=8;
            }
            //Disable();
            
        }
//    }
    fingermode=none;

    
    
}

void Landscape::onTouchMoved(Touch* touch, Event* event)
{
    cocos2d::log("touch moved");
    
//    int touches = ui->TouchCount();
//    
//    if ( drag_touch_count==1 && touches == 2 ) {
//        if ( ui->TimerGetMs()-drag_start_time<1000 )
//            drag_touch_count=2;
//        //return;
//    }
//    
    
    
//    if ( drag_touch_count == 2 && touches == 1 ) {
//        OnStopDrag(event);
//        if ( fingermode==vertical)
//            fingermode=none;
//        return;
//    }
    
    Vec2 delta = drag_current - touch->getLocation();
    
    f32 xscale = (f32)BASE_WIDTH / this->getContentSize().width ;
    f32 yscale = (f32)BASE_HEIGHT / this->getContentSize().height ;
    
    
    f32 dx = delta.x * xscale ;
    f32 dy = delta.y * yscale ;
    
    dx *= ( 512.0f / 400.0f );
    
    
    drag_current=touch->getLocation();
    
    //if ( fingermode==horizontal ) {
        
        //if ( !allowDragLook() )
        //    return;
        
        looking += dx;
        dragged += dx;
        
        //rotate_look += abs(turning);
        
        //ParallaxCharacters();
    //}else{
        
        
//        if ( fingermode==none && ABS(dy) > MINIMUM_VERTICAL_DRAG_MOVEMENT ) {
//            fingermode=vertical;
//        }
//        
//        // we need to move at least 12 pixel to warrant
//        // drag movement
//        if ( drag_touch_count == 1) {
//            if ( fingermode==none && ABS(dx) > MINIMUM_HORIZONTAL_DRAG_MOVEMENT ) {
//                fingermode=horizontal;
//            }
//        }
        
        
    //}

    UpdateLandscape();
    
    
}

void Landscape::onTouchCancelled(Touch* touch, Event* event)
{
    cocos2d::log("touch cancelled");
}




