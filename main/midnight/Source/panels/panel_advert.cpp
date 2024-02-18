
/*
 *  panel_advert.cpp
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#include "panel_advert.h"
#include "../system/moonring.h"
#include "../ui/uihelper.h"

#if defined(_LOM_)
#if defined(_OS_DESKTOP_)
    constexpr LPCSTR SCREEN_ADVERT      = "screens/misc/lom_advert_for_desktops.png";
#else
    constexpr LPCSTR SCREEN_ADVERT      = "screens/misc/lom_advert_for_mobiles.png";
#endif
#endif

#if defined(_DDR_)
#if defined(_OS_DESKTOP_)
    constexpr LPCSTR SCREEN_ADVERT      = "screens/misc/ddr_advert_for_desktops.png";
#else
    constexpr LPCSTR SCREEN_ADVERT      = "screens/misc/ddr_advert_for_mobile.png";
#endif
#endif

constexpr f32 ADVERT_VISIBLE_DELAY = 5.0f;

bool panel_advert::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }

    setBackgroundToWidth(SCREEN_ADVERT);
    
    return true;
}

void panel_advert::OnActivate()
{
    this->scheduleOnce( [&](float) {
        mr->panels->setPanelMode(MODE_MAINMENU, TRANSITION_FADEIN);
    }, ADVERT_VISIBLE_DELAY, "delayed_fade_exit" );
}
