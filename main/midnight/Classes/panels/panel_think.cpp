
/*
 *  panel_think.cpp
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#include "panel_think.h"

bool panel_think::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    return true;
}
