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
#include "../library/inc/mxtypes.h"
#include "panelmanager.h"
#include "../frontend/version_check.h"

// models
#include "../models/selectmodel.h"
#include "../models/mapmodel.h"

using chilli::collections::c_ptr;

#if defined(_OS_DESKTOP_)
    #define _MOUSE_ENABLED_
#endif


//#define _DEBUG_GAME_PANEL_

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
FORWARD_REFERENCE(shadermanager);
FORWARD_REFERENCE(resolutionmanager);

typedef std::function<void(bool updateAvailable, std::string& url)> GetVersionCallback;

struct mouse_data_t
{
public:
    std::string     file;
    cocos2d::Vec2   anchor;
} ;


class moonring
{
private:
    moonring();
public:
    virtual ~moonring();
    
    static moonring* mikesingleton();
    static void release();
        
    static void complain(LPCSTR format, ...);
    static void log(LPCSTR format, ...);

    void initialise( progressmonitor* monitor );
    std::string getWritablePath();
    bool serialize( u32 version, chilli::lib::archive& ar );
    
    // assets
#if defined(_OS_DESKTOP_)
    void changeDisplayMode();
    void reloadAssets();
#endif
    
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
    
    // Version
#if defined(_USE_VERSION_CHECK_)
    void getVersion(const GetVersionCallback& callback);
#endif
    
protected:
    

    
    std::mutex              mutex;
    std::condition_variable condition;
    bool                    isDataLoaded;
    
#if defined(_USE_VERSION_CHECK_)
    bool                    versionCheckCompleted;
    bool                    isUpdateAvailable;
    std::string             updateUrl;
#endif
    
public:
    
    resolutionmanager*      resolution;
    shadermanager*          shader;
    panelmanager*           panels;
    settingsmanager*        settings;
    configmanager*          config;
    helpmanager*            help;
    storymanager*           stories;
    keyboardmanager*        keyboard;
    tmemanager*             tme;
    std::string             writeablepath;
    projectconfig*          project;
    

    
    selectmodel             selectmodel;
    mapmodel                mapmodel;
    
#if defined(_MOUSE_ENABLED_)
    std::vector<mouse_data_t*>    mouseData;
#endif
    
};

#define CONFIG(x)   (mr->settings->x)


#define COMPLAIN    moonring::complain

#ifdef MX_DEBUG
#define UIDEBUG        moonring::log
#else
#define UIDEBUG        if(0) printf
#endif
