
#include "TMEMapBuilder.h"
#include "LandscapeScene.h"



USING_NS_CC;

#define _DEBUG_LANDSCAPE_
//#define _CITADEL_MAP_
#define _LOM_MAP_


//#define MX_PI                   3.141592654358979323
//#define MX_PI2                  (MX_PI*2.0)

//#define RES(x) (s32)	((x)*1.0)
//#define ASP(x) (s32)	((x)*uimanager::singleton()->aspect_scale)
//#define SRC(x) (s32)	((x)*uimanager::singleton()->source_scale)


const std::string terrain_graphics[] = {
    
// lom
    ""                  //    TN_PLAINS
    , "t_citadel0"      //    TN_CITADEL
    , "t_forest0"       //    TN_FOREST
    , "t_henge0"        //    TN_HENGE
    , "t_tower0"        //    TN_TOWER
    , "t_village0"      //    TN_VILLAGE
    , "t_downs0"        //    TN_DOWNS
    , "t_keep0"         //    TN_KEEP
    , "t_snowhall0"     //    TN_SNOWHALL
    , "t_lake0"         //    TN_LAKE
    , "t_frozenwaste0"  //    TN_FROZENWASTE
    , "t_ruin0"         //    TN_RUIN
    , "t_lith0"         //    TN_LITH
    , "t_cavern0"       //    TN_CAVERN
    , "t_mountain0"     //    TN_MOUNTAIN
    , ""                //    TN_UNUSED_15
    
// ddr
    , ""                //    TN_PLAINS2
    , ""                //    TN_MOUNTAIN2
    , ""                //    TN_FOREST2
    , ""                //    TN_HILLS
    , ""                //    TN_GATE
    , ""                //    TN_TEMPLE
    , ""                //    TN_PIT
    , ""                //    TN_PALACE
    , ""                //    TN_FORTRESS
    , ""                //    TN_HALL
    , ""                //    TN_HUT
    , ""                //    TN_WATCHTOWER
    , ""                //    TN_CITY
    , ""                //    TN_FOUNTAIN
    , ""                //    TN_STONES
    , ""                //    TN_ICYWASTE
    , ""                //    TN_UNUSED_32
    
// citadel
    , ""                //    TN_LAND
    , ""                //    TN_ISLE
    , ""                //    TN_LAKELAND
    , ""                //    TN_PLAIN
    , ""                //    TN_PLAINS3
    , "t_forest0"                //    TN_FOREST3
    , ""                //    TN_UNUSED_39
    , "t_trees0"                //    TN_TREES
    , "t_mountain0"                //    TN_MOUNTAIN3
    , "t_mountain0"                //    TN_ICY_MOUNTAIN
    , "t_downs0"                //    TN_DOWNS3
    , "t_downs0"                //    TN_HILLS3
    , "t_downs0"                //    TN_FOOTHILLS
    , ""                //    TN_VALLEY
    , ""                //    TN_BAY
    , ""                //    TN_SEA
    , ""                //    TN_RIVER
    , ""                //    TN_MARSH
    , ""                //    TN_LAKE3
    , ""                //    TN_UNUSED_52
    , ""                //    TN_UNUSED_53
    , "t_mist0"                //    TN_UNUSED_54
    , "t_mist0"                //    TN_MIST
    , ""                //    TN_UNUSED_56

};

const std::string floor_graphics[] = {
    //"t_snow1"
    //, "t_snow1"
    //, "t_snow1"
    
    "t_land1"
    , "t_snow1"
    , "t_water0"
    , "t_water1"
    , "t_water3"
    , "t_snow1"
};


Scene* Landscape::createScene()
{
    return Landscape::create();
}


//static GLProgram* getOrCreateShader(std::string name, const GLchar* vert, const GLchar* frag)
//{
//    auto cache = GLProgramCache::getInstance();
//    auto prog = cache->getGLProgram(name);
//    if(prog == nullptr)
//    {
//        prog = GLProgram::createWithByteArrays(vert, frag);
//        cache->addGLProgram(prog, name);
//    }
//    return prog;
//}



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
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("terrain/lom_M-0.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("terrain/lom_M-1.plist");
#endif
    
#if defined(_CITADEL_MAP_)
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("terrain/citadel_M-0.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("terrain/citadel_M-1.plist");
#endif
    
    masterScale = 1.0;

    
    
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
    
    landscapeGen = new LandscapeGenerator();
    
    
    

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
    
    showWater = true;
    showLand  = true;
    showTerrain = true ;
    debugMode = 0;
    landScaleX = 1; //1.6;
    landScaleY = 1; //2.15;
    debugLand=false;

    //character&	c = TME_CurrentCharacter();
    //Character_Place( c, loc_t(2,110) );
    //Character_Look(c, DR_NORTH);

    
    SetViewForCurrentCharacter();
    
    return true;
}

void Landscape::InitKeyboard()
{
    
    auto eventListener = EventListenerKeyboard::create();
    
    //auto me = this;
    
    eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event){

        if ( turning != 0 )
            return;
        
        Vec2 loc = event->getCurrentTarget()->getPosition();
        switch(keyCode){
                
            case cocos2d::EventKeyboard::KeyCode::KEY_F2:
                showLand = !showLand;
                UpdateLandscape();
                break;
                
            case cocos2d::EventKeyboard::KeyCode::KEY_F3:
                showWater = !showWater;
                UpdateLandscape();
                break;

            case cocos2d::EventKeyboard::KeyCode::KEY_F4:
                showTerrain = !showTerrain;
                UpdateLandscape();
                break;

            case cocos2d::EventKeyboard::KeyCode::KEY_F5:
                debugLand = !debugLand;
                UpdateLandscape();
                break;
                
            case cocos2d::EventKeyboard::KeyCode::KEY_F1:
                debugMode += 1;
                if ( debugMode > 4 )
                    debugMode = 0;
                UpdateLandscape();
                break;
                
            case cocos2d::EventKeyboard::KeyCode::KEY_O:
                landScaleX += 0.01;
                UpdateLandscape();
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_P:
                landScaleX -= 0.01;
                UpdateLandscape();
                break;

            case cocos2d::EventKeyboard::KeyCode::KEY_Q:
                landScaleY += 0.01;
                UpdateLandscape();
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_A:
                landScaleY -= 0.01;
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
    if ( current_landscape_node ) {
        this->removeChild(current_landscape_node);
        current_landscape_node->release();
        current_landscape_node=nullptr;
    }
    
    
    //UpdateScreenCoordinates();
    
    current_landscape_node = new Node();
    
    
    
    
    float scalex = 1024.0 / RES(64) ;
    
    auto floor = Sprite::createWithSpriteFrameName( "floor" );
    floor->setScale(scalex,masterScale);
    floor->setPosition(0, 0);
    floor->setAnchorPoint( Vec2(0,0) );
    
    if ( debugLand )
        floor->setColor(Color3B::YELLOW);
    
    current_landscape_node->addChild(floor);
  
    
    auto sky = Sprite::createWithSpriteFrameName( "sky_day" );
    sky->setScale(scalex,masterScale);
    sky->setPosition(0,RES((80*GSCALE)) );
    sky->setAnchorPoint( Vec2(0,0) );
    current_landscape_node->addChild(sky);

    // generate floors
    //BuildFloors(landscapeGen->items);
    
    
    // generate terrain
    BuildTerrain(landscapeGen->items);

    BuildDebug(landscapeGen->items);
    
    
    this->addChild(current_landscape_node);
    
    
}

void Landscape::UpdateScreenCoordinates()
{
    for(auto const &item: *landscapeGen->items) {
        landscapeGen->loc = here;
        landscapeGen->looking = looking;
        landscapeGen->CalcCylindricalProjection(item);
    }
    
}

int Landscape::NormaliseXPosition(int x)
{
    x = x-rotate_look ;
    if ( rotate_look<1024 && x>1600 )
        x -= 3200;
    if ( rotate_look>=1024 && x<-512 )
        x += 3200;
    return x;
}

void Landscape::BuildFloors( LandscapeItems* items )
{
    LandscapeItem* loc1=nullptr;
    LandscapeItem* loc2=nullptr;
    
    for(auto const& item: *items) {
    
        if ( item->loc == currentLocation)
            loc1 = item;
        
        if ( item->loc == aheadLocation )
            loc2 = item;
    }
    
    auto fnDrawTerrain = [=]( LandscapeItem* item, f32 adjustX, f32 adjustY ) {
        if ((item->position.z>=landscapeGen->near)&&(item->position.z<landscapeGen->far))
        {
            auto graphic = GetFloorImage(item->floor);
            
            if ( graphic ) {
                graphic->setPosition(NormaliseXPosition(item->position.x), this->getContentSize().height - item->position.y);
                
                graphic->setScaleX( graphic->getScaleX() * item->scale*adjustX );
                graphic->setScaleY( graphic->getScaleY() * item->scale*adjustY );
                
                current_landscape_node->addChild(graphic);
                
            }
        }

    };
    
    auto fnDrawItem = [=]( LandscapeItem* item ) {
        
        if ( item == loc1  )
            return;
        
        if ( item != loc2) {
            fnDrawTerrain(item, 1.0f, 1.0f);
            return ;
        }
        
        LandscapeItem under;
        under.floor = item->floor;
        under.position = item->position;
        under.scale = item->scale;
        fnDrawTerrain(&under, 1.0f, 1.0f);
        
//        under.floor = item->floor;
//        under.position = item->position;
//        under.position.x -= 128 * loc2->scale;
//        fnDrawTerrain(&under, 1.0f, 1.0f);
//        
//        under.floor = item->floor;
//        under.position = item->position;
//        under.position.x += 128 * loc2->scale;
//        fnDrawTerrain(&under, 1.0f, 1.0f);

//        under.floor = item->floor;
//        under.position = item->position;
//        under.position.y -= 20;
//        fnDrawTerrain(&under, 1.0f, 1.0f);
//        
//        under.floor = item->floor;
//        under.position = item->position;
//        under.position.y += 20 ;
//        fnDrawTerrain(&under, 1.0f, 1.0f);
        
        
    };
    
    

    
    //
    // Do the water first
    //
    if ( showWater ) {
        for(auto const& item: *items) {
            if ( item->floor != floor_river && item->floor != floor_sea && item->floor != floor_lake )
                continue;
            
            fnDrawItem(item);
        }
    }

    
    
    //
    // The everything else apart from snow
    //
    if ( showLand ) {
        for(auto const& item: *items) {
            if ( item->floor != floor_normal && item->floor != floor_debug )
                continue;
            
            fnDrawItem(item);
        }

        //
        // then snow
        //
        for(auto const& item: *items) {
            
            if ( item->floor != floor_snow )
                continue;
            
            fnDrawItem(item);
            
        }
        
    }


    
    
    LandscapeItem under;
    under.floor = floor_debug; //loc1->floor;
    under.position = loc1->position;
    under.position.y = landscapeGen->HorizonCentreY + landscapeGen->PanoramaHeight + (landscapeGen->LocationHeight/2);
    under.position.z = 1;
    
    under.scale = 1.0;
    fnDrawTerrain(&under, 1.0f, 1.0f);
    
//    under.position.x = loc1->position.x - RES(1024) ;
//    fnDrawTerrain(&under, 1.0f, 1.0f);
//    
//    under.position.x = loc1->position.x - RES(2048) ;
//    fnDrawTerrain(&under, 1.0f, 1.0f);
//    
//    under.position.x = loc1->position.x + RES(1024) ;
//    fnDrawTerrain(&under, 1.0f, 1.0f);
//    
//    under.position.x = loc1->position.x + RES(2048) ;
//    fnDrawTerrain(&under, 1.0f, 1.0f);
    
    
    
    //
    //        under.floor = item->floor;
    //        under.position = item->position;
    //        under.position.x += 128 * loc2->scale;
    //        fnDrawTerrain(&under, 1.0f, 1.0f);
    
    //        under.floor = item->floor;
    //        under.position = item->position;
    //        under.position.y -= 20;
    //        fnDrawTerrain(&under, 1.0f, 1.0f);
    //
    //        under.floor = item->floor;
    //        under.position = item->position;
    //        under.position.y += 20 ;
    //        fnDrawTerrain(&under, 1.0f, 1.0f);
    
    
}

void Landscape::BuildTerrain( LandscapeItems* items )
{
    if ( !showTerrain )
        return;
    
    for(auto const& item: *items) {
        
        if ((item->position.z>=landscapeGen->near)&&(item->position.z<landscapeGen->far))
        {
            f32	alpha = 1.0f;
            
            // fade anything too close - ie as we move through the locations
            if (item->position.z<0.75f )
                alpha = (item->position.z - landscapeGen->near )*2.0f;
            
            auto graphic = GetTerrainImage(item->terrain);
            
            if ( graphic ) {
                
                int y = item->position.y;
                
                if ( item->position.z >= 8.5f  && y < landscapeGen->HorizonCentreY) {
                    f32 range = landscapeGen->far - 8.5f ;
                    f32 diff = landscapeGen->far - item->position.z ;
                    alpha = -1.0 + (2.0*(diff/range));
                    y = landscapeGen->HorizonCentreY ;
                    
                    if ( item->terrain == TN_MOUNTAIN || item->terrain == TN_FROZENWASTE)
                        alpha=1.0;
                    
                }
                
                
                graphic->setPosition(NormaliseXPosition(item->position.x), this->getContentSize().height - y);
                graphic->setScale( graphic->getScale() * item->scale );
                graphic->getGLProgramState()->setUniformFloat("p_alpha", alpha);               // alpha

                
                
                
                current_landscape_node->addChild(graphic);
            }
        }
        
    }
    
}

void Landscape::BuildDebug( Vector<LandscapeItem*>* items )
{
    if ( debugMode != 0 ) {

    
    for(auto const& item: *items) {
        
        if ((item->position.z>=landscapeGen->near)&&(item->position.z<landscapeGen->far))
        {

            int x = NormaliseXPosition(item->position.x);
            
            // debug
            auto dot = Sprite::createWithSpriteFrameName( "dot" );
            dot->setColor(Color3B::RED);
            dot->setPosition(x, this->getContentSize().height - item->position.y);
            dot->setScale(0.1f);
            dot->setAnchorPoint(Vec2(0.5,0.5));
            current_landscape_node->addChild(dot);

            char buffer[20];
            
            switch (debugMode) {
                case 1:
                    sprintf(buffer, "%d,%d", item->loc.x-(here.x/DIR_STEPS), item->loc.y-(here.y/DIR_STEPS));
                    break;
                case 2:
                    sprintf(buffer, "%f", item->scale);
                    break;
                case 3:
                    sprintf(buffer, "%d,%d", (int)item->position.x, (int)item->position.y);
                    break;
                case 4:
                    sprintf(buffer, "%d,%d", item->loc.x, item->loc.y);
                    break;
                default:
                    break;
            }
            
            
            auto label = Label::createWithTTF( buffer, "fonts/arial.ttf", 10);
            
            label->setColor(Color3B::BLUE);
            label->setAlignment(TextHAlignment::CENTER);
            label->setAnchorPoint(Vec2(0.5,0.5));
            label->setPosition(x, this->getContentSize().height - item->position.y - RES(5));
            current_landscape_node->addChild(label, 1);
            
            //        auto dot1 = Sprite::createWithSpriteFrameName( "dot" );
            //        dot1->setColor(Color3B::BLUE);
            //        dot1->setPosition(x-(256*scale), this->getContentSize().height - y);
            //        dot1->setScale(0.1f);
            //        dot1->setAnchorPoint(Vec2(0.5,0.5));
            //        current_landscape_node->addChild(dot1);
            //
            //        auto dot2 = Sprite::createWithSpriteFrameName( "dot" );
            //        dot2->setColor(Color3B::RED);
            //        dot2->setPosition(x+(256*scale), this->getContentSize().height - y);
            //        dot2->setScale(0.1f);
            //        dot2->setAnchorPoint(Vec2(0.5,0.5));
            //        current_landscape_node->addChild(dot2);

        }
        
    }
    
    }
    
    
    
    char buffer[255];

    sprintf(buffer, "Location %d,%d - Looking %f / %d Landscape Adj=(%f,%f) Rotating=%d"
            , currentLocation.x, currentLocation.y
            , looking, currentDirection
            , landScaleX, landScaleY
            , (int)rotate_look
            );
   
    
    
    auto label = Label::createWithTTF( buffer, "fonts/arial.ttf", 20);
    
    label->setMaxLineWidth(this->getContentSize().width);
    label->setAnchorPoint(Vec2(0,0));
    label->setColor(Color3B::WHITE);
    label->setPosition(RES(10), (this->getContentSize().height - RES(20)));
    current_landscape_node->addChild(label, 1);
    
    
}



Sprite* Landscape::GetTerrainImage( mxterrain_t terrain )
{
    
    auto image = Sprite::createWithSpriteFrameName( terrain_graphics[terrain] );
    if ( image == nullptr )
        return image;
    
    image->setScale(masterScale);
    image->setBlendFunc(cocos2d::BlendFunc::ALPHA_NON_PREMULTIPLIED);
    image->setGLProgramState( glProgramState->clone() );
    
    if ( terrain == TN_LAKE ) {
        image->setAnchorPoint(Vec2(0.5,1.0));
    }

    return image;
}

Sprite* Landscape::GetFloorImage( floor_t floor )
{

    Sprite* image = Sprite::createWithSpriteFrameName( floor_graphics[floor] );
    
    if ( image == nullptr )
        return image;
    
    //image->setScale(masterScale);
    
    //if ( floor == floor_water )
    //    image->setScale(0.5);
    
    //image->setBlendFunc(cocos2d::BlendFunc::ALPHA_NON_PREMULTIPLIED);
    //image->setGLProgramState( glProgramState->clone() );

    image->setScaleX(masterScale*landScaleX);
    image->setScaleY(masterScale*landScaleY);
    
    if ( floor == floor_debug ) {
        image->setColor(Color3B::YELLOW);
    }

    
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
    
    //Moving();
    
    //Rotating();
    
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
    
    //UpdateLandscape();
    
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
    here.x *= DIR_STEPS;
    here.y *= DIR_STEPS;
    
    looking = (c.looking * DIR_AMOUNT) ;
    rotate_look = c.looking * 400;
    if ( rotate_look >= 3200 )
        rotate_look-=3200;
    
    currentLocation = c.location;
    currentDirection = c.looking;
    
    aheadLocation.x = currentLocation.x + mxgridref::DirectionLookTable[currentDirection*2];
    aheadLocation.y = currentLocation.y + mxgridref::DirectionLookTable[(currentDirection*2)+1];
    
    
    // TODO: We don't need to generate the locations
    // every time, just refresh the screen positions
    // for the current look direction
    landscapeGen->Build(here, 0);
    
    //SetTimeOfDay(c.time);
    
    //current_info=info[0];
    //next_info=info[1];
    //prev_info=info[2];
    
//#if defined(_DDR_)
//    BOOL isInTunnel = Character_IsInTunnel(c) ;
//#else
//    BOOL isInTunnel = FALSE;
//#endif
    
    
    
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
    //rotate_look = 0; //512;
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
    
    f32 originalLooking = rotate_look;
    f32 target = -400 ;
    
    auto actionfloat = ActionFloat::create(0.5, 0, target, [=](float value) {
        turning = value;
        rotate_look = originalLooking + value;
        if ( value <= target) {
            StopRotating(LM_ROTATE_LEFT);
        }

        UpdateLandscape();
    });
    
    auto ease = new EaseSineInOut();
    ease->initWithAction(actionfloat);
    this->runAction(ease);
    
    dragged=0;
    
    return TRUE;
}

bool Landscape::StartLookRight ( void )
{
    //rotate_look=0;
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
    
    f32 originalLooking = rotate_look;
    f32 target = 400 ;
    
    auto actionfloat = ActionFloat::create(0.5, 0, target, [=](float value) {
        turning = value;
        rotate_look = originalLooking + value;

        if ( value >= target) {
            StopRotating(LM_ROTATE_RIGHT);
        }

        UpdateLandscape();
    });
    
    auto ease = new EaseSineInOut();
    ease->initWithAction(actionfloat);
    this->runAction(ease);
    
    
    dragged=0;
    
    
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
        
        f32 target = DIR_STEPS ;
        
        auto actionfloat = ActionFloat::create(0.5, 0, target, [=](float value) {

            here.x = moveFrom.x*(DIR_STEPS - value) + moveTo.x*value;
            here.y = moveFrom.y*(DIR_STEPS - value) + moveTo.y*value;
            
            if ( value >= target )
                StopMoving();

            //UpdateScreenCoordinates();
            
            landscapeGen->Build(here, 0);
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
    //rotate_look=0;
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
    
    //looking = looking+turning;

    
    if ( turning ) {
        
//        rotate_look += fabsf(turning);
        
//        if ( turning>0 )
//            turning-=1;
//        
//        if ( turning<0)
//            turning+=1;
        
        //f32 distance = rotate_look / (f32)DIR_AMOUNT;
        //if ( current_info )
        //    current_info->AdjustMovement( distance );
        //if (next_info)
        //    next_info->AdjustMovement( distance );
        UpdateLandscape();
    }
    
    
//    if ( turning < 0 ) {
//        if ( rotate_look >= DIR_AMOUNT ) {
//            StopRotating(LM_ROTATE_LEFT);
//        }
//    }
//    
//    if ( turning > 0 ) {
//        if ( rotate_look >= DIR_AMOUNT ) {
//            StopRotating(LM_ROTATE_RIGHT);
//        }
//    }
    
    
    
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




