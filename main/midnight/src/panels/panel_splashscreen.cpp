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
    
    setBackgroundToHeight("splash.png");
    
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




