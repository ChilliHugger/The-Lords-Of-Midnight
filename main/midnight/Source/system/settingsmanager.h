//
//  config
//  frontend
//
//  Created by Chris Wild on 24/04/2012.
//  Copyright (c) 2012 Chilli Hugger. All rights reserved.
//

#pragma once

#include "ringcontroller.h"
#include "../tme/tme_interface.h"


#define OLD_SAVE_GAME_VERSION           12
#define NEW_SAVE_GAME_VERSION           13

#define FRONTEND_SAVE_GAME_VERSION      20
#define CONFIG_FILENAME                 "settings.cfg"
#define CONFIG_VERSION                  15

#ifndef MX_DEBUG
    #define _SHOW_SPLASH_
    #define _SHOW_DEDICATION_
    #define ADVERT_FREQUENCY                16
    #define DEDICATION_FREQUENCY            8
#else
    #define _SHOW_SPLASH_
    #define ADVERT_FREQUENCY                16
    #define DEDICATION_FREQUENCY            8
    #define _USE_DEBUG_MENU_
    #define _SHOW_GAME_VERSION_
    //#define _DEBUG_LANDSCAPE_
#endif

//#define _TME_CHEAT_MODE_

#if defined(_LOM_)
    #define PRE_RENDERED_LORD_NAMES
#endif


#if defined(_LOM_)
#define _SCENARIO_SELECTOR_
#endif

enum class CF_NAVIGATION : int
{
    BOTH    = 0,
    PRESS   = 1,
    SWIPE   = 2,
    FULL    = 3,
};

enum class CF_COMPASS : int
{
    OFF     = 0,
    NORMAL  = 1,
    SHORT   = 2,
    LONG    = 3,
};

enum class CF_THINK_PAGING : int
{
    SWIPE   = 0,
    PRESS   = 1,
};

enum class CF_SCREEN : int
{
    FULL    = 0,
    SMALL   = 1,
    MEDIUM  = 2,
    LARGE   = 3,
};

enum class CF_KEYBOARD : int
{
    CLASSIC = 0,
    NEW     = 1,
};

enum class CF_CURSOR : int
{
    SMALL   = 0,
    MEDIUM  = 1,
    LARGE   = 2,
};

enum class CF_MOVEMENT : int
{
    ORIGINAL= 0,
    INTENDED= 1,
    C64     = 2,
};

enum class CF_FEY_RECRUIT : int
{
    ON      = 0,
    OFF     = 1,
    NOVEL   = 2,
};

enum class CF_APPROACH
{
    SWAP    = 0,
    STAY    = 1
};

enum class CF_SCENARIO
{
    DEFAULT = 0,
    NOVEL   = 1
};

enum class TOGGLE : int
{
    OFF     = 0,
    ON      = 1
};

inline bool is(TOGGLE value) {
    return (value == TOGGLE::ON);
}

inline bool isNot(TOGGLE value) {
    return (value == TOGGLE::OFF);
}

class settingsmanager : public ringcontroller
{
    
public:
    settingsmanager();
    virtual ~settingsmanager();

    bool Save ( void );
    bool Load ( void );
    bool bumpAdvert();
    bool bumpDedication();

public:
    // not stored
    bool                    fullscreensupported;
    bool                    firsttime;
    
    // Game Rules save in game_rules
    CF_MOVEMENT             movement_type;
    CF_FEY_RECRUIT          fey_recruit_mode;
    
    // version 1
    TOGGLE                  tutorial;
    
    // version 2
    TOGGLE                  autofight;
    TOGGLE                  autounhide;
    TOGGLE                  showmovementindicators;
    TOGGLE                  screentransitions;
    CF_NAVIGATION           nav_mode;
    
    // version 3
    CF_COMPASS              compass_delay;
    CF_THINK_PAGING         think_paging_mode;
    TOGGLE                  night_display_fast;
    TOGGLE                  night_battle_full;

    // version 4
    //CF_COMPASS_FEEDBACK   compass_feedback;
    
    // version 5
    CF_SCREEN               screen_mode;
    
    // version 6
    CF_KEYBOARD             keyboard_mode;
    
    // version 7
    int                     advert_screen_count;
    TOGGLE                  novella_pdf ;
    
    // Version 8
    TOGGLE                  flipscreen;
    
    // Version 9
    TOGGLE                  night_confirm;
    
    // Version 10
    CF_CURSOR               cursor_size;
    
    // version 11
    eflags<RULEFLAGS,u64>   game_rules;

    // version 12
    mxdifficulty_t          game_difficulty;
    
    // version 13
    TOGGLE                  autoseek;
    TOGGLE                  autoapproach;
    CF_APPROACH             approach_mode;
    
    // version 14
    CF_SCENARIO             current_scenario;
    
    // version 15
    int                     dedication_screen_count;
};
