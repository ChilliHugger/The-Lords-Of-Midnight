//
//  OptionsScene.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../Extensions/CustomDirector.h"

#include "OptionsScene.h"
#include "MainMenuScene.h"
#include "resolutionmanager.h"

USING_NS_CC;

Scene* Options::createScene()
{
    return Options::create();
}

bool Options::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    setBackground("screens/misc/main_menu.png");

    fadeExit(5.0);
    
    return true;
}



