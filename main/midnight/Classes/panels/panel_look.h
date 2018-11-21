#pragma once

#include "../ui/uipanel.h"
#include "../system/storymanager.h"

#include "../landscaping/LandscapeGenerator.h"
#include "../landscaping/LandscapeView.h"
#include "../landscaping/LandscapePeople.h"
#include "../ui/uihelper.h"
#include "../tme_interface.h"

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
    
    std::string     shield;
    std::string     person;
    std::string     face;
    std::string     name;
    std::string     locationtext;

    bool            tunnel;
    bool            lookingdowntunnel;

} locationinfo_t ;


class panel_look : public uipanel, uinotificationinterface
{

public:
    
    bool init() override;
    CREATE_FUNC(panel_look);
	
    virtual void setObject ( mxid c ) override;
   
    
protected:
    
    virtual void OnNotification( Ref* element ) override;
    
    void getCharacterInfo ( defaultexport::character_t& c, locationinfo_t* info);
    void getCurrentLocationInfo ( void );
    void setViewForCurrentCharacter ( void );
    void initKeyboard();
    
    bool moveForward ( void );

    bool startMoving();
    void stopMoving();
    bool startLookRight ( void );
    bool startLookLeft ( void );
    void stopRotating(LANDSCAPE_MOVEMENT type);
  
    void startInactivity();
    void stopInactivity();
    void showInactivityHelp();
    
    void delayedSave();

    void hideMenus ( void );
    void fadeIn ( rgb_t colour, f32 initialAlpha, MXVoidCallback callback );
    void fadeOut ( rgb_t colour, f32 initialAlpha, MXVoidCallback callback );

    
    void OnMovementComplete( /*uiview* sender, */ LANDSCAPE_MOVEMENT type );
    //void OnActionComplete( tagid_t tag );
    
    void OnShown( void ) override;
    void OnActivate( void ) override;
    void OnDeActivate( void ) override;

    void OnSetupIcons();
    void OnSetupFaces();
    
    void UpdateLandscape();
    
    // Actions and Commands
    bool OnUndo ( savemode_t mode );
#if defined(_DDR_)
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
    ui::ImageView*          imgShield;
    LayerColor*         layHeader;
    
    
    u32                 current_arrow;
    bool                draw_arrows;
    bool                landscape_dragging;
    savemode_t          undo_mode;
    
    // Actions and Commands
    uicommandwindow*    i_command_window;
    //DrawNode*          fade_panel;
    
    LandscapePeople*    people[3];
    LandscapePeople*    current_people;
    LandscapePeople*    next_people;
    LandscapePeople*    prev_people;
    
};







