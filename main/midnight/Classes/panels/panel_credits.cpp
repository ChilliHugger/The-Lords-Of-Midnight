//
//  CreditsScene.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../Extensions/CustomDirector.h"

#include "panel_credits.h"
#include "panel_mainmenu.h"
#include "resolutionmanager.h"


Scene* panel_credits::createScene()
{
    return panel_credits::create();
}

bool panel_credits::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    setBackground("credits.png");

    fadeExit(5.0);
    
    return true;
}


