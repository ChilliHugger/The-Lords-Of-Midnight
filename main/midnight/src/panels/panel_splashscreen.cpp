#include "panel_splashscreen.h"
#include "panel_dedication.h"
#include "panel_mainmenu.h"

#include "../system/resolutionmanager.h"
#include "../ui/uihelper.h"
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

#define MAX_SPLASHSCREEN_TIME   3000
#define TRANSITION_TIME         2.0
#define MAX_PROGESS             40.0f

panel_splashscreen::panel_splashscreen() :
    progress(nullptr),
    loading_progress(nullptr)
{
}

panel_splashscreen::~panel_splashscreen()
{
    SAFEDELETE(progress);
}

bool panel_splashscreen::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    setBackgroundToHeight("screens/misc/splash.png");
    
#if defined(_LOM_)
    TTFConfig font_config_splash;

    font_config_splash.fontFilePath = FONT_FILENAME;
    font_config_splash.fontSize = RES(35);
    font_config_splash.glyphs = GlyphCollection::DYNAMIC;
    font_config_splash.outlineSize = 0;
    font_config_splash.customGlyphs = nullptr;
    font_config_splash.distanceFieldEnabled = false;
    
    f32 lineHeight = RES(40);
    f32 x = (getContentSize().width/2) - RES(476);
    f32 y = RES(32);
    
    auto label1 = Label::createWithTTF( font_config_splash, "Now explore the epic world of" );
    //label1->getFontAtlas()->setAntiAliasTexParameters();
    label1->setTextColor( Color4B(0,255,0,255) );
    label1->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(this,label1,x,y);

    auto label2 = Label::createWithTTF( font_config_splash, "THE LORDS OF MIDNIGHT" );
    //label2->getFontAtlas()->setAntiAliasTexParameters();
    label2->setTextColor( Color4B(253,253,0,255) );
    label2->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(this,label2,x,y+(lineHeight*1));

    auto label3 = Label::createWithTTF( font_config_splash, "         by" );
    //label3->getFontAtlas()->setAntiAliasTexParameters();
    label3->setTextColor( Color4B(205,0,205,255) );
    label3->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(this,label3,x,y+(lineHeight*2));

    auto label4 = Label::createWithTTF( font_config_splash, "Mike Singleton" );
    //label4->getFontAtlas()->setAntiAliasTexParameters();
    label4->setTextColor( Color4B(0,255,255,255) );
    label4->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(this,label4,x,y+(lineHeight*3));
#endif
    
    
    loading_width = RES(512);
    loading_height = RES(16);
    
    loading_progress = DrawNode::create();
    uihelper::AddBottomLeft(this, loading_progress, RES(8), RES(8));
    
    progress = new progressmonitor([&](int value) {
        updateProgress( (f32)value / MAX_PROGESS );
    });
    progress->Start();
    
    StartTime = utils::getTimeInMilliseconds();

    // Initialise in a thread
    auto atp = AsyncTaskPool::getInstance();
    atp->enqueue(AsyncTaskPool::TaskType::TASK_IO, [&]() {
        mr->initialise( progress );
        progress->Stop();
        UIDEBUG("MAX_PROGESS=%d",progress->current);
        SAFEDELETE(progress);
        complete();
    });
    
    return true;
}

void panel_splashscreen::complete()
{
    auto Duration = utils::getTimeInMilliseconds() - StartTime;
    
    // we want at least 3 seconds of splash screen
    f32 delay = MAX(0,(f32)(MAX_SPLASHSCREEN_TIME-Duration) / 1000.0f);
    
    if ( CONFIG(screentransitions) ) {
        loading_progress->runAction( FadeOut::create(1.0) );
    }else{
        loading_progress->setVisible(false);
        delay = 1;
    }
    
    this->scheduleOnce( [&](float) {

        if ( !mr->config->skip_dedication ) {
            mr->panels->setPanelMode(MODE_DEDICATION, TRANSITION_FADEIN );
        } else {
            mr->panels->showMainMenu();
        }
    }, delay, "show_mainmenu" );
}

void panel_splashscreen::updateProgress(f32 percent)
{
    RUN_ON_UI_THREAD([=](){
        loading_progress->clear();
        loading_progress->drawSolidRect(Vec2(0,0), Vec2(loading_width,loading_height), Color4F(_clrBlue) );
        loading_progress->drawSolidRect(Vec2(0,0), Vec2(loading_width*MIN(1.0,percent),loading_height), Color4F(_clrRed) );
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}




