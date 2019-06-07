//
//  moonring.h
//  midnight
//
//  "It will give you the Power of Command and the Power of Vision over those lords and subjects
//  loyal to you, even at great distances. With the Power of Vision you will be able to see through
//  their eyes what they see. With the Power of Command you will be able to urge them to undertake
//  any task they would willingly perform for you. And more than this, it will echo the warmth and
//  strength of your mind and send forth a tide of hope across the cold lands of Midnight.
//  It is yours. Take it, and use it with care."
//
//  Created by Chris Wild on 07/12/2017.
//

#pragma once
#include "../cocos.h"
#include "storymanager.h"
#include "../library/libinc/mxtypes.h"
#include "panelmanager.h"

// models
#include "../models/selectmodel.h"
#include "../models/mapmodel.h"

//
#include <mutex>

FORWARD_REFERENCE(settingsmanager);
FORWARD_REFERENCE(helpmanager);
//FORWARD_REFERENCE(storymanager);
FORWARD_REFERENCE(keyboardmanager);
FORWARD_REFERENCE(tmemanager);
FORWARD_REFERENCE(progressmonitor);
FORWARD_REFERENCE(projectconfig);
FORWARD_REFERENCE(configmanager);

#define RUN_ON_UI_THREAD    cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread

#define RUN_EVENT(x)           RUN_ON_UI_THREAD([=](){ x; } )

class moonring
{
    using GLProgramState = cocos2d::GLProgramState;
    using GLProgram = cocos2d::GLProgram;

private:
    moonring();
public:
    virtual ~moonring();
    
    static moonring* mikesingleton(){
        static moonring singleton;
        return &singleton;
    }
    
	static void complain(LPCSTR format, ...);
	static void log(LPCSTR format, ...);

    void initialise( progressmonitor* monitor );
    LPCSTR getWritablePath();
    bool serialize( u32 version, chilli::lib::archive& ar );
    
    // Stories
    storyid_t startNewStory();
    storyid_t getCurrentStory();
    void continueStory( storyid_t id );
    void closeStory();
    
    // Group
    bool swapGroupLeader(mxid id);
    bool joinGroup(mxid id, mxid leaderid );
    bool leaveGroup(mxid id);
    bool disbandGroup(mxid id);

    // Actions
    bool selectCharacter(mxid id);
    bool look( mxdir_t dir );
    bool undo( savemode_t mode );
    bool night();
    bool approach();
    bool seek();
    bool fight();
    bool hideunhide();
    bool think();
    bool recruitMen();
    bool postMen();
    bool attack();
    
#if defined(_DDR_)
    bool use();
    bool take();
    bool give();
    bool rest();
    bool enterTunnel();
#endif
    
    //
    bool look();
    bool dawn();
    
    // Pages
    void showPage( panelmode_t mode, mxid object = IDT_NONE );

    bool checkGameOverConditions ( void );
protected:
    

    
    std::mutex              mutex;
    std::condition_variable condition;
    bool                    isDataLoaded;
    
public:
    
    panelmanager*           panels;
    settingsmanager*        settings;
    configmanager*          config;
    helpmanager*            help;
    storymanager*           stories;
    keyboardmanager*        keyboard;
    tmemanager*             tme;
    c_str                   writeablepath;
    projectconfig*          project;
    
    GLProgramState*         glProgramState;
    GLProgram*              glShaderProgram;
    
    selectmodel             selectmodel;
    mapmodel                mapmodel;
    
};

#define CONFIG(x)   (mr->settings->x)


//void _msg (LPCSTR format, ... );

#define COMPLAIN    moonring::complain

#ifdef MX_DEBUG
#define UIDEBUG        moonring::log
#else
#define UIDEBUG        if(0) printf
#endif
