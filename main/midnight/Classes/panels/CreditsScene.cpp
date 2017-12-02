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
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto p = Sprite::create("credits.png");
    addChild(p);
    
    p->setPosition(Vec2(0,0));
    p->setAnchorPoint(Vec2(0,0));
    
    float scaley =  ( visibleSize.height / p->getContentSize().height)  ;
    p->setScale(1.0, scaley );

    this->scheduleOnce( [](float) {
        
        CustomDirector *director = (CustomDirector *)Director::getInstance();
        director->popSceneWithTransition<TransitionCrossFade>(1.0);
        
    }, 5.0, "mainmenu" );
    
    return true;
}


