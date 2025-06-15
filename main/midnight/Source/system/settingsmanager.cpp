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
      approach_mode(CF_APPROACH::SWAP)
    , autoapproach(TOGGLE::OFF)
    , autofight(TOGGLE::OFF)
    , autounhide(TOGGLE::OFF)
#if defined(_LOM_)
    , autoseek(TOGGLE::OFF)
#else
    , autoseek(TOGGLE::ON)
#endif
    , compass_delay(CF_COMPASS::NORMAL)
    , current_scenario(CF_SCENARIO::DEFAULT)
    , cursor_size(CF_CURSOR::MEDIUM)
    , dedication_screen_count(0)
    , screentransitions(TOGGLE::ON)
    , fey_recruit_mode(CF_FEY_RECRUIT::ON)
    , firsttime(true)
    , flipscreen(TOGGLE::OFF)
    , fullscreensupported(true)
    , game_difficulty(DF_NORMAL)
    , keyboard_mode(CF_KEYBOARD::CLASSIC)
    , movement_type(CF_MOVEMENT::ORIGINAL)
    , nav_mode(CF_NAVIGATION::BOTH)
    , night_battle_full(TOGGLE::ON)
    , night_confirm(TOGGLE::ON)
    , night_display_fast(TOGGLE::OFF)
    , screen_mode(CF_SCREEN::FULL)
    , think_paging_mode(CF_THINK_PAGING::SWIPE)
    , tutorial(TOGGLE::ON)
{
    
#if defined(_OS_DESKTOP_)
    showmovementindicators=TOGGLE::OFF;
#else
    showmovementindicators=TOGGLE::ON;
#endif
    
#if defined(ADVERT_FREQUENCY)
    advert_screen_count=ADVERT_FREQUENCY-4;
#else
    advert_screen_count=0;
#endif
    
#if defined(_OS_IOS_) || defined(_OS_OSX_)
    novella_pdf=TOGGLE::OFF;
#else
    novella_pdf=TOGGLE::ON;
#endif
    
//#ifdef _OS_DESKTOP_
//    nav_mode=PRESS;
//#endif
    game_rules.Clear();
    game_rules.Set(RF_NONE);
}


settingsmanager::~settingsmanager()
{
}

bool settingsmanager::bumpAdvert()
{
    bool show = false;

    if ( CONFIG(skip_adverts) ) {
        return false;
    }
    
    show = advert_screen_count%ADVERT_FREQUENCY==(ADVERT_FREQUENCY-1);

    advert_screen_count++;

    Save();
    
    return show;
}

bool settingsmanager::bumpDedication()
{
    bool show = false;

    if ( CONFIG(skip_dedication) ) {
        return false;
    }
    
    show = dedication_screen_count%DEDICATION_FREQUENCY==0;

    dedication_screen_count++;

    Save();
    
    return show;
}

bool settingsmanager::Save ( void )
{
#define FROM_ENUM(x) (int)x
#define FROM_TOGGLE(x) FROM_ENUM(x)

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
    ar << FROM_TOGGLE(tutorial) ;

    // version 2
    ar << FROM_TOGGLE(autofight);
    ar << FROM_TOGGLE(autounhide);
    ar << FROM_TOGGLE(showmovementindicators);
    ar << FROM_ENUM(nav_mode) ;
    ar << FROM_TOGGLE(screentransitions) ;

    // version 3
    ar << FROM_ENUM(compass_delay) ;
    ar << FROM_ENUM(think_paging_mode) ;
    ar << FROM_TOGGLE(night_display_fast) ;
    ar << FROM_TOGGLE(night_battle_full) ;

    // version 4
    ar << (int)0 ;

    // version 5
    ar << FROM_ENUM(screen_mode) ;

    // version 6
    ar << FROM_ENUM(keyboard_mode);

    // version 7
    ar << advert_screen_count;
    ar << FROM_TOGGLE(novella_pdf) ;
    
    // version 8
    ar << FROM_TOGGLE(flipscreen);
    
    // version 9
    ar << FROM_TOGGLE(night_confirm);
    
    // version 10
    ar << FROM_ENUM(cursor_size);
    
    // version 11
    ar << game_rules;
    
    // version 12
    ar << FROM_ENUM(game_difficulty);
    
    // version 13
    ar << FROM_TOGGLE(autoseek);
    ar << FROM_TOGGLE(autoapproach);
    ar << FROM_ENUM(approach_mode);
    
    // version 14
    ar << FROM_ENUM(current_scenario);
    
    // version 15
    ar << dedication_screen_count;

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

    firsttime = false;

    archive ar (pFile, archive::load | archive::bNoFlushOnDelete);

    u32 version = CONFIG_VERSION;
    int temp;

    ar >> version ;
    
    ar >> TO_TOGGLE( tutorial ) ;

    if ( version >= 2 ) {
        ar >> TO_TOGGLE(autofight);
        ar >> TO_TOGGLE(autounhide);
        ar >> TO_TOGGLE(showmovementindicators);
        ar >> TO_ENUM(nav_mode, CF_NAVIGATION) ;
        ar >> TO_TOGGLE(screentransitions);
    }

    if ( version >= 3 ) {
        ar >> TO_ENUM(compass_delay, CF_COMPASS);
        ar >> TO_ENUM(think_paging_mode, CF_THINK_PAGING);
        ar >> TO_TOGGLE(night_display_fast);
        ar >> TO_TOGGLE(night_battle_full);
    }

    if ( version >= 4 ) {
        ar >> temp; // Compass Feedback - Deprecated
    }

    if ( version >= 5 ) {
        ar >> TO_ENUM(screen_mode, CF_SCREEN);
    }

    if ( version >= 6 ) {
        ar >> TO_ENUM(keyboard_mode, CF_KEYBOARD);
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
        ar >> TO_ENUM(cursor_size, CF_CURSOR);
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
        ar >> TO_ENUM(approach_mode, CF_APPROACH);
    }

    if ( version >= 14 ) {
        ar >> TO_ENUM(current_scenario, CF_SCENARIO);
    }
    
    if ( version >= 15 ) {
        ar >> dedication_screen_count;
    }

    ar.Close();

    SAFEDELETE ( pFile );

    return TRUE;
}

