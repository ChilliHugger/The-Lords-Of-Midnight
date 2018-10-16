
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


bool panel_advert::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    setBackground(_clrBlue);
    
    return true;
}
