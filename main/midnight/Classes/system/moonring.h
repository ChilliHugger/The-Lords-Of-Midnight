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

FORWARD_REFERENCE(configmanager);
FORWARD_REFERENCE(helpmanager);
FORWARD_REFERENCE(storymanager);
FORWARD_REFERENCE(keyboardmanager);
FORWARD_REFERENCE(tmemanager);

typedef std::function<void(int,int)> MXProgressCallback;
#define RUN_ON_UI_THREAD    cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread

class moonring {
private:
    moonring();
public:
    virtual ~moonring();
    
    static moonring* mikesingleton(){
        static moonring test;
        return &test;
    }
    
    void Initialise( MXProgressCallback callback );
    
    LPCSTR GetWritablePath();
    
    BOOL Serialize( u32 version, chilli::lib::archive& ar );
    
    void NewStory();

protected:
    
    void UpdateProgress(int value);
    
public:
    
    MXProgressCallback      init_progess_callback;
    
    
    configmanager*          config;
    helpmanager*            help;
    storymanager*           stories;
    keyboardmanager*        keyboard;
    tmemanager*             tme;
    c_str                   writeablepath;

    
};

void _complain (LPCSTR format, ... );
void _debug (LPCSTR format, ... );
void _msg (LPCSTR format, ... );

#define COMPLAIN    _complain

#ifdef MX_DEBUG
#define UIDEBUG        CCLOG
#else
#define UIDEBUG        if(0) printf
#endif
