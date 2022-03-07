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
#define CONFIG_VERSION                  9

#ifndef MX_DEBUG
    #define _SHOW_SPLASH_
    #define _SHOW_DEDICATION_
    #define ADVERT_FREQUENCY                16
#else
    #define _SHOW_SPLASH_
    #define ADVERT_FREQUENCY                16
    #define _USE_DEBUG_MENU_
    #define _SHOW_GAME_VERSION_
    //#define _DEBUG_LANDSCAPE_
#endif

//#define _TME_CHEAT_MODE_

#if defined(_LOM_)
    #define PRE_RENDERED_LORD_NAMES
#endif

enum CONFIG_NAV_MODE
{
    CF_NAV_BOTH=0,
    CF_NAV_PRESS=1,
    CF_NAV_SWIPE=2,
    CF_NAV_SWIPE_MOVE_PRESS_LOOK=3,
};

enum CONFIG_COMPASS_DELAY
{
    CF_COMPASS_OFF=0,
    CF_COMPASS_NORMAL=1,
    CF_COMPASS_SHORT=2,
    CF_COMPASS_LONG=3,
};

enum CONFIG_COMPASS_FEEDBACK
{
    CF_COMPASS_LOW=1,
    CF_COMPASS_MEDIUM=2,
    CF_COMPASS_HIGH=3,
};

enum CONFIG_THINK_PAGING
{
    CF_THINK_SWIPE=0,
    CF_THINK_PRESS=1,
};

enum CONFIG_SCREEN_MODE
{
    CF_FULLSCREEN=0,
    CF_WINDOW_SMALL=1,
    CF_WINDOW_MEDIUM=2,
    CF_WINDOW_LARGE=3,
};

enum CONFIG_KEYBOARD_MODE
{
    CF_KEYBOARD_CLASSIC=0,
    CF_KEYBOARD_NEW=1,
};

class settingsmanager : public ringcontroller
{
    
public:
    settingsmanager();
    virtual ~settingsmanager();

    BOOL Save ( void );
    BOOL Load ( void );
    
    bool bumpAdvert();

public:    
    // not stored
    BOOL    fullscreensupported;
    
    
    // version 1
    BOOL    tutorial;
    
    // version 2
    BOOL    autofight;
    BOOL    autounhide;
    BOOL    showmovementindicators;
    BOOL    screentransitions;
    int     nav_mode;
    
    // version 3
    int     compass_delay;
    int     think_paging_mode;
    BOOL    night_display_fast;
    BOOL    night_battle_full;

    // version 4
    int     compass_feedback;
    
    // version 5
    CONFIG_SCREEN_MODE     screen_mode;
    
    // version 6
    int     keyboard_mode;
    
    // version 7
    int     advert_screen_count;
    BOOL    novella_pdf ;
    
    // Version 8
    BOOL    flipscreen;
    
    // Version 9
    BOOL    night_confirm;
};
