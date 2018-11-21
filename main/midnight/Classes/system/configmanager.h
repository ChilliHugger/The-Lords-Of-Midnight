//
//  config
//  frontend
//
//  Created by Chris Wild on 24/04/2012.
//  Copyright (c) 2012 Chilli Hugger. All rights reserved.
//

#pragma once

#include "../tme_interface.h"
#include "ringcontroller.h"

#define SAVE_GAME_VERSION               1
#define FRONTEND_SAVE_GAME_VERSION      13
#define CONFIG_FILENAME                 "settings.cfg"
#define CONFIG_VERSION                  7

//#if defined (_DDR_)
    #define _DAY_NIGHT_SHADER_
//#endif

#ifndef MX_DEBUG
    #define _SHOW_SPLASH_
    #define _SHOW_DEDICATION_
    #define ADVERT_FREQUENCY            16
    //#define _USE_DEBUG_MENU_
    //#define _SHOW_GAME_VERSION_
    #define _DEBUG_UNDO_HISTORY_        1
#else
    #define _SHOW_SPLASH_
    //#define _SHOW_DEDICATION_
    #define ADVERT_FREQUENCY            5
    #define _USE_DEBUG_MENU_
    #define _SHOW_GAME_VERSION_
    #define _DEBUG_UNDO_HISTORY_        10
    //#define _DEBUG_LANDSCAPE_
#endif

#define _TME_CHEAT_MODE_
//#define _TME_DEMO_MODE_

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

enum select_filters {
    filter_show_dawn    = MXBIT(0),
    filter_show_night   = MXBIT(1),
    filter_show_battle  = MXBIT(2),
    filter_show_dead    = MXBIT(3),
    filter_show_current = MXBIT(4),
#if defined(_DDR_)
    filter_show_intunnel= MXBIT(5),
#endif
};

enum map_filters {
    filter_show_critters    = MXBIT(0),
    filter_centre_char      = MXBIT(1),
    filter_overview_map     = MXBIT(2),
#if defined(_DDR_)
    filter_show_tunnels     = MXBIT(3),
#endif
};


class configmanager : public ringcontroller
{
    
public:
    configmanager();
    virtual ~configmanager();

    BOOL Save ( void );
    BOOL Load ( void );

    
    bool bumpAdvert();
    void setGameDefaults();

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
    int     screen_mode;
    
    // version 6
    int     keyboard_mode;
    
    // version 7
    int     advert_screen_count;
    BOOL    novella_pdf ;
    
    
    // game specific
    flags32 map_filters;
    flags32 select_filters;
    point   oldoffset;
    f32     mapscale;
    f32     lastmapscale;

    
    
};
