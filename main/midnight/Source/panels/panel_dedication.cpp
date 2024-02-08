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
#include "../system/settingsmanager.h"
#include "../system/resolutionmanager.h"
#include "../system/panelmanager.h"
#include "../ui/helper.h"

USING_NS_CC;

bool panel_dedication::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    // Background White
    setBackground(_clrWhite);

    auto alpha = CONFIG(screentransitions) ? alpha_zero : alpha_normal ;
    f32 delay = CONFIG(screentransitions) ? 1.0f : 3.0f ;

    // Dedication Image
    image = Sprite::create("misc/in-memory-of.png");
    image->setOpacity(ALPHA(alpha));
    uihelper::AddCenter(this, image);
    
    // wait for a moment then start the dedication fade
    this->scheduleOnce( [&](float) {
        startDedication();
    }, delay, "show_dedication" );
    
    return true;
}

void panel_dedication::startDedication()
{
    if ( CONFIG(screentransitions) )
    {
        image->runAction(Sequence::create(
                                          FadeIn::create( 3.0f ),
                                          DelayTime::create( 3.0f ),
                                          FadeOut::create( 3.0f ),
                                          DelayTime::create( 3.0f ),
                                          CallFunc::create( [this] { nextPanel(); }),
                                          nullptr
                                          ));
    }else{
        nextPanel();
    }
}

void panel_dedication::nextPanel()
{
    mr->panels->showMainMenu();
}


