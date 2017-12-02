#include "SplashScene.h"
#include "MainMenuScene.h"
#include "SimpleAudioEngine.h"

#include "resolutionmanager.h"

USING_NS_CC;

Scene* Splash::createScene()
{
    return Splash::create();
}

bool Splash::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    //Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto p = Sprite::create("splash.png");
    addChild(p);

    p->setPosition(Vec2(0,0));
    p->setAnchorPoint(Vec2(0,0));

    //auto res = resolutionmanager::getInstance();
    float scaley =  ( visibleSize.height / p->getContentSize().height)  ;
    p->setScale(1.0, scaley );
    

    
    this->scheduleOnce( [](float) {
        auto mainmenu = MainMenu::create();
        Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, mainmenu) );
    }, 1.0, "mainmenu" );
    
    
    return true;
}





