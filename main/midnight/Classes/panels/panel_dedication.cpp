//
//  panel_dedication.cpp
//  midnight
//
//  Created by Chris Wild on 16/12/2017.
//
//

#include "panel_dedication.h"
#include "panel_mainmenu.h"

#include "../system/moonring.h"
#include "../system/configmanager.h"
#include "../system/resolutionmanager.h"
#include "../system/panelmanager.h"
#include "../ui/uihelper.h"

USING_NS_CC;

bool panel_dedication::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    // Background White
    SetBackground(_clrWhite);
    
    // Dedication Image
    image = SetBackground("in-memory-of.png");
    image->setOpacity(0);
    
    f32 delay = CONFIG(screentransitions) ? 3.0f : 1.0f ;
    
    // wait for a moment then start the dedication fade
    this->scheduleOnce( [&](float) {
        StartDedication();
    }, delay, "show_dedication" );
    
    return true;
}

void panel_dedication::StartDedication()
{
    if ( CONFIG(screentransitions) )
    {
        image->runAction(Sequence::create(
                                          FadeIn::create( 3.0f ),
                                          DelayTime::create( 3.0f ),
                                          FadeOut::create( 3.0f ),
                                          DelayTime::create( 3.0f ),
                                          CallFunc::create( [this] { NextPanel(); }),
                                          nullptr
                                          ));
    }else{
        NextPanel();
    }
}

void panel_dedication::NextPanel()
{
    mr->panels->SetPanelMode(MODE_MAINMENU,TRANSITION_FADEIN);
}


