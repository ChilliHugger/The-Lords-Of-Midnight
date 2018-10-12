//
//  panelmanager.cpp
//  midnight
//
//  Created by Chris Wild on 13/06/2018.
//

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../Extensions/CustomDirector.h"

#include "panelmanager.h"
#include "../panels/panel_mainmenu.h"
#include "../panels/panel_think.h"
#include "../panels/panel_credits.h"
#include "../panels/panel_splashscreen.h"
#include "../panels/panel_dedication.h"
#include "../panels/panel_options.h"
#include "../panels/panel_look.h"
#include "../ui/uioptionitem.h"

#include "configmanager.h"
#include "moonring.h"
#include "../ui/uipanel.h"

USING_NS_CC;
using namespace cocos2d::ui;

static const f32 defaultDuration = 1.0;


uipanel* panelmanager::CurrentPanel()
{
    return currentpanel;
}

void panelmanager::SetPanelMode ( panelmode_t mode, bool history )
{
    SetPanelMode(mode, TRANSITION_NONE, history);
}

void panelmanager::SetPanelMode ( panelmode_t mode, transition_t transition, bool history )
{
    if ( mode == MODE_MAINMENU ) {
        if ( CONFIG(bumpAdvert()) )
            mode = MODE_ADVERT;
    }

    previousmode = this->currentmode ;

    this->currentmode = mode;

    uipanel* panel = GetPanel(mode);

    if ( panel ) {
        panel->currentmode = mode ;
    }

    if ( history )
        PushCurrentPanel(panel,transition);
    else
        SetCurrentPanel(panel, transition);
}

uipanel* panelmanager::GetPanel( panelmode_t mode )
{
    uipanel* panel = nullptr;
    
    if ( mode == MODE_MAINMENU     )
        panel = panel_mainmenu::create();
    else if ( mode == MODE_OPTIONS )
        panel =  panel_options::create();
    else if ( mode == MODE_CREDITS )
        panel =  panel_credits::create();
    else if ( mode == MODE_TITLE  )
        panel =  panel_splashscreen::create() ;
    else if ( mode == MODE_DEDICATION  )
        panel =  panel_dedication::create() ;
    else if ( mode == MODE_LOOK )
        panel =  panel_look::create();
    
//    else if ( mode == MODE_ADVERT )
//        return panel_advert::create();

    else if ( mode>= MODE_THINK && mode<=MODE_THINK_APPROACH )
        panel =  panel_think::create();
//    else if ( mode>= MODE_SELECT && mode<=MODE_SELECT_LOC )
//        return p_select;
//    else if ( mode == MODE_NIGHT )
//        return p_night;
//    else if ( mode == MODE_MAP || mode == MODE_DEBUG_MAP )
//        return p_map;
//
//    else if ( mode == MODE_WIN     )
//        return p_gameover;
//    else if ( mode == MODE_LOSE     )
//        return p_gameover;
    
    if ( panel != nullptr ) {
        panel->currentmode = mode;
    }
    
    return panel;
}

void panelmanager::ReturnToPrevious()
{
    ReturnToPrevious(reverseTransition);
}

void panelmanager::ReturnToPrevious( transition_t transition )
{
    if ( !CONFIG(screentransitions) )
        transition=TRANSITION_NONE;
    
    CustomDirector *director = (CustomDirector *)Director::getInstance();
    
    if ( transition == TRANSITION_NONE ) {
        
        director->popScene();

    } else if ( transition == TRANSITION_SLIDEUP ) {
        
        director->popSceneWithTransition<TransitionMoveInB>(defaultDuration);

    } else if ( transition == TRANSITION_SLIDEDOWN ) {
        
        director->popSceneWithTransition<TransitionMoveInT>(defaultDuration);

    } else if ( transition == TRANSITION_PUSHUP ) {
        
        director->popSceneWithTransition<TransitionSlideInB>(defaultDuration);

    } else if ( transition == TRANSITION_PUSHDOWN ) {
        
        director->popSceneWithTransition<TransitionSlideInT>(defaultDuration);

    } else if ( transition == TRANSITION_SLIDELEFT ) {
        
        director->popSceneWithTransition<TransitionMoveInL>(defaultDuration);

    } else if ( transition == TRANSITION_SLIDERIGHT ) {
        
        director->popSceneWithTransition<TransitionMoveInR>(defaultDuration);

    } else if ( transition == TRANSITION_PUSHLEFT ) {
        
        director->popSceneWithTransition<TransitionSlideInL>(defaultDuration);

    } else if ( transition == TRANSITION_PUSHRIGHT ) {
        
        director->popSceneWithTransition<TransitionSlideInR>(defaultDuration);

    } else if ( transition == TRANSITION_FADEIN ) {
        
        director->popSceneWithTransition<TransitionCrossFade>(defaultDuration);
    }
    
    
    
}

void panelmanager::SetCurrentPanel( uipanel* incomming, transition_t transition )
{
    //ui->hideKeyboardShortcuts(FALSE);
    
    if ( !CONFIG(screentransitions) )
        transition=TRANSITION_NONE;
    
    outgoing_panel = CurrentPanel();
    
    if ( outgoing_panel == incomming )
        transition = TRANSITION_NONE ;
    
    currentpanel = incomming;
    
    if ( incomming ) {
        
        Director* director = Director::getInstance();
        
        if ( transition == TRANSITION_NONE ) {
            
            director->replaceScene(incomming);
            
        } else if ( transition == TRANSITION_SLIDEUP ) {
            
            director->replaceScene(TransitionMoveInB::create(defaultDuration, incomming) );
            
        } else if ( transition == TRANSITION_SLIDEDOWN ) {
            
            director->replaceScene(TransitionMoveInT::create(defaultDuration, incomming) );
            
        } else if ( transition == TRANSITION_PUSHUP ) {

            director->replaceScene(TransitionSlideInB::create(defaultDuration, incomming) );
            
        } else if ( transition == TRANSITION_PUSHDOWN ) {
            
            director->replaceScene(TransitionSlideInT::create(defaultDuration, incomming) );

        } else if ( transition == TRANSITION_SLIDELEFT ) {
          
            director->replaceScene(TransitionMoveInL::create(defaultDuration, incomming) );

        } else if ( transition == TRANSITION_SLIDERIGHT ) {
            
            director->replaceScene(TransitionMoveInR::create(defaultDuration, incomming) );
            
        } else if ( transition == TRANSITION_PUSHLEFT ) {
            
            director->replaceScene(TransitionSlideInL::create(defaultDuration, incomming) );
            
        } else if ( transition == TRANSITION_PUSHRIGHT ) {
            
            director->replaceScene(TransitionSlideInR::create(defaultDuration, incomming) );

        } else if ( transition == TRANSITION_FADEIN ) {
            
            director->replaceScene(TransitionCrossFade::create(defaultDuration, incomming) );

        }
        
    }
}

void panelmanager::PushCurrentPanel( uipanel* incomming, transition_t transition )
{
    
    if ( !CONFIG(screentransitions) )
        transition=TRANSITION_NONE;
    
    outgoing_panel = CurrentPanel();
    
    if ( outgoing_panel == incomming )
        transition = TRANSITION_NONE ;
    

    currentpanel = incomming;
    
    if ( incomming ) {
        
        Director* director = Director::getInstance();
        
        if ( transition == TRANSITION_NONE ) {
            
            director->pushScene(incomming);
            reverseTransition = transition;
            
        } else if ( transition == TRANSITION_SLIDEUP ) {
            
            director->pushScene(TransitionMoveInB::create(defaultDuration, incomming) );
            reverseTransition = TRANSITION_SLIDEDOWN;
            
        } else if ( transition == TRANSITION_SLIDEDOWN ) {
            
            director->pushScene(TransitionMoveInT::create(defaultDuration, incomming) );
            reverseTransition = TRANSITION_SLIDEUP;
            
        } else if ( transition == TRANSITION_PUSHUP ) {
            
            director->pushScene(TransitionSlideInB::create(defaultDuration, incomming) );
            reverseTransition = TRANSITION_PUSHDOWN;

        } else if ( transition == TRANSITION_PUSHDOWN ) {
            
            director->pushScene(TransitionSlideInT::create(defaultDuration, incomming) );
            reverseTransition = TRANSITION_PUSHUP;

        } else if ( transition == TRANSITION_SLIDELEFT ) {
            
            director->pushScene(TransitionMoveInL::create(defaultDuration, incomming) );
            reverseTransition = TRANSITION_SLIDERIGHT;

        } else if ( transition == TRANSITION_SLIDERIGHT ) {
            
            director->pushScene(TransitionMoveInR::create(defaultDuration, incomming) );
            reverseTransition = TRANSITION_SLIDELEFT;

        } else if ( transition == TRANSITION_PUSHLEFT ) {
            
            director->pushScene(TransitionSlideInL::create(defaultDuration, incomming) );
            reverseTransition = TRANSITION_PUSHRIGHT;

        } else if ( transition == TRANSITION_PUSHRIGHT ) {
            
            director->pushScene(TransitionSlideInR::create(defaultDuration, incomming) );
            reverseTransition = TRANSITION_PUSHLEFT;

        } else if ( transition == TRANSITION_FADEIN ) {
            
            director->pushScene(TransitionCrossFade::create(defaultDuration, incomming) );
            reverseTransition = transition;
        }
    }
    

}

