//
//  OptionsScene.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../Extensions/CustomDirector.h"

#include "panel_options.h"
#include "panel_mainmenu.h"
#include "resolutionmanager.h"

//USING_NS_CC;

Scene* panel_options::createScene()
{
    return panel_options::create();
}

bool panel_options::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    setBackground("screens/misc/main_menu.png");

    fadeExit(5.0);
    
    return true;
}



