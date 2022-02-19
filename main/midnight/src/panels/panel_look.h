#pragma once

#include "../ui/uipanel.h"
#include "../system/storymanager.h"

#include "../landscaping/LandscapeGenerator.h"
#include "../landscaping/LandscapeView.h"
#include "../landscaping/LandscapePeople.h"
#include "../ui/uielement.h"
#include "../tme/tme_interface.h"

#define NONE_ACTIVTY_DURATION   30.0f

FORWARD_REFERENCE(uicommandwindow);
FORWARD_REFERENCE(uisinglelord);

typedef enum SNAP_MODE {
    sm_back_right=1,
    sm_forward_right=3,
} SNAP_MODE ;

typedef enum GESTURE_MODE {
    none=0,
    horizontal=1,
    vertical=2,
} GESTURE_MODE ;

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

typedef struct {
    mxid            id;
    mxdir_t         looking;
    mxtime_t        time;
    tme::loc_t      location;
    mxid            following;
    
    std::string     shield;
    std::string     person;
    std::string     face;
    std::string     name;
    std::string     locationtext;

    bool            tunnel;
    bool            lookingdowntunnel;
    bool            lookingouttunnel;

} locationinfo_t ;


class panel_look :
    public uipanel,
    chilli::ui::NotificationInterface,
    chilli::ui::DragElement
{
    using Touch = cocos2d::Touch;
    using Event = cocos2d::Event;
    using Sprite = cocos2d::Sprite;
    using ImageView = cocos2d::ui::ImageView;
    using Label = cocos2d::Label;
    using LayerColor = cocos2d::LayerColor;
    using Vec2 = cocos2d::Vec2;
    using uidragevent = chilli::ui::DragEvent;
    using Color3B = cocos2d::Color3B;
    using DrawNode = cocos2d::DrawNode;

public:
    virtual ~panel_look() override;
    
    bool init() override;
    CREATE_FUNC(panel_look);
    
    virtual void setObject ( mxid c ) override;
   
    
protected:
    panel_look();
    virtual void OnNotification( Ref* element ) override;
    
    static void getCharacterInfo ( character& c, locationinfo_t* info);
    void getCurrentLocationInfo ( void );
    void setViewForCurrentCharacter ( void );
    
    void addTouchListener();
    
    bool moveForward ( void );

    bool startMoving();
    void stopMoving();
    bool startLookRight ( void );
    bool startLookLeft ( void );
    void stopRotating(LANDSCAPE_MOVEMENT type);
    void stopMovement(LANDSCAPE_MOVEMENT type);

    void startInactivity();
    void stopInactivity();
    void showInactivityHelp();
    
    void delayedSave();

    void hideMenus ( void );
    void fadeIn ( Color3B colour, f32 initialAlpha, const MXVoidCallback& callback );
    void fadeOut ( Color3B colour, f32 initialAlpha, const MXVoidCallback& callback );

    
    void OnMovementComplete( /*uiview* sender, */ LANDSCAPE_MOVEMENT type );
    bool OnMouseEvent( Touch* touch, Event* event, bool pressed );
    bool OnKeyboardEvent( uikeyboardevent* event ) override;

    
    void OnShown( void ) override;
    void OnActivate( void ) override;
    void OnDeActivate( void ) override;

    void OnSetupIcons();
    void setupLeaderButton();
    
    void UpdateLandscape();
    void UpdatePanningLandscape();
    
    // Actions and Commands
    bool OnUndo ( savemode_t mode );
#if defined(_DDR_)
    bool OnEnterTunnel();
    bool OnExitTunnel();
#endif
   
    void setupMovementIndicators();
    void updateMovementIndicators(LANDSCAPE_MOVEMENT movement);
    
    // dragging
    void parallaxCharacters ( void );
    void OnSelectDrag(uidragevent* event) override;
    void OnDrag(uidragevent* event) override;
    void OnStopDrag(uidragevent* event) override;
    void OnStartDrag(uidragevent* event) override;
    bool allowDragDownMove();
    bool allowDragLook();
    void lookPanoramaSnap(uidragevent* event);
    void stopDragging(s32 adjustment);


protected:
    mxid                characterId{};
    
    ILandscape*         current_view;
    
    LandscapeOptions    options;
    //bool                isLooking;
    //bool                isMoving;
    
    locationinfo_t*     current_info;
    
    Label*              lblDescription;
    Label*              lblName;
    ImageView*          imgShield;
    LayerColor*         layHeader;
    ImageView*          imgHeader{};
    uisinglelord*       following{};
    DrawNode*           gradientL{};
    DrawNode*           gradientR{};
    
    LANDSCAPE_MOVEMENT  currentMovementIndicator;
 
    // Actions and Commands
    uicommandwindow*    i_command_window;
  
    LandscapePeople*    people[5];

    LandscapePeople*    prev_people1;
    LandscapePeople*    prev_people;
    LandscapePeople*    current_people;
    LandscapePeople*    next_people;
    LandscapePeople*    next_people1;
 
    Sprite*             movementIndicators[3];
    
    // draggin
    //f32                 dragged;
    bool                landscape_dragging;
    u64                 mouse_down_time{};
    bool                mouse_down{};
    Vec2                mouse_down_pos;
    Vec2                mouse_last_position;

    f32                 startDragLookAmount;
    
};
