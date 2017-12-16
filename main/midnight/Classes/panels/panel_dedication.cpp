//
//  panel_dedication.cpp
//  midnight
//
//  Created by Chris Wild on 16/12/2017.
//
//

#include "panel_dedication.h"
#include "panel_mainmenu.h"

#include "resolutionmanager.h"
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
    
    // wait for a moment then start the dedication fade
    this->scheduleOnce( [&](float) {
        StartDedication();
    }, 3.0f, "show_dedication" );
    
    return true;
}

void panel_dedication::StartDedication()
{
    image->runAction(Sequence::create(
                                      FadeIn::create( 3.0f ),
                                      DelayTime::create( 3.0f ),
                                      FadeOut::create( 3.0f ),
                                      DelayTime::create( 3.0f ),
                                      CallFunc::create( [this] { NextPanel(); }),
                                      nullptr
                                      ));
}

void panel_dedication::NextPanel()
{
    Director::getInstance()->replaceScene(TransitionCrossFade::create(2.0, panel_mainmenu::create()) );
}


