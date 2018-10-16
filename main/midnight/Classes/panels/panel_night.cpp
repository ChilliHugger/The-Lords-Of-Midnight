
/*
 *  panel_night.cpp
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#include "panel_night.h"

#include "../system/moonring.h"
//#include "../system/configmanager.h"
//#include "../system/resolutionmanager.h"
//#include "../system/panelmanager.h"
#include "../ui/uihelper.h"


bool panel_night::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    setBackground(_clrDarkBlue);
    
    return true;
}
