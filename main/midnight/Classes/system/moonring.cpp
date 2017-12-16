//
//  moonring.cpp
//  midnight
//
//  Created by Chris Wild on 07/12/2017.
//

#include "moonring.h"
#include "helpmanager.h"
#include "configmanager.h"
#include "storymanager.h"
#include "keyboardmanager.h"
#include "tmemanager.h"

USING_NS_CC;

using namespace chilli::lib;

static bool mySerialize ( u32 version, chilli::lib::archive& ar )
{
    return moonring::mikesingleton()->Serialize(version, ar );
}



moonring::moonring()
{
    init_progess_callback = nullptr;
    
    help = new helpmanager(this);
    config = new configmanager(this);
    stories = new storymanager(this);
    keyboard = new keyboardmanager();
    tme = new tmemanager();
    
    stories->SetLoadSave(&mySerialize);
    
    config->Load();
    
    keyboard->SetKeyboardMode((CONFIG_KEYBOARD_MODE)config->keyboard_mode);

}



moonring::~moonring()
{
    SAFEDELETE(help);
    SAFEDELETE(config);
    SAFEDELETE(stories);
    SAFEDELETE(keyboard);
    SAFEDELETE(tme);
}


LPCSTR moonring::GetWritablePath()
{
    auto path = cocos2d::FileUtils::getInstance()->getWritablePath();
    
#if defined(_OS_DESKTOP_)
    path.append(TME_ScenarioName()).c_str();
#endif
    
    writeablepath = path.c_str();
    
    return (LPCSTR)writeablepath;
}


void moonring::NewStory()
{
    //destroyGamePanels();
    
    storyid_t id = stories->alloc();
    
    stories->create(id);
    
    config->setGameDefaults();
    
    UIDEBUG("Resolve TME Data");
    
    tme->ResolveTMEData();

    //createGamePanels();
    
    help->Load( id );
}

BOOL moonring::Serialize( u32 version, archive& ar )
{
    // TME will not call this function
    // prior to version 5
    
    tme->ResolveTMEData();
    
    if ( version <=5 )
        return TRUE;
    
    version = FRONTEND_SAVE_GAME_VERSION;
    
    
    if ( ar.IsStoring() ) {
        
        ar << version ;
        
        //p_map->Serialize(version, ar);
        ////p_look->Serialize(version, ar);
        //p_select->Serialize(version, ar);
        ////p_think->Serialize(version, ar);
        
        stories->Serialize(version, ar);
        
        
    }else{
        
        ar >> version ;
        
        //p_map->Serialize(version, ar);
        ////p_look->Serialize(version, ar);
        //p_select->Serialize(version, ar);
        ////p_think->Serialize(version, ar);
        
        if ( version>= 10 )
            stories->Serialize(version, ar);
        
    }
    
    return TRUE;
}

void moonring::Initialise( MXProgressCallback callback )
{
    init_progess_callback = callback;
    
    // let's give up some time to the ui Immediatley
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    RUN_ON_UI_THREAD([=](){
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("rest-0.plist");
        UpdateProgress(1);
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    RUN_ON_UI_THREAD([=](){
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("rest-1.plist");
        UpdateProgress(2);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    RUN_ON_UI_THREAD([=](){
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("language-0.plist");
        UpdateProgress(3);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

void moonring::UpdateProgress(int value)
{
    if ( init_progess_callback != nullptr )
        init_progess_callback(3,value);
}




void _complain (LPCSTR format, ... )
{
    static char msg_buffer[1024];

    va_list arglist ;
    
    va_start( arglist, format ) ;
    vsprintf( msg_buffer, format, arglist );
    va_end( arglist ) ;
    
    CCLOG("%s",msg_buffer);
    
    exit(-1);
}



