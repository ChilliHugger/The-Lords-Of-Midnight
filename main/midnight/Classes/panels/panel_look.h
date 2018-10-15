#pragma once

#include "uipanel.h"
#include "../system/storymanager.h"

#include "../landscaping/LandscapeGenerator.h"
#include "../landscaping/LandscapeView.h"
#include "uihelper.h"


#include "tme_interface.h"

#define NONE_ACTIVTY_DURATION   30.0f

FORWARD_REFERENCE(uicommandwindow);

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

typedef struct {
    mxid            id;
    mxdir_t         looking;
    mxtime_t        time;
    tme::loc_t      location;
    
    //mximage*        shield;
    //mximage*        person;
    //mximage*        face;
    std::string     name;
    std::string     locationtext;

    BOOL            tunnel;

} locationinfo_t ;


class panel_look : public uipanel, uinotificationinterface
{

public:
    
    virtual bool init();
    CREATE_FUNC(panel_look);
	
    virtual void SetObject ( mxid c );
   
    
protected:
    
    virtual void OnNotification( Ref* element );
    
    void GetCharacterInfo ( defaultexport::character_t& c, locationinfo_t* info);
    void GetCurrentLocationInfo ( void );
    void SetViewForCurrentCharacter ( void );
    void InitKeyboard();
    
    bool moveForward ( void );

    bool StartMoving();
    void StopMoving();
    bool StartLookRight ( void );
    bool StartLookLeft ( void );
    void StopRotating(LANDSCAPE_MOVEMENT type);
  
    void StartInactivity();
    void StopInactivity();
    void ShowInactivityHelp();
    
    void delayedSave();

    void hideMenus ( void );
    void fadeIn ( rgb_t colour, f32 initialAlpha, MXVoidCallback callback );
    void fadeOut ( rgb_t colour, f32 initialAlpha, MXVoidCallback callback );

    
    void OnMovementComplete( /*uiview* sender, */ LANDSCAPE_MOVEMENT type );
    //void OnActionComplete( tagid_t tag );
    
    void OnShown( void );
    void OnActivate( void );
    void OnDeActivate( void );

    void OnSetupIcons();
    void OnSetupFaces();
    
    void UpdateLandscape();
    
    // Actions and Commands
    bool OnSelectCharacter(mxid id);
    bool OnChoose();    // ID_CHOOSE
    bool OnHome();      // ID_HOME
    bool OnUndoNight();
    bool OnUndoDawn();
    bool OnUndo();
    bool OnUndo ( savemode_t mode );
    bool OnNight();
    bool OnMap();
    bool OnSelect();
    bool OnApproach();
    bool OnSeek();
    bool OnHideUnhide();
    bool OnFight();
    bool OnThink();
    bool OnRecruitMen();
    bool OnPostMen();
    bool OnAttack();
#if defined(_DDR_)
    bool OnGive();
    bool OnTake();
    bool OnUse();
    bool OnRest();
    bool OnEnterTunnel();
    bool OnExitTunnel();
#endif

    
    
   
    
protected:
    mxid                characterId;
    
    ILandscape*         current_view;
    LandscapeView*      landscapeView;
    LandscapeView*      tunnelView;
    
    LandscapeOptions    options;
    //bool                isLooking;
    //bool                isMoving;
    
    locationinfo_t*     current_info;
    locationinfo_t*     follower_info;
    
    Label*              lblDescription;
    Label*              lblName;
    
    
    u32                 current_arrow;
    bool                draw_arrows;
    bool                landscape_dragging;
    savemode_t          undo_mode;
    
    // Actions and Commands
    uicommandwindow*    i_command_window;
    //DrawNode*          fade_panel;
};







