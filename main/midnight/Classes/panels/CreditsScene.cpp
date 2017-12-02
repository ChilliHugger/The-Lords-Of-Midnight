//
//  CreditsScene.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../Extensions/CustomDirector.h"

#include "CreditsScene.h"
#include "MainMenuScene.h"
#include "resolutionmanager.h"

USING_NS_CC;

Scene* Credits::createScene()
{
    return Credits::create();
}

bool Credits::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    setBackground("credits.png");

    fadeExit(5.0);
    
    return true;
}


