#include "../axmol_sdk.h"

#include "panel_look.h"
#include "panel_think.h"

#include "../system/moonring.h"
#include "../system/settingsmanager.h"
#include "../system/helpmanager.h"
#include "../system/resolutionmanager.h"
#include "../system/keyboardmanager.h"
#include "../system/tmemanager.h"
#include "../system/shadermanager.h"

#include "../frontend/layout_id.h"
#include "../frontend/choose_id.h"
#include "../frontend/language.h"

#include "../ui/uihelper.h"
#include "../ui/uicommandwindow.h"
#include "../ui/uihelpwindow.h"
#include "../ui/characters/uisinglelord.h"
#include "../ui/uicompass.h"

#include "../landscaping/LandscapeSky.h"
#include "../landscaping/LandscapeLand.h"
#include "../landscaping/LandscapeTerrain.h"
#include "../landscaping/LandscapeColour.h"

#include "../landscaping/TunnelView.h"

#include "../tme/tme_interface.h"

#include "../tme/TMEMapBuilder.h"

#include <iostream>
#include <string>
#include <algorithm>

USING_NS_AX;
USING_NS_TME;
USING_NS_AX_UI;

enum tagid_t {
    TAG_NONE                = 0,
    TAG_UNDO                = 1,
    TAG_UNDO_DONE           = 2,
    TAG_MENU_COLLAPSE       _UNUSED_ = 3,
    TAG_DELAYED_SAVE        _UNUSED_ = 4,
    TAG_ENTER_TUNNEL        _UNUSED_ = 5,
    TAG_ENTER_TUNNEL_DONE   _UNUSED_ = 6,
    TAG_EXIT_TUNNEL         _UNUSED_ = 7,
    TAG_EXIT_TUNNEL_DONE    _UNUSED_ = 8,
};

//#define SHIELD_WIDTH    RES(192)
//#define SHIELD_HEIGHT   RES(224)
#define HEADER_HEIGHT   RES(228)

constexpr f32 FADE_IN_SPEED = 1.0f;
constexpr f32 FADE_OUT_SPEED = 1.0f;


#if defined(_LOM_)
#define DESCRIPTION_COLOUR  _clrWhite
#define SHIELD_X        RES(8)
#define SHIELD_Y        RES(16)
#define SHIELD_SCALE    1.0f
#endif

#if defined(_DDR_)
#define DESCRIPTION_COLOUR  _clrRed
#define SHIELD_X        RES(32)
#define SHIELD_Y        RES(0)
#define SHIELD_SCALE    1.0f
#define IMAGE_HEADER    "misc/header.png"
#endif

static f32 compass_delays[] = { 0.0f, 1.0f, 0.375f, 2.0f };

panel_look::panel_look() :
    current_view(nullptr),
    current_info(nullptr),
    i_command_window(nullptr),
    lblDescription(nullptr),
    lblName(nullptr),
    imgShield(nullptr),
    layHeader(nullptr),
    landscape_dragging(false),
    current_people(nullptr),
    next_people(nullptr),
    next_people1(nullptr),
    prev_people(nullptr),
    prev_people1(nullptr),
    options(nullptr),
    compass(nullptr),
    touchListener(nullptr),
    currentMovementIndicator(LM_NONE)
{
    CLEARARRAY(people);
    CLEARARRAY(movementIndicators);
    
}

panel_look::~panel_look()
{
    if(options!=nullptr)
    {
        UIDEBUG("panel_look: Delete Landscape options");
        options->terrainTimeShader = nullptr;
        options->characterTimeShader = nullptr;
        SAFEDELETE(options->colour)
        SAFEDELETE(options->generator)
        SAFEDELETE(options);
    }
    
    AX_SAFE_RELEASE_NULL(i_command_window);
}

bool panel_look::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    // TODO: DDR Tunnel view
    
    // TODO: DDR TunnelGenerator
    UIDEBUG("panel_look: Alloc Landscape options");
    options = new LandscapeOptions();
    options->mr = GetMoonring();
    options->generator = new LandscapeGenerator();
    options->colour = new LandscapeColour(options);
    options->showWater = false;
    options->showLand  = false;
    options->showTerrain = true ;
    options->debugMode = 0;
    options->landScaleX = 1.6f;
    options->landScaleY = 2.15f;
    options->debugLand=false;
    options->isMoving=false;
    options->isLooking=false;
    options->terrainTimeShader = mr->shader->GetTerrainTimeShader();
    options->characterTimeShader = mr->shader->GetCharacterTimeShader();

    // Header area
    layHeader = LayerColor::create(Color4B(_clrWhite), getContentSize().width, HEADER_HEIGHT );
    layHeader->setLocalZOrder(ZORDER_FAR+1);
    uihelper::AddTopLeft(this,layHeader);
    
#if defined(_DDR_)
    imgHeader = ImageView::create(IMAGE_HEADER);
    imgHeader->setColor(_clrBlack);
    uihelper::AddBottomLeft(layHeader,imgHeader,RES(0), RES(0));
    f32 lblNameAdjust = 0;
#endif

    // Name Label
#if defined(_LOM_)
    lblName = Label::createWithTTF( uihelper::font_config_big, "" );
    lblName->getFontAtlas()->setAntiAliasTexParameters();
    lblName->setTextColor(Color4B::YELLOW);
    lblName->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(safeArea,lblName,RES(32),RES(32));
    f32 lblNameAdjust = PHONE_SCALE(RES(32)) ;
#endif

    // Location Description Label
    lblDescription = Label::createWithTTF( uihelper::font_config_big, "" );
    lblDescription->getFontAtlas()->setAntiAliasTexParameters();
    lblDescription->setTextColor(Color4B(DESCRIPTION_COLOUR));
    lblDescription->setLocalZOrder(ZORDER_DEFAULT);
    lblDescription->setWidth(getContentSize().width*0.70f);
    uihelper::AddTopLeft(safeArea,lblDescription, RES(32),RES(32)+lblNameAdjust);

    // Shield
    imgShield = ImageView::create();
    imgShield->setLocalZOrder(ZORDER_DEFAULT);
    imgShield->setScale(SHIELD_SCALE);
    imgShield->setTouchEnabled(true);
    imgShield->addClickEventListener(clickCallback);
    imgShield->setTag(ID_THINK);
    
#if defined(_DDR_)
    // Take account of notch
    // It's been removed from the right, so utilsise the left side
    auto adjustX = mr->resolution->getSafeArea().left;
    imgShield->setAnchorPoint(uihelper::AnchorBottomRight);
    imgShield->setPosition(Vec2(getContentSize().width-SHIELD_X-adjustX, getContentSize().height-SHIELD_Y-HEADER_HEIGHT));
    safeArea->addChild(imgShield);
#else
    uihelper::AddTopRight(safeArea, imgShield,SHIELD_X,SHIELD_Y);
#endif
    

    // Leader Shield
    following = uisinglelord::create();
    following->setLocalZOrder(ZORDER_DEFAULT+1);
    following->addClickEventListener(clickCallback);
    following->setStatusImageVisible(false);
#if defined(_DDR_)
    uihelper::AddTopRight(safeArea, following, SHIELD_X+RES(96),HEADER_HEIGHT-RES(128));
#else
    uihelper::AddTopRight(safeArea, following, SHIELD_X+RES(32),SHIELD_Y+RES(128+48));
#endif
    
    // people in front
    for (auto &person : people) {
        person = LandscapePeople::create(options);
        person->setLocalZOrder(ZORDER_FAR+1);
        person->setContentSize( Size( getContentSize().width, RES(256)) );
        person->setCallback(clickCallback);
        addChild(person);
    }

    //
    current_info = new locationinfo_t();
    
    // Command Window
    i_command_window = uicommandwindow::create(this);
    i_command_window->retain();
    
    auto choose = uihelper::CreateImageButton("i_actions", ID_ACTIONS, clickCallback);
    uihelper::AddBottomRight(safeArea, choose, RES(10), RES(10) );
    
    // Character Selection
    auto select = uihelper::CreateImageButton("i_select2", ID_SELECT_ALL, clickCallback);
    uihelper::AddBottomLeft(safeArea, select, RES(10), RES(10) );

    // Help
    i_help = uihelper::CreateImageButton("i_tutorial_flash", ID_HELP, clickCallback);
    i_help->setVisible(false);
#if defined(_DDR_)
    uihelper::AddTopRight(safeArea, i_help, RES(8), HEADER_HEIGHT );
#else
    uihelper::AddTopRight(safeArea, i_help, RES(0), RES(8) );
#endif
    
    
    // map keyboard shortcut keys to layout children
    uishortcutkeys::registerCallback(safeArea, clickCallback);
    
    addTouchListener();
    
    // TODO: Popup menu for character selection
    
    // Shortcut keys
    addShortcutKey(ID_SELECT_ALL, K_SELECT);
    addShortcutKey(ID_ACTIONS,    K_CHOOSE);

    // TODO: Debug Menu
        
    // Direction movement indicators
    setupMovementIndicators();


    auto size = getContentSize();
    f32 landscapeWidth = size.height*1.3333;
    f32 centerLandscapeViewAdjustment = ((size.width - landscapeWidth)/2)*1.05;
    options->lookOffsetAdjustment = RES(LANDSCAPE_DIR_AMOUNT) - centerLandscapeViewAdjustment;
    
    return true;
}

void panel_look::OnMovementComplete( /*uiview* sender,*/ LANDSCAPE_MOVEMENT type )
{
    //UIDEBUG("OnMovementComplete(%d)", type);
    
    if ( type == LM_MOVE_FORWARD_LEAVE_TUNNEL ) {
        OnExitTunnel();
        return;
    }
    
    updateMovementIndicators(LM_NONE);
    
    startInactivity();
    
    if ( type == LM_DRAG_START ) {
        landscape_dragging=true;
        stopInactivity();
        return;
    }
    
    landscape_dragging=FALSE;
    
    Enable();
    
    setObject(characterId);
    
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
            mr->showPage(MODE_THINK_FIGHT, c.lastcommandid);
            
        }else if ( c.lastcommand == CMD_SEEK ) {
            mr->showPage(MODE_THINK_SEEK,c.lastcommandid);
        }else if ( c.lastcommand == CMD_APPROACH ) {
            mr->afterApproach();
            Enable();
            return;
        }
        
        mr->stories->save();
        
        Enable();
        setObject(characterId);
            
    #if defined(_DDR_)
        if (location_object_to_take!=ID_NONE) {
            mr->showPage ( MODE_THINK );
        } else
    #endif
        if ( (mr->panels->currentmode == this->currentmode)
            && !recruitable_characters.empty() ) {
            mr->showPage ( MODE_THINK_APPROACH, recruitable_characters.front() );
        }
    }
    
    // get the current location
    TME_GetCharacterLocationInfo(TME_CurrentCharacter());
    
    if ( location_infront_armies.foes_armies ) {
        if ( !showHelpWindow(HELP_BATTLE))
            return;
        if ( !showHelpWindow(HELP_ARMIES) )
            return;
    }
    
    int id = GET_ID(location_infront_object) ;
    if ( id >= OB_WOLVES && id<=OB_WILDHORSES ) {
        if ( !showHelpWindow(HELP_NASTIES) )
            return;
        return;
    }
    
    
    defaultexport::location_t loc;
    TME_GetMapLocation(loc, location_infrontid );

    if ( Character_IsInTunnel(TME_CurrentCharacter()) ) {
        if (loc.flags.Is(lf_tunnel_small)) {
            if ( !showHelpWindow( HELP_SMALL_TUNNEL ) )
                return;
        }
        return;
    }
    
#if defined(_DDR_)
    if ( loc.flags.Is(lf_mist) && !tme::variables::sv_display_no_mist) {
        if ( !showHelpWindow( HELP_TN_MISTS ) )
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
    
    if ( !showHelpWindow( (helpid_t)(HELP_NONE+1+(int)loc.terrain )) )
        return;
    
}

void panel_look::delayedSave()
{
}

void panel_look::getCharacterInfo ( character& c, locationinfo_t* info)
{
    info->id = c.id;
    info->time = c.time;
    info->location = c.location;
    info->looking = c.looking;
    info->following = c.following;
    info->shield = GetCharacterShield(c) ;
    info->person = GetCharacterImage(c);
    info->face = GetCharacterFace(c);
    info->tunnel = Character_IsInTunnel(c);
    info->lookingdowntunnel = false;
    info->lookingouttunnel = false;
    info->narrowtunnel = false;

    if ( (c.looking&1) == 0 ) {
        maplocation m;
        TME_GetLocationInDirection(m, c.location, c.looking);

        info->lookingdowntunnel = m.flags.Is(lf_tunnel) && info->tunnel;
        info->lookingouttunnel = m.flags.Is(lf_tunnel_exit) && info->tunnel;
        info->narrowtunnel = m.flags.Is(lf_tunnel_small) && info->tunnel;
    }

    info->name = StringExtensions::toUpper(c.longname) ;
    
    info->locationtext = TME_GetCharacterText ( c, "CharLocation" );
    
}

void panel_look::setObject ( mxid c )
{
    if ( ID_TYPE(c) != IDT_CHARACTER )
        return;
    
    characterId = c ;
    TME_CurrentCharacter(c);
    
    getCurrentLocationInfo();
    setViewForCurrentCharacter();
    
}


void panel_look::getCurrentLocationInfo()
{
    character&    c = TME_CurrentCharacter();
    getCharacterInfo(c, current_info);
    TME_GetCharacterLocationInfo ( c );

    OnSetupIcons();
    
}

void panel_look::setupLeaderButton()
{
    if ( current_info->following == IDT_NONE ) {
        following->setVisible(false);
        return;
    }
    
    character c;
    TME_GetCharacter(c, current_info->following);
    
    auto tag = (layoutid_t) (ID_SELECT_CHAR+c.id);
    following->setLord( c.id );
    following->setTag(tag);
    following->setUserData(static_cast<char_data_t*>(c.userdata));
    following->status.Clear();
    following->refreshStatus();
    following->setStatusImageVisible(false);
    following->setVisible(true);

}

void panel_look::setViewForCurrentCharacter()
{
    character&    c = TME_CurrentCharacter();
    
#if defined (_LOM_)
    lblName->setString(current_info->name);
#endif

    std::string shieldImage = current_info->shield;
    if ( shieldImage.empty() )
        shieldImage = current_info->person ;
    
    lblDescription->setString(current_info->locationtext);
    imgShield->loadTexture(shieldImage, Widget::TextureResType::LOCAL);

    setupLeaderButton();
    
    options->colour->SetLookColour(current_info->time);
    options->timeofday = current_info->time;
    
    options->here = current_info->location;
    options->here.x *= LANDSCAPE_DIR_STEPS;
    options->here.y *= LANDSCAPE_DIR_STEPS;
    options->isMoving = false;
    options->isLooking = false;
    options->isInTunnel = current_info->tunnel;
    options->isLookingDownTunnel = current_info->lookingdowntunnel;
    options->isLookingOutTunnel = current_info->lookingouttunnel;
    options->isNarrowTunnel = current_info->narrowtunnel;

    // TODO: Make this independent of UI
    options->lookAmount = (f32)(current_info->looking-1) * LANDSCAPE_DIR_AMOUNT;
    if ( options->lookAmount >= LANDSCAPE_FULL_WIDTH )
    {
        options->lookAmount-=LANDSCAPE_FULL_WIDTH;
    }
    
    options->currentLocation = current_info->location;
    options->currentDirection = current_info->looking;
    
    options->aheadLocation.x = options->currentLocation.x + mxgridref::DirectionLookTable[options->currentDirection*2];
    options->aheadLocation.y = options->currentLocation.y + mxgridref::DirectionLookTable[(options->currentDirection*2)+1];
    
    UIDEBUG("setViewForCurrentCharacter: Build Landscape");
    options->generator->Build(options);
    
    // Initialise people
    prev_people1=people[0];
    prev_people=people[1];
    current_people=people[2];
    next_people=people[3];
    next_people1=people[4];

    auto width = getContentSize().width;

    prev_people1->Initialise( c, PREV_DIRECTION(PREV_DIRECTION(current_info->looking)));
    prev_people1->setPositionX(0-width-width);
    
    prev_people->Initialise( c, PREV_DIRECTION(current_info->looking));
    prev_people->setPositionX(0-width);
    
    current_people->Initialise(c, current_info->looking );
    current_people->setPositionX(0);

    next_people->Initialise( c, NEXT_DIRECTION(current_info->looking));
    next_people->setPositionX(width);

    next_people1->Initialise( c, NEXT_DIRECTION(NEXT_DIRECTION(current_info->looking)));
    next_people1->setPositionX(width+width);

    UpdateLandscape();
    
    // Add shield if following
    
    // update actions
    
}

void panel_look::UpdateLandscape()
{
    //UIDEBUG("UpdateLandscape: Offset: %f", options->lookAmount);
 
    if ( current_view ) {
        UIDEBUG("UpdateLandscape: Remove Current View");
        removeChild(current_view);
    }
    
    options->generator->horizontalOffset = options->lookAmount;
    options->generator->landscapeScreenWidth = getContentSize().width ;

    if ( current_info->tunnel ) {
        UIDEBUG("UpdateLandscape: Create Tunnel View");
        current_view = TunnelView::create(options);
    }else{
        UIDEBUG("UpdateLandscape: Create Landscape View");
        current_view = LandscapeView::create(options);
    }

    current_view->setAnchorPoint(Vec2::ZERO);
    current_view->setPosition(Vec2::ZERO);
    current_view->setLocalZOrder(ZORDER_FAR);
    
    UIDEBUG("UpdateLandscape: Add Current View");
    addChild(current_view);
    
    // LoM's header is affectively part of the background
    // so update the colour to the same as the sky
#if defined(_LOM_)
    auto color = current_info->tunnel
        ? _clrBlack
        : Color3B(options->colour->CalcCurrentMovementTint(TINT::TerrainOutline));
    layHeader->setColor(color);
#endif

#if defined(_DDR_)
    if(!current_info->tunnel){
        imgHeader->setColor(Color3B(options->colour->CalcCurrentMovementTint(TINT::TerrainFill)));
    }
#endif

}

void panel_look::addTouchListener()
{
    using DragEventType = uidragevent::Type;
    
    // TODO: Turn these into gestures
    // we need swipe, drag, pinch zoom
    //
    // mouse events
    touchListener = EventListenerTouchOneByOne::create();
    
    // trigger when you push down
    touchListener->onTouchBegan = [this](Touch* touch, Event* event){
        mouse_down_time = utils::getTimeInMilliseconds() ;
        mouse_down_pos = touch->getLocation();
        mouse_last_position = mouse_down_pos;
        //UIDEBUG("Mouse Down = (%f,%f)", mouse_last_position.x, mouse_last_position.y );
        OnMouseEvent(touch,event,true);
        return true;
    };
    
    // trigger when moving touch
    touchListener->onTouchMoved = [this](Touch* touch, Event* event){
        
        Vec2 delta = touch->getLocation() - mouse_last_position;
        Vec2 distance = touch->getLocation() - mouse_down_pos;
       
        //UIDEBUG("Delta Mouse Move = (%f,%f)", delta.x, delta.y );
        
        if ( isDragging() ) {
            uidragevent    dev(nullptr,touch->getLocation(),DragEventType::drag);
            dev.startposition = mouse_down_pos;
            dev.lastposition = mouse_last_position;
            dev.time = utils::getTimeInMilliseconds() ;
            
            //UIDEBUG("Dragged = (%f,%f)", drag_amount.x, drag_amount.y );
            OnDrag(&dev);

        }
        
        else if ( ABS(distance.x) > MINIMUM_HORIZONTAL_DRAG_MOVEMENT
            || ABS(distance.y) > MINIMUM_VERTICAL_DRAG_MOVEMENT ) {
            uidragevent    dev(nullptr,touch->getLocation(),DragEventType::start);
            dev.startposition = mouse_down_pos;
            dev.lastposition = mouse_last_position;
            dev.starttime = mouse_down_time;
            dev.time = utils::getTimeInMilliseconds() ;
            OnStartDrag(&dev);
            //touch_capture = focus;
        }
        
        mouse_last_position = touch->getLocation();
        //UIDEBUG("Mouse Move = (%f,%f)", mouse_last_position.x, mouse_last_position.y );
        
    };
    
    // trigger when you let up
    touchListener->onTouchEnded = [this](Touch* touch, Event* event){
        
        if ( isDragging() ) {
            uidragevent    dev(nullptr,touch->getLocation(),DragEventType::stop);
            dev.startposition = mouse_down_pos;
            dev.lastposition = mouse_last_position;
            dev.starttime = mouse_down_time;
            dev.time = utils::getTimeInMilliseconds() ;
            OnStopDrag(&dev);
            return;
        }
        
        mouse_last_position = touch->getLocation();
        //UIDEBUG("Mouse Up = (%f,%f)", mouse_last_position.x, mouse_last_position.y );
        OnMouseEvent(touch,event,false);
    };
    
    // Add listener
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
}


bool panel_look::startLookLeft()
{
    Disable();
    
    character&    c = TME_CurrentCharacter();
    Character_LookLeft ( c );
    
    if ( options->isInTunnel || is(mr->settings->flipscreen) ) {
        stopRotating(LM_ROTATE_LEFT);
        return true;
    }
    
    options->colour->SetLookColour(c.time);
    
    f32 originalLooking = options->lookAmount;
    f32 target = -LANDSCAPE_DIR_AMOUNT ;
    
    options->isLooking = true;
    
    auto actionfloat = ActionFloat::create(TRANSITION_DURATION, 0, target, [=, this](float value) {
        options->lookAmount = originalLooking + value;
        
        //UIDEBUG("MovementLeft  %f %f %f", target, options->lookAmount, value);
        
        f32 distance = value / target;
  
        current_people->adjustMovement( distance );
        prev_people->adjustMovement( distance );
        prev_people1->adjustMovement( distance );

        if ( value <= target) {
            stopRotating(LM_ROTATE_LEFT);
            return;
        }
        UpdatePanningLandscape();
    });
    
    this->runAction(EaseSineInOut::create(actionfloat));
    
    prev_people1->startSlideFromLeft(2);
    prev_people->startSlideFromLeft(1);
    current_people->startSlideOffRight(0);
    
    return TRUE;
}

bool panel_look::startLookRight()
{
    Disable();
    
    character&    c = TME_CurrentCharacter();
    Character_LookRight ( c );
    
    if ( options->isInTunnel || is(mr->settings->flipscreen) ) {
        stopRotating(LM_ROTATE_RIGHT);
        return true;
    }
    
    options->colour->SetLookColour(c.time);
    
    f32 originalLooking = options->lookAmount;
    f32 target = LANDSCAPE_DIR_AMOUNT ;
    
    options->isLooking = true;
    
    auto actionfloat = ActionFloat::create(TRANSITION_DURATION, 0, target, [=, this](float value) {
        options->lookAmount = originalLooking + value;
        
        //UIDEBUG("MovementRight  %f %f %f", target, options->lookAmount, value);
        
        f32 distance = value / target;
        current_people->adjustMovement( distance );
        next_people->adjustMovement( distance );
        next_people1->adjustMovement( distance );

        if ( value >= target) {
            stopRotating(LM_ROTATE_RIGHT);
            return;
        }
        
        UpdatePanningLandscape();
    });
    
    this->runAction(EaseSineInOut::create(actionfloat));
    
    next_people1->startSlideFromRight(2);
    next_people->startSlideFromRight(1);
    current_people->startSlideOffLeft(0);
    
    return TRUE;
    
}


bool panel_look::moveForward()
{
    character& c = TME_CurrentCharacter();
    
    TME_GetCharacterLocationInfo ( c );
    
#if defined(_LOM_)
    // mr->checkFight?
    // something is in our way that we must fight
    // so check for auto fight
    if ( location_flags&lif_fight ) {
        if ( is(mr->settings->autofight) ) {
            // do we have an army?
            // do we have the correct sword?
            
            mxid fight = Character_Fight(c);
            
            if ( fight != IDT_NONE && Character_IsDead(c)) {
                mr->showPage(MODE_THINK_FIGHT,fight);
                mr->stories->save();
                return FALSE;
            }
            
            //return FALSE;
        }
    }
    
    // mr->checkUnhide?
    if ( location_flags&lif_unhide && Character_IsHidden(c) ) {
        if ( is(mr->settings->autounhide) ) {
            if ( Character_UnHide(c) ) {
            }
        }
    }
#endif
    
    if ( !startMoving() ) {
        
        c = TME_CurrentCharacter();
        
        TME_GetCharacterLocationInfo(c);
        
        if ( location_stubborn_lord_move!=IDT_NONE) {
            // switch to the stubborn character
            // and think
            setObject(location_stubborn_lord_move);
            mr->showPage(MODE_THINK);
            return true;
        }
        
        if ( Character_IsNight(c) ) {
            if ( !showHelpWindow(HELP_PRESS_NIGHT))
                return true;
        }
        
        if ( c.following != IDT_NONE )
            if (!showHelpWindow(HELP_GROUPED))
                return true;
        
#if defined(_DDR_)
        if(!recruitable_characters.empty() ) {
            mr->showPage ( MODE_THINK_APPROACH, recruitable_characters.front() );
            return true;
        }
#endif
        
        if ( location_infront_armies.foes_armies )
            if (!showHelpWindow(HELP_BATTLE, true))
                return true;
        
        // something is in our way that we must fight
        mxid objectid = location_flags&lif_fight ? location_fightthing : Character_LocationObject(c);
        
        // TODO: Character_UtterlyTired(c)
        if ( objectid == IDT_NONE && c.energy <= tme::variables::sv_energy_cannot_continue ) {
            mr->showPage(MODE_THINK_PERSON);
        }else{
            mr->showPage(MODE_THINK, objectid);
        }
        return true;
    }
    
    return false;
}



bool panel_look::startMoving()
{
    Disable();
    
    character&    c = TME_CurrentCharacter();
    
    mxtime_t startTime = c.time;
    
    mxid            key;
    maplocation        map;
    key = MAKE_LOCID(location_infront.x, location_infront.y);
    TME_GetLocation( map, key );
    
    options->moveLocationHasArmy = map.flags&lf_army ;
    options->moveFrom = c.location;
    
    if ( Character_Move(c) ) {
        
        if ( options->isInTunnel || is(mr->settings->flipscreen) ) {
            stopMoving();
            return true;
        }
        
        options->colour->SetMovementColour(startTime,c.time);
        
        options->moveTo = c.location;
        f32 target = LANDSCAPE_DIR_STEPS ;
        
        options->isMoving = true;
   
        auto actionfloat = ActionFloat::create(TRANSITION_DURATION, 0, target, [=, this](float value) {
            
            options->here.x = options->moveFrom.x*(LANDSCAPE_DIR_STEPS - value) + options->moveTo.x*value;
            options->here.y = options->moveFrom.y*(LANDSCAPE_DIR_STEPS - value) + options->moveTo.y*value;
            
            if ( value >= target ) {
                stopMoving();
                return;
            }
        
            f32 result = value / target ;
            options->movementAmount = result;
            
            options->generator->Build(options);
            UpdateLandscape();
        });
        
        this->runAction(EaseSineInOut::create(actionfloat));
        
        next_people->Initialise(c, c.looking);
        next_people->startFadeIn();
        
        current_people->startFadeOut();
        
        return true;
    }
    
    // why did move fail?
    
    
    Enable();
    
    return false;
    
}

void panel_look::stopRotating(LANDSCAPE_MOVEMENT type)
{
    options->isLooking = false;
    
    if(type == LM_ROTATE_LEFT)
    {
        LandscapePeople* temp = next_people1;
        next_people1 = next_people;
        next_people=current_people;
        current_people=temp;
    }

    else if(type == LM_ROTATE_RIGHT)
    {
        LandscapePeople* temp = prev_people1;
        prev_people1 = prev_people;
        prev_people=current_people;
        current_people=temp;
    }

    stopMovement(type);
}

void panel_look::stopMoving()
{
    options->isMoving = false;
    
    // swap info
    LandscapePeople* temp = next_people;
    next_people=current_people;
    current_people=temp;
    
    stopMovement(LM_MOVE_FORWARD);
    
}

void panel_look::stopMovement(LANDSCAPE_MOVEMENT type)
{
    next_people1->clear();
    next_people->clear();
    current_people->stopAnim();
    prev_people->clear();
    prev_people1->clear();
    
    setViewForCurrentCharacter();
    
    OnMovementComplete(type);
    
    Enable();
}

void panel_look::startInactivity()
{
    this->scheduleOnce( [&](float)
    {
        showInactivityHelp();
    }, NONE_ACTIVTY_DURATION, "NonActivityTimer" );
}

void panel_look::stopInactivity()
{
    unschedule("NonActivityTimer");
    cancelCompassTimer();
}

void panel_look::showInactivityHelp()
{
    static helpid_t help_options[] = { HELP_LOOKING_AROUND, HELP_MOVEMENT, HELP_SELECTING_CHARACTER, HELP_THINKING, HELP_CHOOSE };
    
    if ( isEnabled() ) {
        if ( !isHelpVisible() ) {
            for (auto & help_option : help_options) {
                if ( !showHelpWindow( help_option ) )
                    break;
            }
        }
    }
}

void panel_look::hideMenus()
{
    // TODO: Hide Menus
//    if ( actions )
//        actions->Collapse();
//    if ( debug )
//        debug->Collapse();
}


void panel_look::fadeIn ( rgb_t colour, f32 initialAlpha,  const MXVoidCallback& callback )
{
    if ( isNot(mr->settings->screentransitions) ) {
        if ( callback != nullptr ) {
            callback();
        }
        return;
    }
    
    auto rect = this->getBoundingBox();
    auto fade_panel = DrawNode::create();
    fade_panel->drawSolidRect(Vec2::ZERO, Vec2( rect.getMaxX(), rect.getMaxY()), Color4F(colour) );
    fade_panel->setOpacity(ALPHA(initialAlpha));
    fade_panel->setLocalZOrder(ZORDER_POPUP);
    addChild(fade_panel);
    
    auto callbackAction = CallFunc::create([=, this]() {
        if ( callback != nullptr ) {
            callback();
        }
        this->removeChild(fade_panel);
        //fade_panel = nullptr;
    });
    
    auto action = FadeOut::create(FADE_IN_SPEED);
    auto sequence = Sequence::createWithTwoActions(action, callbackAction);
    fade_panel->runAction(sequence);
    
}

void panel_look::fadeOut ( rgb_t colour, f32 initialAlpha,  const MXVoidCallback& callback )
{
    if ( isNot(mr->settings->screentransitions) ) {
        if ( callback != nullptr ) {
            callback();
        }
        return;
    }
    
    auto rect = this->getBoundingBox();
    auto fade_panel = DrawNode::create();
    fade_panel->drawSolidRect(Vec2::ZERO, Vec2( rect.getMaxX(), rect.getMaxY()), Color4F(colour) );
    fade_panel->setLocalZOrder(ZORDER_POPUP);
    fade_panel->setOpacity(ALPHA(initialAlpha));
    addChild(fade_panel);
    
    auto callbackAction = CallFunc::create([=, this]() {
        if ( callback != nullptr ) {
            callback();
        }
        this->removeChild(fade_panel);
        //fade_panel = nullptr;
    });
    
    auto action = FadeIn::create(FADE_OUT_SPEED);
    auto sequence = Sequence::createWithTwoActions(action, callbackAction);
    fade_panel->runAction(sequence);
}

void panel_look::OnShown()
{
    startInactivity();
    
    
    if ( variables::sv_days == 0) {
        if ( !showHelpWindow(HELP_DAY1))
            return;
        
        if ( !showHelpWindow(HELP_CHOICE_OF_GAMES))
            return;
    }
    
    if ( variables::sv_days == 1) {
        if ( !showHelpWindow(HELP_BATTLE))
            return;
#if defined(_LOM_)
        if ( !showHelpWindow(HELP_ICEFEAR))
            return;
#endif
    }
    if ( variables::sv_days == 2) {
        if ( !showHelpWindow(HELP_VICTORY))
            return;
    }
    
#if defined(_LOM_)
    character& c = TME_CurrentCharacter();
    if ( c.race == RA_FREE || c.race == RA_MORKIN )
        if ( !showHelpWindow(HELP_FREE))
            return;
    if ( c.race == RA_FEY )
        if ( !showHelpWindow(HELP_FEY))
            return;
    if ( c.race == RA_WISE )
        if ( !showHelpWindow(HELP_WISE))
            return;
#endif
}

void panel_look::OnActivate()
{
    uipanel::OnActivate();
    
    // make sure we are working with our character
    setObject(characterId);
    
    character&    c = TME_CurrentCharacter();
    
    stopInactivity();
    
    if ( Character_IsDead(c) ) {
        mr->showPage( MODE_THINK );
        return;
    }
    
    OnSetupIcons();
}

void panel_look::OnDeActivate()
{
    uipanel::OnDeActivate();
}

bool panel_look::OnKeyboardEvent( uikeyboardevent* event )
{
    if ( !event->isUp() )
        return false;
    
    if ( options->isMoving || options->isLooking )
        return false;
    
    //
    if ( event->getKey() >= KEYCODE(1) && event->getKey() <= KEYCODE(8) ) {
        if ( event->isShift() ) {
            auto dir = (mxdir_t)((u32)event->getKey() - (u32)KEYCODE(0) -1);
            return mr->look(dir);
        }
    }
    
    switch(event->getKey()){
        
        case KEYCODE(LEFT_ARROW):
        {
            startLookLeft();
            return true;
        }
        case KEYCODE(RIGHT_ARROW):
        {
            startLookRight();
            return true;
        }

        case KEYCODE(Q):
        case KEYCODE(UP_ARROW):
        {
            moveForward();
            return true;
        }
        case KEYCODE(DOWN_ARROW):
        {
            return true;
        }
        default:
            if ( i_command_window != nullptr ) {
                if( i_command_window->dispatchShortcutKey(event->getKey()) )
                    return true;
            }
            break;
    }
    return uipanel::OnKeyboardEvent(event);
}


void panel_look::OnNotification( Ref* sender )
{
    stopInactivity();
    
    auto button = dynamic_cast<Widget*>(sender);
    if ( button == nullptr )
        return;
    
    auto id = static_cast<layoutid_t>(button->getTag());
    
    hideMenus();
    
    if ( id >= ID_SELECT_CHAR ) {
        mxid selectId = id-ID_SELECT_CHAR;
        mr->selectCharacter(selectId);
        return;
    }
    
    switch ( id ) {
            
        case ID_HELP:
            showHelpPending();
            break;
            
        case ID_ACTIONS:
            i_command_window->show(nullptr);
            break;
            
        case ID_HOME:
        {
            AreYouSure(CLOSE_STORY_MSG, [&] {
                mr->closeStory();
            });
            
            break;
        }
            
        case ID_UNDO_DAWN:
        {
            AreYouSure(UNDO_DAWN_MSG, [&] {
                OnUndo(savemode_dawn);
            });
            
            return;
        }
            
        case ID_UNDO:
        {
            OnUndo(savemode_normal);
            return;
        }

        case ID_NIGHT:
        {
            if(is(mr->settings->night_confirm)) {
                AreYouSure(NIGHT_MSG, [&] {
                    mr->night();
                });
            }else{
                mr->night();
            }
            
            return;
        }
            
        case ID_MAP:
        {
            mr->panels->showMap();
            break;
        }
            
        case ID_SELECT_ALL:
        {
            mr->help->Shown(HELP_SELECTING_CHARACTER);
            mr->showPage( MODE_SELECT );
            break;
        }
            
        case ID_APPROACH:
        {
            if ( mr->approach() ) {
                return;
            }
        }
            
#if defined(_LOM_)
        case ID_SEEK:
        {
            if ( mr->seek() )
                return;
        }
            
        case ID_HIDE:
        {
            if ( mr->hide()) {
                return;
            }
            break;
        }
        
        case ID_UNHIDE:
        {
            if ( mr->unhide()) {
                return;
            }
            break;
        }
#endif
        case ID_DISMOUNT:
        {
            if ( mr->dismount()) {
                return;
            }
            break;
        }
            
        case ID_FIGHT:
        {
            if ( mr->fight() ) {
                return;
            }
            break;
        }

            
#if defined(_DDR_)
        case ID_GIVE:
        {
            if ( mr->give() )
                return;
            break;
        }
        case ID_TAKE:
        {
            if ( mr->take() )
                return;
            break;
        }
        case ID_USE:
        {
            if ( mr->use() )
                return;
            break;
        }
        case ID_REST:
        {
            if ( mr->rest() )
                return;
            break;
        }
#endif

        case ID_ENTER_TUNNEL:
        {
            if ( OnEnterTunnel() )
                return;
        }
            
        case ID_THINK:
        {
            if ( mr->think() )
                return;
        }
        
        case ID_THINK_PERSON:
        {
            char_data_t* d = static_cast<char_data_t*>(button->getUserData());
            mr->showPage(MODE_THINK_PERSON,d->id);
            return;
        }

        case ID_RECRUITMEN:
        {
            if ( mr->recruitMen() )
                return;
            break;
        }
        case ID_POSTMEN:
        {
            if ( mr->postMen() )
                return;
            break;
        }
            
        case ID_ATTACK:
        {
            if ( mr->attack() )
                return;
            break;
        }
            
        // check for character selection
        case ID_SHOW_LEADER:
        {
            auto data = static_cast<char_data_t*>(button->getUserData());
            if ( mr->selectCharacter(data->id) )
                return;
 
            break;
        }
            
        default:
            break;
    }
    
    hideMenus();
    
    TME_RefreshCurrentCharacter();
    TME_GetCharacterLocationInfo ( TME_CurrentCharacter() );
    uipanel::OnNotification(sender);
}

bool panel_look::OnUndo ( savemode_t mode )
{
    fadeOut(_clrBlack, 0.75f, [=, this](){
        mr->undo(mode);
        setObject( TME_CurrentCharacter().id );
        fadeIn(_clrBlack, 1.0f, [&]{
                // do nothing
        });
    });
    return true;
}


bool panel_look::OnEnterTunnel()
{
    hideMenus();
    fadeOut(_clrBlack, 0.75f, [&](){
        
        mr->enterTunnel();
        
        getCurrentLocationInfo();
        setViewForCurrentCharacter();
       // current_view->SetViewForCurrentCharacter();
        fadeIn(_clrBlack, 1.0f, [&](){
            showHelpWindow(HELP_TN_TUNNEL);
        });
    });
    
    
    return true;
}

bool panel_look::OnExitTunnel()
{
    hideMenus();
    fadeOut(_clrWhite, 0.0f, [&]() {
        OnMovementComplete(LM_MOVE_FORWARD);
        fadeIn(_clrBlack, 1.0f, [&](){
        });
    });
    return true;
}

void panel_look::OnSetupIcons()
{
    if ( i_command_window==nullptr )
        return;
    
    i_command_window->updateElements();
    
}

void panel_look::showCompass(Vec2 pos)
{
    stopInactivity();
    
#if defined(_MOUSE_ENABLED_)
    setCursor(MOUSE_NORMAL);
#endif

    compass = uicompass::create(this);
    compass->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    compass->setPosition(pos);
    compass->setSelected( current_info->looking );
    
    compass->onCancel = [&] {
        compass = nullptr;
    };

    compass->onOk = [&] {
        mr->look(compass->getSelected());
        compass = nullptr;
    };

    compass->Show(touchListener);
}

void panel_look::startCompassTimer(Vec2 pos)
{
    if (mr->settings->compass_delay == CF_COMPASS::OFF ) {
        return;
    }

    f32 compass_delay = compass_delays[(int)mr->settings->compass_delay];

    this->scheduleOnce( [&,pos](float) {
        showCompass(pos);
    }, compass_delay, "long_press" );
}

void panel_look::cancelCompassTimer()
{
    unschedule("long_press");
}


bool panel_look::OnMouseEvent( Touch* touch, Event* event, bool pressed )
{
    if(options->isLooking)
    {
        return false;
    }
    
    f32 MOUSE_MOVE_BLEED = 256;

    auto size = getContentSize();
    
    bool IsLeftMouseDown = false;
    bool IsLeftMouseUp = false;
    
    if ( event->getType() == Event::Type::MOUSE || event->getType() == Event::Type::TOUCH ) {
        if ( pressed ) {
            IsLeftMouseDown = true;
        } else {
            IsLeftMouseUp = true;
            cancelCompassTimer();
        }
    }
    
    auto position =  touch->getLocation();
    
    if ( mr->settings->nav_mode != CF_NAVIGATION::SWIPE || is(mr->settings->flipscreen) ) {
        
        f32 move_press_y = size.height - RES(MOUSE_MOVE_BLEED) ;
        f32 move_press_left = RES(MOUSE_MOVE_BLEED) ;
        f32 move_press_right = size.width - RES(MOUSE_MOVE_BLEED) ;
        
        // use full centre height if we are only pressing
        if ( mr->settings->nav_mode==CF_NAVIGATION::FULL)
            move_press_y = size.height * 0.75 ;
        
        else if ( mr->settings->nav_mode==CF_NAVIGATION::PRESS || is(mr->settings->flipscreen))
            move_press_y = 0;
        
        if ( IsLeftMouseDown ) {
            
            if ( mr->settings->nav_mode != CF_NAVIGATION::FULL || is(mr->settings->flipscreen)) {
                if ( position.y > move_press_y
                    && (position.x>move_press_left && position.x < move_press_right ) ) {
                    updateMovementIndicators(LM_MOVE_FORWARD);
                    return true;
                }
            }
            
            if ( position.x > move_press_right ) {
                updateMovementIndicators(LM_ROTATE_RIGHT);
                return true;
            } else if ( position.x < move_press_left ) {
                updateMovementIndicators(LM_ROTATE_LEFT);
                return true;
            }
            
        }
        
        if(IsLeftMouseDown) {
            startCompassTimer(position);
        }

        if ( IsLeftMouseUp  ) {
            
            if ( currentMovementIndicator == LM_NONE )
                return true;
            
            if ( mr->settings->nav_mode != CF_NAVIGATION::FULL || is(mr->settings->flipscreen)) {
                if ( position.y > move_press_y
                    && (position.x>move_press_left && position.x < move_press_right ) ) {
                    updateMovementIndicators(LM_NONE);
                    moveForward();
                    return true;
                }
            }
            
            if ( position.x > move_press_right ) {
                updateMovementIndicators(LM_NONE);
                startLookRight();
                return true;
            } else if ( position.x < move_press_left ) {
                updateMovementIndicators(LM_NONE);
                startLookLeft();
                return true;
            }
        }
    }
    
    return false;
}

void panel_look::setupMovementIndicators()
{
    if ( isNot(mr->settings->showmovementindicators) )
        return;
    
    f32 scale = 2.5f;
    
    movementIndicators[0] = Sprite::createWithSpriteFrameName("arrow_left");
    movementIndicators[0]->setScale(scale);
    movementIndicators[0]->setLocalZOrder(ZORDER_DEFAULT+1);
    uihelper::AddCenterLeft(this, movementIndicators[0], RES(48),RES(0));

    
    movementIndicators[1] = Sprite::createWithSpriteFrameName("arrow_right");
    movementIndicators[1]->setScale(scale);
    movementIndicators[1]->setLocalZOrder(ZORDER_DEFAULT+1);
    uihelper::AddCenterRight(this, movementIndicators[1], RES(48),RES(0));

    movementIndicators[2] = Sprite::createWithSpriteFrameName("arrow_up");
    movementIndicators[2]->setScale(scale);
    movementIndicators[2]->setLocalZOrder(ZORDER_DEFAULT+1);
    uihelper::AddTopCenter(this, movementIndicators[2],RES(0),RES(256));

    updateMovementIndicators(LM_NONE);
}

void panel_look::updateMovementIndicators(LANDSCAPE_MOVEMENT movement)
{
    bool drawArrows = movement != LM_NONE ;
    currentMovementIndicator = movement;
 
    if ( isNot(mr->settings->showmovementindicators) )
        return;

    for ( int ii=0; ii<NUMELE(movementIndicators); ii++ ) {
        movementIndicators[ii]->setVisible(drawArrows);
//        movementIndicators[ii]->setColor(_clrWhite);
        
        if ( drawArrows ) {
            if ( movement == (ii+1) ) {
                movementIndicators[ii]->setOpacity(ALPHA(1.0f));
            }else{
                movementIndicators[ii]->setOpacity(ALPHA(0.25f));
            }
        }
    }
}

bool panel_look::allowDragDownMove()
{
    return ( mr->settings->nav_mode != CF_NAVIGATION::PRESS);
}

bool panel_look::allowDragLook()
{
    if(current_info->tunnel) {
        return false;
    }

    CF_NAVIGATION value = mr->settings->nav_mode ;
    if ( value != CF_NAVIGATION::PRESS && value != CF_NAVIGATION::FULL && isNot(mr->settings->flipscreen) )
        return  true;
    return false;
}

void panel_look::OnSelectDrag(uidragevent* event)
{
    DragElement::OnSelectDrag(event);
}

void panel_look::OnStartDrag(uidragevent* event)
{
    DragElement::OnStartDrag(event);
    
    OnMovementComplete(LM_DRAG_START);

    startDragLookAmount = options->lookAmount;
}

void panel_look::OnStopDrag(uidragevent* event)
{
    DragElement::OnStopDrag(event);

    // vertical or horizontal
    auto size = getContentSize();
    Vec2 delta = drag_start - drag_stop;
    f32 dx = delta.x / size.width ;
    f32 dy = delta.y / size.height ;

    // GESTURE Move Forward
    if( allowDragDownMove()) {
        if( abs(dx) < 0.01 && dy > MINIMUM_VERTICAL_DRAG_GESTURE) {
            moveForward();
        }
    }

    // GESTURE Show map
    if( abs(dx) < 0.01 && dy < -MINIMUM_VERTICAL_DRAG_GESTURE) {
        mr->panels->showMap();
    }

    if ( allowDragLook() ) {
        lookPanoramaSnap(event);
        return;
    }

    
}

void panel_look::OnDrag(uidragevent* event)
{
    DragElement::OnDrag(event);
    
    if ( ! allowDragLook() )
        return;
    
    auto size = getContentSize();
    Vec2 delta = drag_start - drag_current;
    f32 dx = delta.x / size.width ;

    options->isLooking = true;
    options->isMoving=false;
    options->lookAmount = startDragLookAmount + ((LANDSCAPE_DIR_AMOUNT*2) * dx);
    
    UpdatePanningLandscape();
    parallaxCharacters();
}

void panel_look::UpdatePanningLandscape()
{
    options->generator->horizontalOffset = options->lookAmount;
    options->generator->landscapeScreenWidth = getContentSize().width ;
    current_view->RefreshPositions();
}

//
// we need to offset the overlay views
// by the required amount in the correct direction
// they are going to move 1024 pixels
// in the time it takes the main view to move 512 ( maybe 400 )
//
void panel_look::parallaxCharacters ()
{
    f32 distance = (startDragLookAmount - options->lookAmount) / LANDSCAPE_DIR_AMOUNT;

    f32 width = getContentSize().width;
    f32 movement = width * distance;
    
    prev_people1->setPositionX( 0 - width - width + movement );
    prev_people->setPositionX( 0 - width + movement );
    
    current_people->setPositionX( 0 + movement );
    next_people->setPositionX( 0 + width + movement );
    next_people1->setPositionX( 0 + width + width + movement );
    
}

//
// we need to snap the view to the nearest look direction
// we either snap forward or back
//
void panel_look::lookPanoramaSnap(uidragevent* event)
{
    f32 duration = TRANSITION_DURATION;
    f32 amount;

    f32 distanceDragged = event->position.x - event->startposition.x;
    f32 timeDragged = (f32)(event->time - event->starttime) / 1000.0f;
    f32 velocity = ABS(distanceDragged) / timeDragged;
    
    bool inertiaScroll = (timeDragged < 0.5f && velocity > 500)  && (ABS(distanceDragged) < (getContentSize().width/2));
    int locationAdjustment;
    
//    UIDEBUG("Time %f, Velocity %f, Distance %f, Inertia: %s",
//            timeDragged, velocity, distanceDragged,
//            inertiaScroll ? "TRUE":"FALSE");
        
    if(!inertiaScroll)
    {
        // normal magnetic scroll
        amount = (options->lookAmount / LANDSCAPE_DIR_AMOUNT);
        amount = ((s32)ROUNDFLOAT(amount)) * LANDSCAPE_DIR_AMOUNT ;
        locationAdjustment = ROUNDFLOAT(ABS((options->lookAmount-startDragLookAmount) / (f32)LANDSCAPE_DIR_AMOUNT) * SIGN(distanceDragged));
          
    }else{
        // Inertia force
        f32 dir = SIGN(distanceDragged);
        amount = (ABS(options->lookAmount-startDragLookAmount) > LANDSCAPE_DIR_AMOUNT/2) ? 2 : 1;
        locationAdjustment = (int)(amount*dir);
        amount = startDragLookAmount - ((f32)locationAdjustment*LANDSCAPE_DIR_AMOUNT);

        f32 distanceRemaining = ABS(amount-options->lookAmount);
        f32 howFar = (distanceRemaining/LANDSCAPE_DIR_AMOUNT);
        duration = duration * howFar;
    }
    
    //UIDEBUG("Panning: Direction=%d from %f to %f diff %f : Inertia=%s",
    //        options->currentDirection, options->lookAmount, amount, (options->lookAmount-amount), inertiaScroll ? "TRUE":"FALSE");
  
    //UIDEBUG("Panning: Current Direction=%d Adjustment %d Final Direction %d",
    //        options->currentDirection, locationAdjustment, options->currentDirection + locationAdjustment );


    auto actionfloat = ActionFloat::create(duration, options->lookAmount, amount, [=, this](float value) {
        options->isLooking = true;
        options->lookAmount =  value;
        UpdatePanningLandscape();
        parallaxCharacters();
    });
    
    runAction(Sequence::createWithTwoActions( actionfloat, CallFunc::create( [=, this] { stopDragging(locationAdjustment); } )));
}

//
// Reset the view after dragging and snap back
//
void panel_look::stopDragging(s32 adjustment)
{
    character&    c = TME_CurrentCharacter();

    auto dir = (mxdir_t) ((int)c.looking - adjustment);
    
    options->lookAmount = 0;
    options->isLooking = false;
    
    if ( c.looking != dir ) {
        Character_Look( c, dir );
    }
    stopRotating(LM_NONE);
}

#if defined(_MOUSE_ENABLED_)
bool panel_look::OnMouseMove( Vec2 position )
{
    if(compass!=nullptr) {
        return false;
    }

    f32 MOUSE_MOVE_BLEED = 256;

    auto size = getContentSize();
 
    f32 move_press_y = size.height - RES(MOUSE_MOVE_BLEED) ;
    f32 move_press_left = RES(MOUSE_MOVE_BLEED) ;
    f32 move_press_right = size.width - RES(MOUSE_MOVE_BLEED) ;

    // check mouse position
    
    MOUSE_CURSOR mode = MOUSE_NORMAL ;

    if ( (i_command_window==NULL || !i_command_window->isVisible())
         && (help_window==NULL || !help_window->isVisible())
         && !landscape_dragging ) {

        // check if pointer over any gadget   
        if ( childFromPoint(position) == nullptr ) {
        
        if ( mr->settings->nav_mode!=CF_NAVIGATION::SWIPE ) {

            // use full centre height if we are only pressing
            if ( mr->settings->nav_mode==CF_NAVIGATION::FULL)
                move_press_y = size.height * 0.75 ;
            else if ( mr->settings->nav_mode==CF_NAVIGATION::PRESS )
                move_press_y = size.height ;
            
            
            if ( mr->settings->nav_mode!=CF_NAVIGATION::FULL || is(mr->settings->flipscreen)) {
                if ( position.y > move_press_y && (position.x>move_press_left && position.x < move_press_right) ) {
                    mode = MOUSE_MOVE;
                }
            }
            
            if ( position.x > move_press_right ) {
                mode = MOUSE_LOOK_RIGHT ;
            } else if ( position.x < move_press_left ) {
                mode = MOUSE_LOOK_LEFT ;
            }
     
        }
        
        }

    }
    
    setCursor(mode);

    return true;
}
#endif
