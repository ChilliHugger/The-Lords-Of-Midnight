#include "panel_splashscreen.h"
#include "panel_dedication.h"
#include "panel_mainmenu.h"
#include "../frontend/language.h"

#include "../system/resolutionmanager.h"
#include "../ui/helper.h"
#include "../system/moonring.h"
#include "../system/settingsmanager.h"
#include "../system/configmanager.h"
#include "../system/panelmanager.h"
#include "../system/progressmonitor.h"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <future>

USING_NS_CC;

constexpr s32 MAX_SPLASHSCREEN_TIME = 3000;
constexpr f32 TRANSITION_TIME = 2.0f;
constexpr f32 MAX_PROGRESS = 40.0f;

panel_splashscreen::panel_splashscreen() :
    progress(nullptr),
    loadingProgress(nullptr),
    loadingBars(nullptr),
    loadingComplete(false),
    loadingHeight(0),
    loadingWidth(0),
    startTime(0)
{
}

panel_splashscreen::~panel_splashscreen()
{
    SAFEDELETE(progress)
    //CC_SAFE_RELEASE(loadingProgress);
    //CC_SAFE_RELEASE(loadingBars);
}

bool panel_splashscreen::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    auto backgroundColor = setBackground(_clrDarkYellow);
    backgroundColor->setLocalZOrder(ZORDER_FAR-2);
     
    auto background = setBackgroundToHeight("screens/0/splash.png",false);

#if defined(_LOM_)
    TTFConfig font_config_splash;

    font_config_splash.fontFilePath = FONT_FILENAME;
    font_config_splash.fontSize = RES(35);
    font_config_splash.glyphs = GlyphCollection::DYNAMIC;
    font_config_splash.outlineSize = 0;
    font_config_splash.distanceFieldEnabled = false;
    
    f32 lineHeight = RES(40);
    f32 x = RES(32);
    f32 y = RES(32);
    
    auto label1 = Label::createWithTTF( font_config_splash, SPLASH_SCREEN_EXPLORE );
    label1->setTextColor( Color4B(0,255,0,255) );
    label1->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(background,label1,x,y);

    auto label2 = Label::createWithTTF( font_config_splash, SPLASH_SCREEN_TITLE );
    label2->setTextColor( Color4B(253,253,0,255) );
    label2->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(background,label2,x,y+(lineHeight*1));

    auto label3 = Label::createWithTTF( font_config_splash, SPLASH_SCREEN_BY );
    label3->setTextColor( Color4B(205,0,205,255) );
    label3->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(background,label3,x,y+(lineHeight*2));

    auto label4 = Label::createWithTTF( font_config_splash, SPLASH_SCREEN_MIKE );
    label4->setTextColor( Color4B(0,255,255,255) );
    label4->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(background,label4,x,y+(lineHeight*3));
#endif
    

    loadingWidth = RES(512);
    loadingHeight = RES(16);

    loadingProgress = DrawNode::create();
    uihelper::AddBottomLeft(background, loadingProgress, RES(8), RES(8));

    loadingBars = DrawNode::create();
    loadingBars->setContentSize(getContentSize());
    loadingBars->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    loadingBars->setLocalZOrder(ZORDER_FAR-1);
    uihelper::AddTopLeft(this,loadingBars);
    
    loadingComplete=false;
    scheduleUpdate();
    
    
    progress = new progressmonitor([&](int value) {
        updateProgress((f32)value / MAX_PROGRESS );
    });
    progress->Start();
    
    startTime = utils::getTimeInMilliseconds();

    // Initialise in a thread
    auto atp = AsyncTaskPool::getInstance();
    atp->enqueue(AsyncTaskPool::TaskType::TASK_IO, [&]()
    {
        mr->initialise( progress );
        
        this->scheduleOnce( [&](float) {
            complete();
        }, 0.1, "delayed_complete" );
        
    });
    
    return true;
}

void panel_splashscreen::update(float delta)
{
    f32 y=0;
    f32 width = getContentSize().width;
    int c=chilli::randomno::instance.get()&1;

    uipanel::update(delta);

    loadingProgress->clear();

    if(loadingProgress->isVisible()) {
        loadingProgress->drawSolidRect(Vec2::ZERO, Vec2(loadingWidth,loadingHeight), Color4F(_clrDarkYellow) );
        loadingProgress->drawSolidRect(Vec2::ZERO, Vec2(loadingWidth*MIN(1.0f,currentProgress),loadingHeight), Color4F(_clrDarkBlue) );
    }

    if(chilli::randomno::instance.chance(0.5)) {
        return;
    }
    
    loadingBars->clear();
    
    if(loadingComplete) {
        return;
    }

    while(y<getContentSize().height)
    {
        f32 height = chilli::randomno::instance.get(RES(2), RES(20));

        if(c==0) {
            loadingBars->drawSolidRect(Vec2(0,y), Vec2(0+width, y+height), Color4F(_clrDarkBlue) );
        }
        c = (c+1)&1;

        y+=height;
    }
}

void panel_splashscreen::complete()
{
    progress->Stop();
    loadingComplete=true;
    UIDEBUG("MAX_PROGRESS=%d",progress->current);
    SAFEDELETE(progress)

    auto Duration = utils::getTimeInMilliseconds() - startTime;
    
    // we want at least 3 seconds of splash screen
    f32 delay = MAX(0,(f32)(MAX_SPLASHSCREEN_TIME-Duration) / 1000.0f);
    
    if ( CONFIG(screentransitions) ) {
        loadingProgress->runAction( FadeOut::create(1.0) );
    }else{
        loadingProgress->setVisible(false);
        delay = 1;
    }
    
    this->scheduleOnce([&](float)
    {
        if ( !mr->config->skip_dedication ) {
            mr->panels->setPanelMode(MODE_DEDICATION, TRANSITION_FADEIN );
        } else {
            mr->panels->showMainMenu();
        }
    }, delay, "show_mainmenu" );
}

void panel_splashscreen::updateProgress(f32 percent)
{
    currentProgress = percent;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
