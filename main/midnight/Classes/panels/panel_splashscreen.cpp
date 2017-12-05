#include "panel_splashscreen.h"
#include "panel_mainmenu.h"
#include "SimpleAudioEngine.h"

#include "resolutionmanager.h"

USING_NS_CC;

Scene* panel_splashscreen::createScene()
{
    return panel_splashscreen::create();
}

bool panel_splashscreen::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    setBackground("splash.png");
    
    this->scheduleOnce( [](float) {
        auto panel = panel_mainmenu::create();
        Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, panel) );
    }, 1.0, "show_mainmenu" );
    
    
    return true;
}





