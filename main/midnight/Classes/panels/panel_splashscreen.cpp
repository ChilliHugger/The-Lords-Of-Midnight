#include "panel_splashscreen.h"
#include "panel_mainmenu.h"
#include "SimpleAudioEngine.h"

#include "../frontend/resolutionmanager.h"
#include "../ui/uihelper.h"

USING_NS_CC;

bool panel_splashscreen::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    uihelper::Init();
    
    SetBackground("splash.png");
    
    this->scheduleOnce( [](float) {
        auto panel = panel_mainmenu::create();
        Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, panel) );
    }, 1.0, "show_mainmenu" );
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("rest-0.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("rest-1.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("language-0.plist");
    
    
    return true;
}





