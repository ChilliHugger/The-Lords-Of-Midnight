//
//  config.cpp
//  frontend
//
//  Created by Chris Wild on 24/04/2012.
//  Copyright (c) 2012 Chilli Hugger. All rights reserved.
//

#include "config.h"

//#include "ui.h"
//#include "globals.h"

using namespace chilli::lib ;

config::config()
{
    tutorial=TRUE;
    autofight=FALSE;
    autounhide=FALSE;
    nav_mode=CF_NAV_BOTH;
    screentransitions=TRUE;
 
    compass_delay = CF_COMPASS_NORMAL ;
    compass_feedback = CF_COMPASS_MEDIUM ;
    think_paging_mode = CF_THINK_SWIPE ;
    
    night_display_fast = FALSE ;
    night_battle_full = TRUE;
    
    screen_mode = CF_FULLSCREEN;
    keyboard_mode = CF_KEYBOARD_CLASSIC;
    fullscreensupported=TRUE;
    
#if defined(_OS_DESKTOP_)
    showmovementindicators=FALSE;
#else
    showmovementindicators=TRUE;
#endif
    setGameDefaults();
    
    advert_screen_count=ADVERT_FREQUENCY-4;
    
#if defined(_OS_IOS_) || defined(_OS_OSX_)
    novella_pdf=false;
#else
    novella_pdf=true;
#endif
    
//#ifdef _OS_DESKTOP_
//    nav_mode=CF_NAV_PRESS;
//#endif
    
}


config::~config()
{
}

void config::setGameDefaults ()
{
    map_filters.Set(0xffffffff);
    select_filters.Set(0xffffffff);
    map_filters.Reset(filter_show_critters);

    oldoffset = point(0,0);
    mapscale=1.0f;
    lastmapscale=0.5f;

}



BOOL config::Save ( void )
{
	chilli::os::file* pFile = new chilli::os::file ( CONFIG_FILENAME, chilli::os::file::modeReadWrite|chilli::os::file::modeCreate );
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
    ar << compass_feedback ;
    
    // version 5
    ar << screen_mode ;
    
    // version 5
    ar << keyboard_mode;
    
    // version 7
    ar << advert_screen_count;
    ar << novella_pdf ;
    
	ar.Close();
    
	SAFEDELETE ( pFile );
    
    return TRUE;
}

BOOL config::Load ( void )
{
 	chilli::os::file* pFile = new chilli::os::file ( CONFIG_FILENAME, chilli::os::file::modeRead );
	if ( !pFile->IsOpen() ) {
		if ( pFile ) delete pFile;
		return FALSE;
	}
    
	archive ar (pFile, archive::load | archive::bNoFlushOnDelete);
    
    u32 version=CONFIG_VERSION;
    
    ar >> version ;
    ar >> tutorial ;
    
    if ( version>=2 ) {
        ar >> autofight;
        ar >> autounhide;
        ar >> showmovementindicators;
        ar >> nav_mode ;
        ar >> screentransitions ;
    }
    
    if ( version>=3 ) {
        ar >> compass_delay ;
        ar >> think_paging_mode;
        ar >> night_display_fast ;
        ar >> night_battle_full ;
    }
    
    if ( version>=5 ) {
        ar >> compass_feedback ;
        ar >> screen_mode ;
    }
    
    if ( version >= 6 ) {
        ar >> keyboard_mode ;
    }
    
    if ( version >= 7 ) {
        ar >> advert_screen_count ;
        ar >> novella_pdf ;
    }
    
	ar.Close();
    
	SAFEDELETE ( pFile );
    
    
    return TRUE;
}

