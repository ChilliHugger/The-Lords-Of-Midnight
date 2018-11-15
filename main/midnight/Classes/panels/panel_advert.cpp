
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
//#include "../system/configmanager.h"
//#include "../system/resolutionmanager.h"
//#include "../system/panelmanager.h"
#include "../ui/uihelper.h"

#if defined(_LOM_)
#if defined(_OS_DESKTOP_)
#define SCREEN_ADVERT   "screens/misc/lom_advert_for_desktops.png"
#else
#define SCREEN_ADVERT   "screens/misc/lom_advert_for_mobiles.png"
#endif
#endif

#if defined(_DDR_)
#if defined(_OS_DESKTOP_)
#define SCREEN_ADVERT   "screens/misc/ddr_advert_for_desktops.png"
#else
#define SCREEN_ADVERT   "screens/misc/ddr_advert_for_mobile.png"
#endif
#endif


bool panel_advert::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    setBackgroundToWidth(SCREEN_ADVERT);
    
    Exit(5.0);
    
    return true;
}
