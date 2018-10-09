#pragma once

#include "uipanel.h"
#include "../system/storymanager.h"

#include "../landscaping/LandscapeGenerator.h"
#include "../landscaping/LandscapeView.h"
#include "tme_interface.h"

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
    std::string            name;
    std::string            locationtext;
#if defined(_DDR_)
    BOOL            tunnel;
#endif
} locationinfo_t ;

// TODO: Add SetCharacter Interface

class panel_look : public uipanel
{

public:
    
    virtual bool init();
    CREATE_FUNC(panel_look);
	
    void SetCharacter ( mxid c );
    
    
protected:
    
    void GetCharacterInfo ( defaultexport::character_t& c, locationinfo_t* info);
    void GetCurrentLocationInfo ( void );
    void SetViewForCurrentCharacter ( void );
    void InitKeyboard();
    
    bool StartMoving();
    void StopMoving();
    bool StartLookRight ( void );
    bool StartLookLeft ( void );
    void StopRotating(LANDSCAPE_MOVEMENT type);
  
    void OnMovementComplete( /*uiview* sender, */ LANDSCAPE_MOVEMENT type );
    
    void UpdateLandscape();
    
protected:
    mxid                characterId;
    LandscapeView*      landscapeView;
    LandscapeOptions    options;
    //bool                isLooking;
    //bool                isMoving;
    
    locationinfo_t*    current_info;
    locationinfo_t*    follower_info;
    
    Label*              lblDescription;
    Label*              lblName;
    
};







