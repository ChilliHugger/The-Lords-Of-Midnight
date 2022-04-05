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
      tutorial(true)
    , autofight(false)
    , autounhide(false)
    , nav_mode(CF_NAV_BOTH)
    , screentransitions(true)
    , flipscreen(false)
    , compass_delay(CF_COMPASS_NORMAL)
    , think_paging_mode(CF_THINK_SWIPE)
    , night_confirm(true)
    , night_display_fast(false)
    , night_battle_full(true)
    , screen_mode(CF_FULLSCREEN)
    , keyboard_mode(CF_KEYBOARD_CLASSIC)
    , fullscreensupported(true)
    , cursor_size(CF_CURSOR_MEDIUM)
{
    
#if defined(_OS_DESKTOP_)
    showmovementindicators=FALSE;
#else
    showmovementindicators=TRUE;
#endif
    
#if defined(ADVERT_FREQUENCY)
    advert_screen_count=ADVERT_FREQUENCY-4;
#else
    advert_screen_count=0;
#endif
    
#if defined(_OS_IOS_) || defined(_OS_OSX_)
    novella_pdf=false;
#else
    novella_pdf=true;
#endif
    
//#ifdef _OS_DESKTOP_
//    nav_mode=CF_NAV_PRESS;
//#endif
    
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

BOOL settingsmanager::Save ( void )
{
    auto filename = StringExtensions::Format("%s/%s", mr->getWritablePath().c_str(), CONFIG_FILENAME );
    
    chilli::os::file* pFile = new chilli::os::file ( filename.c_str(), chilli::os::file::modeReadWrite|chilli::os::file::modeCreate );
    if ( pFile == NULL || !pFile->IsOpen() ) {
        if ( pFile ) delete pFile;
        return FALSE;
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

    ar.Close();

    SAFEDELETE ( pFile );
    
    return TRUE;
}

BOOL settingsmanager::Load ( void )
{
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
    ar >> tutorial ;

    if ( version>=2 ) {
        ar >> autofight;
        ar >> autounhide;
        ar >> showmovementindicators;
        ar >> temp; nav_mode = (CONFIG_NAV_MODE) temp;
        ar >> screentransitions ;
    }

    if ( version>=3 ) {
        ar >> temp; compass_delay = (CONFIG_COMPASS_DELAY) temp;
        ar >> temp; think_paging_mode = (CONFIG_THINK_PAGING) temp;
        ar >> night_display_fast ;
        ar >> night_battle_full ;
    }

    if ( version>=4 ) {
        ar >> temp; // Compass Feedback - Deprecated
    }

    if ( version>=5 ) {
        ar >> temp; screen_mode = (CONFIG_SCREEN_MODE) temp;
    }

    if ( version >= 6 ) {
        ar >> temp; keyboard_mode = (CONFIG_KEYBOARD_MODE)temp;
    }

    if ( version >= 7 ) {
        ar >> advert_screen_count ;
        ar >> novella_pdf ;
    }

    if ( version >= 8 ) {
        ar >> flipscreen ;
    }
    
    if ( version >= 9 ) {
        ar >> night_confirm;
    }

    if ( version >= 10 ) {
        ar >> temp; cursor_size = (CONFIG_CURSOR_SIZE) temp;
    }

    ar.Close();

    SAFEDELETE ( pFile );

    return TRUE;
}

