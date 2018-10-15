#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "panel_look.h"
#include "../system/moonring.h"
#include "../system/configmanager.h"
#include "../system/helpmanager.h"
#include "../system/resolutionmanager.h"

#include "../frontend/layout_id.h"
#include "../frontend/choose_id.h"
#include "../frontend/language.h"

#include "../ui/uihelper.h"
#include "../ui/uicommandwindow.h"

#include "../landscaping/LandscapeSky.h"
#include "../landscaping/LandscapeLand.h"
#include "../landscaping/LandscapeTerrain.h"
#include "../landscaping/LandscapeColour.h"

#include "tme_interface.h"

#include "TMEMapBuilder.h"

#include <iostream>
#include <string>
#include <algorithm>

USING_NS_CC;
using namespace tme;
using namespace cocos2d::ui;

enum tagid_t {
    TAG_NONE            = 0,
    TAG_MENU_COLLAPSE   = 1,
    TAG_DELAYED_SAVE    = 2,
    TAG_UNDO            = 3,
    TAG_UNDO_DONE       = 4,
    TAG_ENTER_TUNNEL    = 5,
    TAG_ENTER_TUNNEL_DONE=6,
    TAG_EXIT_TUNNEL     =7,
    TAG_EXIT_TUNNEL_DONE=8,
};



bool panel_look::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    // TODO: DDR Tunnel view
    
    // TODO: DDR TunnelGenerator
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

    // Name Label
    lblName = Label::createWithTTF( uihelper::font_config_big, "" );
    lblName->getFontAtlas()->setAntiAliasTexParameters();
    lblName->setTextColor(Color4B::YELLOW);
    lblName->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(this,lblName,RES(32),RES(32));

    // Location Desction Label
    lblDescription = Label::createWithTTF( uihelper::font_config_big, "" );
    lblDescription->getFontAtlas()->setAntiAliasTexParameters();
    lblDescription->setTextColor(Color4B::WHITE);
    lblDescription->setLocalZOrder(ZORDER_DEFAULT);
    lblDescription->setWidth(RES(800-64));
    uihelper::AddTopLeft(this,lblDescription, RES(32),RES(64));

    
    current_info = new locationinfo_t();
    follower_info = new locationinfo_t();
    
    //
    // Generic mapping from Cocos2dx
    //
    cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback callback = [&] (Ref* ref ) {
        this->OnNotification(ref);
    };
    
    
    
    // Command Window
    i_command_window = new uicommandwindow(this);
//    i_command_window->setNotificationCallback ( [&](uinotificationinterface* s, uieventargs* e) {
//
//    });
    
    
    auto choose = uihelper::CreateImageButton("i_actions", ID_ACTIONS, callback);
    uihelper::AddBottomRight(this, choose, RES(10), RES(10) );
    choose->setLocalZOrder(ZORDER_DEFAULT);

    auto select = uihelper::CreateImageButton("i_select2", ID_SELECT_ALL, callback);
    uihelper::AddBottomLeft(this, select, RES(10), RES(10) );
    select->setLocalZOrder(ZORDER_DEFAULT);
    
    
    // Keyboard
    InitKeyboard();
    
    // TODO: Popup menu for character selection
    
    // TODO: Shortcut keys
    
    // TODO: Shield / Character
    // TODO: Following
    
    // TODO: Debug Menu
    
    // TODO: Compass
    
    // TODO: Direction movement indicators
    
    return true;
}

void panel_look::OnMovementComplete( /*uiview* sender,*/ LANDSCAPE_MOVEMENT type )
{
#if defined(_DDR_)
    if ( type == LM_MOVE_FORWARD_LEAVE_TUNNEL ) {
        OnExitTunnel();
        return;
    }
#endif
    
    draw_arrows=FALSE;
    
    StartInactivity();
    
    
    if ( type == LM_DRAG_START ) {
        landscape_dragging=true;
        StopInactivity();
        return;
    }
    
    landscape_dragging=FALSE;
    
    if ( type == LM_SHOW_COMPASS ) {
        StopInactivity();
        
        // TODO:
        //character&    c = TME_CurrentCharacter();
        //i_compass->Show(landscape->mouse_down_pos, c.looking );
        
        return;
    }
    
    Enable();
    
    SetObject(characterId);
    
    if ( type ==  LM_ROTATE_LEFT || type == LM_ROTATE_RIGHT || type == LM_NONE ) {
        mr->help->Shown(HELP_LOOKING_AROUND);
    }
    
    if ( type ==  LM_MOVE_FORWARD ) {
        
        mr->help->Shown(HELP_MOVEMENT);
        mr->help->Shown(HELP_MOVING);
        
        Disable();
        
        //
        // did we fight
        // or seek ?
        character&    c = TME_CurrentCharacter();
        if ( c.lastcommand == CMD_FIGHT ) {
            // think!
            mr->ShowPage(MODE_THINK_FIGHT, c.lastcommandid);
            
        }else if ( c.lastcommand == CMD_SEEK ) {
            mr->ShowPage(MODE_THINK_SEEK, c.lastcommandid);

        }
        
        this->scheduleOnce( [&](float) {
            delayedSave();
        }, 0.1f, "DelayedSave" );
        
        
    }
    
    // get the current location
    TME_GetCharacterLocationInfo(TME_CurrentCharacter());
    
    if ( location_infront_armies.foes_armies ) {
        if ( !ShowHelpWindow(HELP_BATTLE))
            return;
        if ( !ShowHelpWindow(HELP_ARMIES) )
            return;
    }
    
    int id = GET_ID(location_infront_object) ;
    if ( id >= OB_WOLVES && id<=OB_WILDHORSES ) {
        if ( !ShowHelpWindow(HELP_NASTIES) )
            return;
        return;
    }
    
#if defined(_DDR_)
    if ( Character_IsInTunnel(TME_CurrentCharacter()) ) {
        return;
    }
#endif
    
    
    defaultexport::location_t loc;
    TME_GetMapLocation(loc, location_infrontid );
    
#if defined(_DDR_)
    if ( loc.flags.Is(lf_mist) ) {
        if ( !ShowHelpWindow( HELP_TN_MISTS ) )
            return;
    }
#endif
    if ( (loc.terrain == TN_PLAINS ) || (loc.terrain== TN_PLAINS2) ) {
        TME_GetMapLocation(loc, location_lookingatid );
        if ( (loc.terrain != TN_PLAINS) && (loc.terrain!=TN_PLAINS2) )
            return;
    }
    
#if defined(_DDR_)
    // map plains for ddr plains
    if ( loc.terrain == TN_PLAINS )
        loc.terrain = TN_PLAINS2;
    loc.terrain = (mxterrain_t)((int)loc.terrain - TN_PLAINS2) ;
#endif
    
    if ( !ShowHelpWindow( (helpid_t)(HELP_NONE+1+(int)loc.terrain )) )
        return;
    
    
}

void panel_look::delayedSave()
{
    mr->stories->save();
    Enable();
    SetObject(characterId);
    
    if ( (mr->panels->currentmode == this->currentmode)
        && recruitable_characters.Count() ) {
        mr->ShowPage ( MODE_THINK_APPROACH, characterId );
    }
}

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

// TODO: Actions

void panel_look::GetCharacterInfo ( defaultexport::character_t& c, locationinfo_t* info)
{
    info->id = c.id;
    info->time = c.time;
    info->location = c.location;
    info->looking = c.looking;
    
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
    info->name = c.longname ;
    std::transform(info->name.begin(), info->name.end(), info->name.begin(), ::toupper);
    
    //lib::c_strupr ( info->name.c_str() );
    info->locationtext = TME_GetCharacterText ( c, "CharLocation" );
    
}

void panel_look::SetObject ( mxid c )
{
    if ( ID_TYPE(c) != IDT_CHARACTER )
        return;
    
    characterId = c ;
    TME_CurrentCharacter(c);
    
    //if ( parent ) {
        GetCurrentLocationInfo( );
        SetViewForCurrentCharacter();
    //}
    
    //if ( current_view )
    //    current_view->SetCharacter(c);
    
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
    
    OnSetupIcons();
    OnSetupFaces();
    
}



void panel_look::SetViewForCurrentCharacter ( void )
{
#if defined (_LOM_)
    lblName->setString(current_info->name);
    //lblName->Enable();
#endif
    
    lblDescription->setString(current_info->locationtext);
    //lblDescription->Enable();

    // TODO: Shield
    //((uiimage*)(imgShield->children[0]))->Image(current_info->shield);
    //imgShield->Enable();
    
    // TODO: Following
//    if ( follower_info->id ) {
//        uiimage* img = (uiimage*)i_Following->children[0];
//        img->Image(follower_info->face);
//        i_Following->dimensions = img->dimensions ;
//        i_Following->tag = follower_info->id;
//        i_Following->ShowEnable();
//        //ShowHelpWindow(HELP_GROUPED);
//    }else{
//        i_Following->HideDisable();
//    }

    options.colour->SetLookColour(current_info->time);
    
    options.here = current_info->location;
    options.here.x *= DIR_STEPS;
    options.here.y *= DIR_STEPS;
    
    // looking = (c.looking * DIR_AMOUNT) ;
    options.lookAmount = current_info->looking * 400;
    if ( options.lookAmount >= 3200 )
        options.lookAmount-=3200;
    
    options.currentLocation = current_info->location;
    options.currentDirection = current_info->looking;
    
    options.aheadLocation.x = options.currentLocation.x + mxgridref::DirectionLookTable[options.currentDirection*2];
    options.aheadLocation.y = options.currentLocation.y + mxgridref::DirectionLookTable[(options.currentDirection*2)+1];
    
    options.generator->Build(options.here, 0);
    
    UpdateLandscape();
    
    // Add characters in front
    
    // Add shield
    // Add shield if following
    
    // update actions
    
    
    
}

void panel_look::UpdateLandscape()
{
    if ( current_view ) {
        removeChild(current_view);
        current_view->release();
        current_view=nullptr;
    }
    
    options.generator->horizontalOffset = options.lookAmount;
    
    if ( current_info->tunnel ) {
        // TODO: Tunnels
        //tunnelView = new TunnelView();
        //current_view = tunnelView;
    }else{
        landscapeView = new LandscapeView();
        current_view = landscapeView;
    }
    
    current_view->programState = mr->glProgramState;
    current_view->Init(&options);
    current_view->setAnchorPoint(Vec2(0.5,0.5));
    current_view->setPosition( Vec2(getContentSize().width/2, getContentSize().height/2));
    current_view->setLocalZOrder(ZORDER_FAR);
    addChild(current_view);
    
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
                moveForward();
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


bool panel_look::moveForward ( void )
{
    character& c = TME_CurrentCharacter();
    
    TME_GetCharacterLocationInfo ( c );
    
    // something is in our way that we must fight
    // so check for auto fight
    if ( location_flags&lif_fight ) {
        if ( mr->config->autofight ) {
            // do we have an army?
            // do we have the correct sword?
            
            mxid fight = Character_Fight(c);
            
            if ( fight != IDT_NONE && Character_IsDead(c)) {
                mr->ShowPage(MODE_THINK_FIGHT,fight);
                mr->stories->save();
                return FALSE;
            }
            
            //return FALSE;
        }
    }
    
    //
    if ( Character_IsHidden(c) ) {
        if ( mr->config->autounhide ) {
            if ( Character_Hide(c) ) {
            }
        }
    }
    
    if ( !StartMoving() ) {
        
        character& c = TME_CurrentCharacter();
        
        TME_GetCharacterLocationInfo(c);
        
        if ( location_stubborn_lord_move!=IDT_NONE) {
            // switch to the stubborn character
            // and think
            SetObject(location_stubborn_lord_move);
            return true;
        }
        
        if ( Character_IsNight(c) ) {
            if ( !ShowHelpWindow(HELP_PRESS_NIGHT))
                return TRUE;
        }
        
        if ( c.following != IDT_NONE )
            if (!ShowHelpWindow(HELP_GROUPED))
                return TRUE;
        
        if ( location_infront_armies.foes_armies )
            if (!ShowHelpWindow(HELP_BATTLE, TRUE))
                return TRUE;
        
        // something is in our way that we must fight
        mxid object = location_flags&lif_fight ? location_fightthing : Character_LocationObject(c);
        
        mr->ShowPage(MODE_THINK,object);
        
        return TRUE;
    }
    
    return FALSE;
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
    
    OnMovementComplete(type);
}

void panel_look::StopMoving()
{
    options.isMoving = false;
    
    OnMovementComplete(LM_MOVE_FORWARD);
    
}

void panel_look::StartInactivity()
{
    this->scheduleOnce( [&](float) {
        ShowInactivityHelp();
    }, NONE_ACTIVTY_DURATION, "NonActivityTimer" );
}

void panel_look::StopInactivity()
{
    this->unschedule("NonActivityTimer");
}

void panel_look::ShowInactivityHelp()
{
    static helpid_t help_options[] = { HELP_LOOKING_AROUND, HELP_MOVEMENT, HELP_SELECTING_CHARACTER, HELP_THINKING, HELP_CHOOSE };
    
    if ( isEnabled() ) {
        if ( !isHelpVisible() ) {
            for ( u32 ii=0; ii<NUMELE(help_options); ii++ ) {
                if ( !ShowHelpWindow( help_options[ii] ) )
                    break;
            }
        }
    }
}

void panel_look::hideMenus ( void )
{
    // TODO: Hide Menus
//    if ( actions )
//        actions->Collapse();
//    if ( faces )
//        faces->Collapse();
//    if ( debug )
//        debug->Collapse();
}


void panel_look::fadeIn ( rgb_t colour, f32 initialAlpha,  MXVoidCallback callback )
{
//    if ( !gl->settings.screentransitions ) {
//        uiaction action;
//        action.tag=tag;
//        OnActionComplete(&action, 0);
//        return;
//    }
//
    auto rect = this->getBoundingBox();
    auto fade_panel = DrawNode::create();
    fade_panel->drawSolidRect(Vec2(0,0), Vec2( rect.getMaxX(), rect.getMaxY()), Color4F(colour) );
    fade_panel->setOpacity(ALPHA(initialAlpha));
    fade_panel->setLocalZOrder(ZORDER_NEAR);
    addChild(fade_panel);
    
    auto callbackAction = CallFunc::create([=]() {
        if ( callback != nullptr ) {
            callback();
        }
        this->removeChild(fade_panel);
        //fade_panel = nullptr;
    });
    
    auto action = FadeOut::create(3.0f);
    auto sequence = Sequence::createWithTwoActions(action, callbackAction);
    fade_panel->runAction(sequence);
    
}

void panel_look::fadeOut ( rgb_t colour, f32 initialAlpha,  MXVoidCallback callback )
{
//    if ( !gl->settings.screentransitions ) {
//        uiaction action;
//        action.tag=tag;
//        OnActionComplete(&action, 0);
//        return;
//    }
    
    auto rect = this->getBoundingBox();
    auto fade_panel = DrawNode::create();
    fade_panel->drawSolidRect(Vec2(0,0), Vec2( rect.getMaxX(), rect.getMaxY()), Color4F(colour) );
    fade_panel->setLocalZOrder(ZORDER_NEAR);
    fade_panel->setOpacity(ALPHA(initialAlpha));
    addChild(fade_panel);
    
    auto callbackAction = CallFunc::create([=]() {
        if ( callback != nullptr ) {
            callback();
        }
        this->removeChild(fade_panel);
        //fade_panel = nullptr;
    });
    
    auto action = FadeIn::create(3.0f);
    auto sequence = Sequence::createWithTwoActions(action, callbackAction);
    fade_panel->runAction(sequence);
}

void panel_look::OnShown()
{
    StartInactivity();
    
    
    if ( variables::sv_days == 0) {
        if ( !ShowHelpWindow(HELP_DAY1))
            return;
        
        if ( !ShowHelpWindow(HELP_CHOICE_OF_GAMES))
            return;
    }
    
    if ( variables::sv_days == 1) {
        if ( !ShowHelpWindow(HELP_BATTLE))
            return;
#if defined(_LOM_)
        if ( !ShowHelpWindow(HELP_ICEFEAR))
            return;
#endif
    }
    if ( variables::sv_days == 2) {
        if ( !ShowHelpWindow(HELP_VICTORY))
            return;
    }
    
#if defined(_LOM_)
    character& c = TME_CurrentCharacter();
    if ( c.race == RA_FREE || c.race == RA_MORKIN )
        if ( !ShowHelpWindow(HELP_FREE))
            return;
    if ( c.race == RA_FEY )
        if ( !ShowHelpWindow(HELP_FEY))
            return;
    if ( c.race == RA_WISE )
        if ( !ShowHelpWindow(HELP_WISE))
            return;
#endif
}

void panel_look::OnActivate( void )
{
    uipanel::OnActivate();
    
    // make sure we are working with our character
    SetObject(characterId);
    
    character&    c = TME_CurrentCharacter();
    
    StopInactivity();
    
    if ( Character_IsDead(c) ) {
        mr->ShowPage( MODE_THINK, c.id );
        return;
    }
    
    OnSetupIcons();
    OnSetupFaces();
}

void panel_look::OnDeActivate( void )
{
    uipanel::OnDeActivate();
}

void panel_look::OnSetupFaces()
{
}

void panel_look::OnNotification( Ref* sender )
{
    StopInactivity();
    
    auto button = static_cast<Button*>(sender);
    if ( button == nullptr )
        return;
    
    switch ( button->getTag() ) {
            
        case ID_ACTIONS:
            OnChoose();
            break;
            
        case ID_HOME:
            OnHome();
            break;
            
        case ID_UNDO_NIGHT:
            OnUndoNight();
            return;
            break;
            
        case ID_UNDO_DAWN:
        {
            OnUndoDawn();
            return;
            break;
        }
            
        case ID_UNDO:
        {
            OnUndo();
            return;
        }

        case ID_NIGHT:
        {
            OnNight();
            hideMenus();
            return;
        }
            
        case ID_MAP:
        {
            OnMap();
            break;
        }
            
        case ID_SELECT_ALL:
        {
            OnSelect();
            break;
        }
            
        case ID_APPROACH:
        {
            OnApproach();
            return;
        }
#if defined(_LOM_)
        case ID_SEEK:
        {
            OnSeek();
            return;
        }
            
        case ID_HIDE:
        case ID_UNHIDE:
        {
            if ( OnHideUnhide() ) {
                return;
            }
            break;
        }
            
        case ID_FIGHT:
        {
            if ( OnFight() ) {
                return;
            }
            break;
        }
#endif
            
#if defined(_DDR_)
        case ID_GIVE:
        {
            if ( OnGive() )
                return;
            break;
        }
        case ID_TAKE:
        {
            if ( OnTake() )
                return;
            break;
        }
        case ID_USE:
        {
            if ( OnUse() )
                return;
            break;
        }
        case ID_REST:
        {
            OnRest();
            break;
        }
        case ID_ENTER_TUNNEL:
        {
            OnEnterTunnel();
            return;
        }
#endif
            
        case ID_THINK:
        {
            OnThink();
            return;
        }
            
        case ID_RECRUITMEN:
        {
            if ( OnRecruitMen() )
                return;
            break;
        }
        case ID_POSTMEN:
        {
            if ( OnPostMen() )
                return;
            break;
        }
            
        case ID_ATTACK:
        {
            if ( OnAttack() )
                return;
            break;
        }
    }
    
    hideMenus();
    
    TME_RefreshCurrentCharacter();
    TME_GetCharacterLocationInfo ( TME_CurrentCharacter() );
    uipanel::OnNotification(sender);
}

#pragma mark Commands
#pragma mark -


bool panel_look::OnChoose()
{
    i_command_window->show(nullptr);
    return true;
}

bool panel_look::OnHome()
{
    hideMenus();
    
    AreYouSure(CLOSE_STORY_MSG, [&] {
        mr->stories->save();
        mr->stories->cleanup();
        mr->ShowPage( MODE_MAINMENU, 0 );
    });
    
    return true;
}

bool panel_look::OnUndoNight()
{
    hideMenus();
    
    AreYouSure(UNDO_NIGHT_MSG, [&] {
          OnUndo(savemode_night);
    });
    
    return true;
}

bool panel_look::OnUndoDawn()
{
    hideMenus();
    
    AreYouSure(UNDO_DAWN_MSG, [&] {
        OnUndo(savemode_dawn);
    });
    
    return true;
}

bool panel_look::OnUndo()
{
    hideMenus();
    OnUndo(savemode_normal);
    return true;
}

bool panel_look::OnUndo ( savemode_t mode )
{
    undo_mode=mode;
    fadeOut(_clrBlack, 0.75f, [&](){
        mr->stories->undo(undo_mode);
        SetObject( TME_CurrentCharacter().id );
        TME_RefreshCurrentCharacter();
        TME_GetCharacterLocationInfo ( TME_CurrentCharacter() );
        fadeIn(_clrBlack, 1.0f, [&]{
                // do nothing
        });
    });
    return true;
}

bool panel_look::OnNight()
{
    hideMenus();
    
    AreYouSure(NIGHT_MSG, [&] {
        mr->stories->save(savemode_night);
        mr->ShowPage( MODE_NIGHT );
    });
    
    return true;
}

bool panel_look::OnMap()
{
    hideMenus();
    mr->ShowPage(MODE_MAP);
    return true;
}

bool panel_look::OnSelect()
{
    hideMenus();
    mr->help->Shown(HELP_SELECTING_CHARACTER);
    mr->ShowPage( MODE_SELECT );
    return true;
}

bool panel_look::OnApproach()
{
    hideMenus();
    // if the recruit is successful then
    // make the new character the current character
    
    character& c = TME_CurrentCharacter();
    
    if ( Character_Approach(c) ) {
        if ( characterId != TME_CurrentCharacter().id ) {
            // TODO: Add new lord to select screen
            //mr->ShowPage(MODE_SELECT, TME_CurrentCharacter().id );
            //select->AddNewLord(TME_CurrentCharacter().id);
            SetObject( TME_CurrentCharacter().id );
        }
    }else{
        TME_RefreshCurrentCharacter();
    }
    
    mr->ShowPage ( MODE_THINK );
    mr->stories->save();
    return true;
}

bool panel_look::OnSeek()
{
    hideMenus();
    character& c = TME_CurrentCharacter();
    mr->ShowPage(MODE_THINK_SEEK, Character_Seek(c));
    mr->stories->save();
    return true;
}

bool panel_look::OnHideUnhide()
{
    hideMenus();
    character& c = TME_CurrentCharacter();
    if ( Character_Hide(c) ) {
        mr->ShowPage(MODE_THINK);
        mr->stories->save();
        return true;
    }
    return false;
}

bool panel_look::OnFight()
{
    hideMenus();
    character& c = TME_CurrentCharacter();
    mxid fight = Character_Fight(c);
    if ( fight != IDT_NONE ) {
        mr->stories->save();
        mr->ShowPage ( MODE_THINK_FIGHT, fight );
        return true;
    }
    return false;
}

bool panel_look::OnThink()
{
    hideMenus();
    character& c = TME_CurrentCharacter();
    mr->ShowPage(MODE_THINK_PERSON, Character_LocationObject(c));
    return true;
}

bool panel_look::OnRecruitMen()
{
    hideMenus();
    character& c = TME_CurrentCharacter();
    if ( Character_RecruitMen(c) ) {
        mr->stories->save();
        mr->ShowPage ( MODE_THINK_RECRUITGUARD, 0 );
        return true;
    }
    return false;
}

bool panel_look::OnPostMen()
{
    hideMenus();
    character& c = TME_CurrentCharacter();
    if ( Character_PostMen(c) ) {
        mr->stories->save();
        mr->ShowPage ( MODE_THINK_RECRUITGUARD, 0 );
        return true;
    }
    return false;
}

bool panel_look::OnAttack()
{
    character& c = TME_CurrentCharacter();
    if ( Character_Attack(c) ) {
        SetObject(characterId);
        mr->stories->save();
        mr->ShowPage(MODE_THINK_ARMY);
        return true;
    }else{
        TME_GetCharacterLocationInfo(c);
        if ( location_stubborn_lord_attack!=IDT_NONE ) {
            SetObject(location_stubborn_lord_attack);
            mr->stories->save();
            mr->ShowPage(MODE_THINK_PERSON);
            return true;
        }
    }
    return false;
}

#if defined(_DDR_)
bool panel_look::OnGive()
{
    hideMenus();
    if ( Character_Give(c,location_someone_to_give_to) ) {
        mr->stories->save();
        mr->ShowPage( MODE_THINK_PLACE );
        return true;
    }
    return false;
}
bool panel_look::OnTake()
{
    hideMenus();
    if ( Character_Take(c) ) {
        mr->stories->save();
        mr->ShowPage( MODE_THINK_PLACE );
        return true;
    }
    return false;
}

bool panel_look::OnUse()
{
    hideMenus();
    if ( Character_Use(c) ) {
        mr->stories->save();
        mr->ShowPage( MODE_THINK_SEEK );
        return true;
    }
    return false;
}

bool panel_look::OnRest()
{
    hideMenus();
    Character_Rest(c);
    mr->stories->save();
    mr->ShowPage(MODE_THINK);
    return true;
}

bool panel_look::OnEnterTunnel()
{
    hideMenus();
    fadeOut(_clrBlack, 0.75f, [&](){
        Character_EnterTunnel(TME_CurrentCharacter());
        mr->stories->save();
        GetCurrentLocationInfo();
        SetViewForCurrentCharacter();
        current_view->SetViewForCurrentCharacter();
        fadeIn(_clrBlack, 1.0f, [&](){
            ShowHelpWindow(HELP_TN_TUNNEL);
        });
    });
    
    
    return true;
}

bool panel_look::OnExitTunnel()
{
    hideMenus();
    fadeOut(_clrWhite, 0.0f, [&]() {
        OnMovementComplete(NULL, LM_MOVE_FORWARD);
        fadeIn(_clrBlack, 1.0f, [&](){
        });
    });
    return true;
}
#endif


void panel_look::OnSetupIcons ( void )
{

// TODO: Debug menu
//    if ( debug ) {
//        debug->ClearItems();
//
//        if ( gl->stories->canUndo(savemode_night) )
//            debug->AddItem(i_debug_undo_night);
//
//        if ( gl->stories->canUndo(savemode_dawn) )
//            debug->AddItem(i_debug_undo_dawn);
//
//        if ( gl->stories->canUndo(savemode_normal) )
//            debug->AddItem(i_debug_undo);
//
//        debug->AddItem(i_debug_email);
//        debug->AddItem(i_debug_map);
//    }
    
    
    if ( i_command_window==NULL )
        return;
    
    character& c = TME_CurrentCharacter();
    TME_GetCharacterLocationInfo(c);
    
    
    //i_command_window->clearItems();
    

#if defined(_LOM_)
    // SEEK
    i_command_window->enableItem(ID_SEEK, location_flags.Is(lif_seek) );
    
    // HIDE
    if ( Character_IsHidden(c) ) {
        i_command_window->showItem(ID_UNHIDE, true);
        i_command_window->showItem(ID_HIDE, false);
    } else {
        i_command_window->showItem(ID_UNHIDE, false);
        i_command_window->showItem(ID_HIDE, true);
        i_command_window->enableItem(ID_HIDE, location_flags.Is(lif_hide));
    }

    // FIGHT
    i_command_window->enableItem(ID_FIGHT, location_flags.Is(lif_fight) );

#endif


#if defined(_DDR_)

    i_command_window->enableItem(ID_GIVE, location_flags.Is(lif_give) );

//    if ( location_flags.Is(lif_give)) {
//        character c;
//        TME_GetCharacter(c, location_someone_to_give_to );
//        i_GiveText->Text(c.shortname);
//        i_GiveText->ShowEnable();
//    } else {
//        i_Give->Disable();
//        i_GiveText->HideDisable();
//    }
    
    i_command_window->enableItem(ID_TAKE, location_flags.Is(lif_take) );
    i_command_window->enableItem(ID_USE, location_flags.Is(lif_use) );
    i_command_window->enableItem(ID_REST, location_flags.Is(lif_rest) );
    i_command_window->enableItem(ID_ENTER_TUNNEL, location_flags.Is(lif_enter_tunnel) );

    
#endif
    
    // APPROACH
    i_command_window->enableItem(ID_APPROACH, location_flags.Is(lif_recruitchar) );

    // RECRUIT SOLDIERS
    i_command_window->enableItem(ID_RECRUITMEN, location_flags.Is(lif_recruitmen) );

    // POST SOLDIERS
    i_command_window->enableItem(ID_POSTMEN, location_flags.Is(lif_guardmen) );

    // BATTLE
    i_command_window->enableItem(ID_ATTACK, location_flags.Is(lif_enterbattle) );

    
//    if ( SELECT_MODE == 2 ) {
//        int count=0;
//        for ( u32 ii=0; ii<i_char.Count(); ii++ ) {
//            uielement* i = i_char[ii];
//            if ( characterId != i->tag ) {
//                character c;
//                TME_GetCharacter(c, i->tag);
//                if ( Character_IsAlive(c) && Character_IsControllable(i->tag) ) {
//                    actions->addItem(i,CHOOSE_CHAR+count);
//                    count++;
//                }
//            }
//        }
//    }
    
    
    i_command_window->enableItem(ID_UNDO_DAWN, mr->stories->canUndo(savemode_dawn) );
    i_command_window->enableItem(ID_UNDO, mr->stories->canUndo(savemode_last) );

}

