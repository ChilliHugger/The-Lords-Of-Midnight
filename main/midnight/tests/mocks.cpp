//
//  mocks.cpp
//  midnight
//
//  Created by Chris Wild on 04/01/2021.
//

#include "mocks.h"


void mocks::init()
{
    auto mr = moonring::mikesingleton();
    
    SAFEDELETE(mr->stories);
    SAFEDELETE(mr->panels);
    
    mr->stories = new storymanagermock();
    mr->stories->InjectMoonRing(mr);
    
    mr->panels = new panelmanagermock();
    mr->panels->InjectMoonRing(mr);
    mr->panels->currentpanel = new uipanelmock();
        
    reset();
}

void mocks::reset()
{
    auto mr = moonring::mikesingleton();
    auto stories = static_cast<storymanagermock*>(mr->stories);
    stories->save_called=false;
    stories->cleanup_called=false;

    auto panel = static_cast<uipanelmock*>(mr->panels->currentpanel);
    panel->init();
}
 
bool mocks::storyHasBeenSaved()
{
    auto mr = moonring::mikesingleton();
    return static_cast<storymanagermock*>(mr->stories)->save_called;
}

bool uipanelmock::init()
{
    currentmode = MODE_NONE;
    currentObject = IDT_NONE;
    return true;
}

void uipanelmock::setObject(mxid object)
{
    currentObject = object;
    return true;
}



bool storymanagermock::load ( storyid_t id )
{
    return true;
}

bool storymanagermock::save ( savemode_t mode )
{
    save_called = true;
    return true;
}
    
bool storymanagermock::create ( storyid_t id )
{
    return true;
}
    
bool storymanagermock::save ( storyid_t id, savemode_t mode )
{
    save_called = true;
    return true;
}
    
bool storymanagermock::destroy ( storyid_t id )
{
    return true;
}
    
bool storymanagermock::cleanup ( void )
{
    cleanup_called = true;
    return true;
}
    
