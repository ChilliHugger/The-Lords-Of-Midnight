//
//  config.cpp
//  frontend
//
//  Created by Chris Wild on 24/04/2012.
//  Copyright (c) 2012 Chilli Hugger. All rights reserved.
//

#include "settingsmanager.h"
#include "configmanager.h"
#include "moonring.h"

using namespace chilli::lib ;

settingsmanager::settingsmanager() :
      tutorial(ON)
    , autofight(OFF)
    , autounhide(OFF)
    , nav_mode(CF_NAV_BOTH)
    , screentransitions(ON)
    , flipscreen(OFF)
    , compass_delay(CF_COMPASS_NORMAL)
    , think_paging_mode(CF_THINK_SWIPE)
    , night_confirm(ON)
    , night_display_fast(OFF)
    , night_battle_full(ON)
    , screen_mode(CF_FULLSCREEN)
    , keyboard_mode(CF_KEYBOARD_CLASSIC)
    , fullscreensupported(ON)
    , cursor_size(CF_CURSOR_MEDIUM)
    , game_difficulty(DF_NORMAL)
    , movement_type(CF_MOVEMENT_ORIGINAL)
#if defined(_LOM_)
    , autoseek(OFF)
#else
    , autoseek(ON)
#endif
    , autoapproach(OFF)
    , approach_mode(CF_APPROACH_SWAP)
{
    
#if defined(_OS_DESKTOP_)
    showmovementindicators=OFF;
#else
    showmovementindicators=ON;
#endif
    
#if defined(ADVERT_FREQUENCY)
    advert_screen_count=ADVERT_FREQUENCY-4;
#else
    advert_screen_count=0;
#endif
    
#if defined(_OS_IOS_) || defined(_OS_OSX_)
    novella_pdf=OFF;
#else
    novella_pdf=ON;
#endif
    
//#ifdef _OS_DESKTOP_
//    nav_mode=CF_NAV_PRESS;
//#endif
    game_rules.Clear();
    game_rules.Set(RF_NONE);
}


settingsmanager::~settingsmanager()
{
}

bool settingsmanager::bumpAdvert()
{
    bool showAdvert = false;

    if ( mr->config->skip_adverts ) {
        return false;
    }
    
    showAdvert = advert_screen_count%ADVERT_FREQUENCY==(ADVERT_FREQUENCY-1);

    advert_screen_count++;

    Save();
    
    return showAdvert;
}

bool settingsmanager::Save ( void )
{
    auto filename = StringExtensions::Format("%s/%s", mr->getWritablePath().c_str(), CONFIG_FILENAME );
    
    chilli::os::file* pFile = new chilli::os::file ( filename.c_str(), chilli::os::file::modeReadWrite|chilli::os::file::modeCreate );
    if ( pFile == NULL || !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        return false;
    }

    chilli::lib::archive ar (pFile, archive::store | archive::bNoFlushOnDelete );

    u32 version = CONFIG_VERSION ;

    // version 1
    ar << version ;
    ar << tutorial ;

    // version 2
    ar << autofight;
    ar << autounhide;
    ar << showmovementindicators;
    ar << nav_mode ;
    ar << screentransitions ;

    // version 3
    ar << compass_delay ;
    ar << think_paging_mode ;
    ar << night_display_fast ;
    ar << night_battle_full ;

    // version 4
    ar << (int)0 ;

    // version 5
    ar << screen_mode ;

    // version 6
    ar << keyboard_mode;

    // version 7
    ar << advert_screen_count;
    ar << novella_pdf ;
    
    // version 8
    ar << flipscreen;
    
    // version 9
    ar << night_confirm;
    
    // version 10
    ar << cursor_size;
    
    // version 11
    ar << game_rules;
    
    // version 12
    ar << game_difficulty;
    
    // version 13
    ar << autoseek;
    ar << autoapproach;
    ar << approach_mode;

    ar.Close();

    SAFEDELETE ( pFile );
    
    return TRUE;
}



bool settingsmanager::Load ( void )
{
#define TO_ENUM(x,y) temp; x = (y)temp
#define TO_TOGGLE(x) TO_ENUM(x,TOGGLE)

    auto filename = StringExtensions::Format("%s/%s", mr->getWritablePath().c_str(), CONFIG_FILENAME );
    
    chilli::os::file* pFile = new chilli::os::file ( filename.c_str(), chilli::os::file::modeRead );
    if ( !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        return FALSE;
    }

    archive ar (pFile, archive::load | archive::bNoFlushOnDelete);

    u32 version=CONFIG_VERSION;
    int temp;

    ar >> version ;
    
    ar >> TO_TOGGLE( tutorial ) ;

    if ( version>=2 ) {
        ar >> TO_TOGGLE(autofight);
        ar >> TO_TOGGLE(autounhide);
        ar >> TO_TOGGLE(showmovementindicators);
        ar >> TO_ENUM(nav_mode,CONFIG_NAV_MODE) ;
        ar >> TO_TOGGLE(screentransitions);
    }

    if ( version>=3 ) {
        ar >> TO_ENUM(compass_delay,CONFIG_COMPASS_DELAY);
        ar >> TO_ENUM(think_paging_mode,CONFIG_THINK_PAGING);
        ar >> TO_TOGGLE(night_display_fast);
        ar >> TO_TOGGLE(night_battle_full);
    }

    if ( version>=4 ) {
        ar >> temp; // Compass Feedback - Deprecated
    }

    if ( version>=5 ) {
        ar >> TO_ENUM(screen_mode,CONFIG_SCREEN_MODE);
    }

    if ( version >= 6 ) {
        ar >> TO_ENUM(keyboard_mode,CONFIG_KEYBOARD_MODE);
    }

    if ( version >= 7 ) {
        ar >> advert_screen_count ;
        ar >> TO_TOGGLE(novella_pdf);
    }

    if ( version >= 8 ) {
        ar >> TO_TOGGLE(flipscreen);
    }
    
    if ( version >= 9 ) {
        ar >> TO_TOGGLE(night_confirm);
    }

    if ( version >= 10 ) {
        ar >> TO_ENUM(cursor_size,CONFIG_CURSOR_SIZE);
    }

    if ( version >= 11 ) {
        ar >> game_rules;
    }
    
    if ( version >= 12 ) {
        ar >> TO_ENUM(game_difficulty,mxdifficulty_t);
    }

    if ( version >= 13 ) {
        ar >> TO_TOGGLE(autoseek);
        ar >> TO_TOGGLE(autoapproach);
        ar >> TO_ENUM(approach_mode,CONFIG_APPROACH_MODE);
    }

    ar.Close();

    SAFEDELETE ( pFile );

    return TRUE;
}

