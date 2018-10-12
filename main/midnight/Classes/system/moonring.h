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

#include "../library/libinc/mxtypes.h"
#include "panelmanager.h"

FORWARD_REFERENCE(configmanager);
FORWARD_REFERENCE(helpmanager);
//FORWARD_REFERENCE(storymanager);
FORWARD_REFERENCE(keyboardmanager);
FORWARD_REFERENCE(tmemanager);
//FORWARD_REFERENCE(panelmanager);
FORWARD_REFERENCE(progressmonitor);
FORWARD_REFERENCE(projectconfig);

#include "storymanager.h"

#define RUN_ON_UI_THREAD    cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread


class moonring {
private:
    moonring();
public:
    virtual ~moonring();
    
    static moonring* mikesingleton(){
        static moonring singleton;
        return &singleton;
    }
    
    void Initialise( progressmonitor* monitor );
    
    LPCSTR GetWritablePath();
    
    BOOL Serialize( u32 version, chilli::lib::archive& ar );
    
    storyid_t NewStory();
    void LoadStory( storyid_t id );
    
    void ShowPage( panelmode_t mode, mxid object );
    
protected:
    
    BOOL CheckGameOverConditions ( void );
    
    std::mutex              mutex;
    std::condition_variable condition;
    bool                    isDataLoaded;
    
public:
    
    panelmanager*           panels;
    configmanager*          config;
    helpmanager*            help;
    storymanager*           stories;
    keyboardmanager*        keyboard;
    tmemanager*             tme;
    c_str                   writeablepath;
    projectconfig*          project;
    
    cocos2d::GLProgramState*         glProgramState;
    cocos2d::GLProgram*              glShaderProgram;
    
};

#define CONFIG(x)   (mr->config->x)

void _complain (LPCSTR format, ... );
void _debug (LPCSTR format, ... );
void _msg (LPCSTR format, ... );

#define COMPLAIN    _complain

#ifdef MX_DEBUG
#define UIDEBUG        CCLOG
#else
#define UIDEBUG        if(0) printf
#endif
