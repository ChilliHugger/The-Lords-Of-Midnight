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
    
    setBackground("splash.png");
    
    this->scheduleOnce( [](float) {
        auto mainmenu = MainMenu::create();
        Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, mainmenu) );
    }, 1.0, "mainmenu" );
    
    
    return true;
}





