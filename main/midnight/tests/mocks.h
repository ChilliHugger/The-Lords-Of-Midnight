//
//  mocks.hpp
//  midnight
//
//  Created by Chris Wild on 04/01/2021.
//
#pragma once

#include "../src/system/moonring.h"
#include "../src/ui/uipanel.h"

class storymanagermock : public storymanager
{
public:
    virtual bool load ( storyid_t id ) override;
    virtual bool save ( savemode_t mode = savemode_normal ) override;
    virtual bool create ( storyid_t id ) override;
    virtual bool save ( storyid_t id, savemode_t mode=savemode_normal ) override;
    virtual bool destroy ( storyid_t id ) override;
    virtual bool cleanup ( void ) override;
        
public:
    bool cleanup_called;
    bool save_called;

};

class panelmanagermock : public panelmanager
{
public:
    virtual void setPanelMode ( panelmode_t mode, bool history = false) override
    {
        previousmode = currentmode;
        currentmode = mode;
    }
    
    virtual void setPanelMode ( panelmode_t mode, transition_t transition, bool history = false ) override
    {
        setPanelMode(mode);
    }
};

class uipanelmock : public uipanel
{
public:

    virtual bool init() override;
    virtual void setObject( mxid object ) override ;

public:
    mxid currentObject;
};

class mocks
{
public:
    static void init();
    static void reset();
    static bool storyHasBeenSaved();
};
