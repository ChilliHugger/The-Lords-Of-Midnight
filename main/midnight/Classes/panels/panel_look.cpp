#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "panel_look.h"
#include "../system/moonring.h"

#include "../ui/uihelper.h"

#include "../landscaping/LandscapeSky.h"
#include "../landscaping/LandscapeLand.h"
#include "../landscaping/LandscapeTerrain.h"

#include "tme_interface.h"

#include "TMEMapBuilder.h"

USING_NS_CC;
using namespace tme;

bool panel_look::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    // TODO: DDR Tunnel view
    
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
    
    // TODO: Fade panel for transition in and out
    
    // TODO: Popup menu for character selection
    
    // TODO: Choose Action menu
    //  BOTH    Attack
    //          Approach
    //          Post Men
    //          Recruitment
    //          Night
    //          Map
    //          Home
    //          Think
    //  LOM     Fight
    //          Hide
    //          Unhide
    //          Seel
    //  DDR     Give
    //          Take
    //          Use
    //          Rest
    
    // TODO: Shortcut keys
    
    // TODO: Name label
    // TODO: Description Label
    
    // TODO: Shield / Character
    // TODO: Following
    
    // TODO: Debug Menu
    
    // TODO: Compass
    
    // TODO: Direction movement indicators
    
    // TODO: Help Dialog
    
    return true;
}

//void panel_look::hideMenus ( void )
//{
//}

//void panel_look::OnMovementComplete( uiview* sender, LANDSCAPE_MOVEMENT type )
//{
//}

//void panel_look::OnActionComplete( uiaction* sender, s32 value )
//{
//  UNDO
//  Enter Tunnel
//  Exit Tunnel
//  Menu Collapse
//  Delayed Save
//}

//void panel_look::OnCompassEvent(uicompass *sender, uicompassevent *event)
//{
//}

//void panel_look::fadeIn ( int tag, rgb_t colour )
//{
//}

//void panel_look::fadeOut ( int tag, rgb_t colour )
//{
//}

// TODO: Actions

void panel_look::GetCharacterInfo ( defaultexport::character_t& c, locationinfo_t* info)
{
//    info->id = c.id;
//    info->shield = GetCharacterShield(c) ;
//    info->person = GetCharacterImage(c);
//    
//#if defined(_DDR_)
//    info->tunnel = Character_IsInTunnel(c);
//    if ( info->shield == NULL )
//        info->shield = info->person ;
//#endif
//    
//    info->face = GetCharacterFace(c);
//    info->name = c.longname ;
//    lib::strupr ( info->name );
//    info->locationtext = TME_GetCharacterText ( c, "CharLocation" );
    
}


void panel_look::GetCurrentLocationInfo ( void )
{
    character&    c = TME_CurrentCharacter();
    GetCharacterInfo(c, current_info);
    TME_GetCharacterLocationInfo ( c );
    
    
    if ( c.following ) {
        character tempc;
        TME_GetCharacter(tempc, c.following );
        GetCharacterInfo(tempc, follower_info);
    }else{
        follower_info->id=0;
    }
    
#if defined(_DDR_)
    if ( current_info->tunnel ) {
        if ( current_view != tunnel ) {
            RemoveChild(landscape);
            //landscape->OnDeInit();
            AddChild(tunnel);
            current_view=tunnel;
        }
    }else{
        if ( current_view != landscape ) {
            RemoveChild(tunnel);
            //tunnel->OnDeInit();
            AddChild(landscape);
            current_view=landscape;
        }
        
    }
#else
//    if ( current_view != landscape ) {
//        AddChild(landscape);
//        current_view=landscape;
//    }
#endif
    
    
//    OnSetupIcons();
//    OnSetupFaces();
    
}

/*
void panel_look::SetViewForCurrentCharacter ( void )
{
#if defined (_LOM_)
    lblName->Text(current_info->name);
    lblName->Enable();
#endif
    
    lblDescription->Text(current_info->locationtext);
    ((uiimage*)(imgShield->children[0]))->Image(current_info->shield);
    
    imgShield->Enable();
    lblDescription->Enable();
    
    if ( follower_info->id ) {
        uiimage* img = (uiimage*)i_Following->children[0];
        img->Image(follower_info->face);
        i_Following->dimensions = img->dimensions ;
        i_Following->tag = follower_info->id;
        i_Following->ShowEnable();
        //ShowHelpWindow(HELP_GROUPED);
    }else{
        i_Following->HideDisable();
    }
}
*/

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
    
    // Add location text
    
    // Add characters in front
    
    // Add shield
    // Add shield if following
    
    // update actions
    
    
    
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



