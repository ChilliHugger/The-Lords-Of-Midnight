#include "panel_splashscreen.h"
#include "panel_dedication.h"
#include "panel_mainmenu.h"

#include "../frontend/resolutionmanager.h"
#include "../ui/uihelper.h"
#include "../system/moonring.h"
#include "../system/configmanager.h"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <future>

USING_NS_CC;

#define MAX_SPLASHSCREEN_TIME   3000
#define TRANSITION_TIME         2.0

bool panel_splashscreen::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    uihelper::Init();
    
    SetBackground("splash.png");
    
    loading_width = RES(512);
    loading_height = RES(16);
    
    loading_progress = DrawNode::create();
    uihelper::AddBottomLeft(this, loading_progress, RES(8), RES(8));
    
    UpdateProgress(0);
    
    StartTime = utils::getTimeInMilliseconds();
    
    // Initialise in a thread
    auto atp = AsyncTaskPool::getInstance();
    atp->enqueue(AsyncTaskPool::TaskType::TASK_IO, [&]() {
        mr->Initialise( [&](int max, int value) {
            UpdateProgress( (f32)value/(f32)max );
        });
        Complete();
    });
    
    return true;
}

void panel_splashscreen::Complete()
{
    if ( mr->config->screentransitions )
        loading_progress->runAction( FadeOut::create(1.0) );
    else
        loading_progress->setVisible(false);
    
    auto Duration = utils::getTimeInMilliseconds() - StartTime;
    
    // we want at least 3 seconds of splash screen
    f32 delay = MAX(0,(f32)(MAX_SPLASHSCREEN_TIME-Duration) / 1000.0f);
    f32 transition = TRANSITION_TIME;
    
    if (!mr->config->screentransitions ) {
        delay = 1;
        transition=0;
    }
    
    this->scheduleOnce( [&](float) {
        
        Scene* panel = nullptr;
        
        if ( mr->config->screentransitions )
            panel = panel_dedication::create() ;
        else
            panel = panel_mainmenu::create();
        
        Director::getInstance()->replaceScene(TransitionCrossFade::create(transition, panel) );
    }, delay, "show_mainmenu" );
}

void panel_splashscreen::UpdateProgress(f32 percent)
{
    loading_progress->clear();
    loading_progress->drawSolidRect(Vec2(0,0), Vec2(loading_width,loading_height), Color4F(_clrBlue) );
    loading_progress->drawSolidRect(Vec2(0,0), Vec2(loading_width*percent,loading_height), Color4F(_clrRed) );
}




