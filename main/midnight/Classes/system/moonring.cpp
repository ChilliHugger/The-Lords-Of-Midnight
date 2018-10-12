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
#include "panelmanager.h"
#include "projectconfig.h"
#include "progressmonitor.h"

#include "../panels/panel_look.h"
#include "../panels/panel_think.h"


USING_NS_CC;

using namespace chilli::lib;

static bool mySerialize ( u32 version, chilli::lib::archive& ar )
{
    return moonring::mikesingleton()->Serialize(version, ar );
}



moonring::moonring()
{
    help = new helpmanager();
    help->InjectMoonRing(this);
    
    config = new configmanager();
    config->InjectMoonRing(this);

    stories = new storymanager();
    stories->InjectMoonRing(this);

    keyboard = new keyboardmanager();
    tme = new tmemanager();
    
    panels = new panelmanager();
    panels->InjectMoonRing(this);
    
    project = new projectconfig();
    project->InjectMoonRing(this);
    
    stories->SetLoadSave(&mySerialize);
    
    config->Load();
    
    keyboard->SetKeyboardMode((CONFIG_KEYBOARD_MODE)config->keyboard_mode);

}



moonring::~moonring()
{
    SAFEDELETE(panels);
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


storyid_t moonring::NewStory()
{
    //destroyGamePanels();
    
    storyid_t id = stories->alloc();
    
    stories->create(id);
    
    config->setGameDefaults();
    
    UIDEBUG("Resolve TME Data");
    
    tme->ResolveTMEData();

    //createGamePanels();
    
    help->Load( id );
    
    return id;
}

void moonring::ShowPage( panelmode_t mode, mxid object )
{
    panels->SetPanelMode(mode);
    panels->CurrentPanel()->SetObject(object);
}

void moonring::LoadStory( storyid_t id )
{
    //destroyGamePanels();
    
    stories->load(id);
    help->Load( id );
    
    //createGamePanels();
    
    TME_CurrentCharacter( TME_CurrentCharacter().id );
    
    // start game
    if (!CheckGameOverConditions() ) {
        character& c = TME_CurrentCharacter();
        auto mode = Character_IsDead(c) ? MODE_THINK : MODE_LOOK;
        
        panels->SetPanelMode(mode,TRANSITION_PUSHUP);

        if ( mode == MODE_LOOK ) {
            panels->CurrentPanel()->SetObject(c.id);
        }
        
    }
}

BOOL moonring::CheckGameOverConditions ( void )
{
    m_gameover_t win = TME_CheckWinLose();
    if ( win != MG_NONE ) {
        auto mode =  win == MG_WIN ? MODE_WIN : MODE_LOSE ;
        panels->SetPanelMode(mode);
        return TRUE;
    }
    return FALSE;
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



void moonring::Initialise( progressmonitor* monitor )
{

    isDataLoaded = false;
    int count=0;
    
    // let's give up some time to the ui Immediatley
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // load images
    for (std::string file_name : {"rest-0", "rest-1", "language-0", "terrain-0", "terrain-1" }) {
        Director::getInstance()->getTextureCache()->addImageAsync(file_name + ".png", [&,file_name](Texture2D* loaded_texture) {
            SpriteFrameCache::getInstance()->addSpriteFramesWithFile(file_name + ".plist", loaded_texture);
            monitor->Update("Loaded " + file_name,1);
            count++;
            if ( count == 5 ) {
                std::lock_guard<std::mutex> guard(mutex);
                isDataLoaded = true;
                condition.notify_one();
            }
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }
    
    // wait for the above to finish
    std::unique_lock<std::mutex> mlock(mutex);
    condition.wait(mlock, std::bind(&moonring::isDataLoaded, this));
    isDataLoaded=false;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    
    RUN_ON_UI_THREAD([=](){
        // load shader
        auto p1 = GLProgram::createWithFilenames("terrain/standard.vsh", "terrain/dayNight.fsh");
        glProgramState = GLProgramState::getOrCreateWithGLProgram( p1 );
        glProgramState->setUniformVec4("p_left", Vec4(0,0,165.0/255.0,1));      // outline
        glProgramState->setUniformVec4("p_right", Vec4(1,1,1,1));               // body
        glProgramState->setUniformFloat("p_alpha", 1.0f);               // alpha
        monitor->Update("Loaded Shader", 1);
        
        std::lock_guard<std::mutex> guard(mutex);
        isDataLoaded = true;
        condition.notify_one();
        
    });
  
    // wait for the above to finish
    condition.wait(mlock, std::bind(&moonring::isDataLoaded, this));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    
    UIDEBUG("panel_splashscreens::Run() - load confog");
#if defined(_DDR_)
    project->LoadXmlConfig("ddr/ddr.tme", monitor );
#endif
#if defined(_LOM_)
    project->LoadXmlConfig("lom/lom.tme", monitor );
#endif
    

    // initialise TME
    TME_Init();
    
#ifdef _TME_CHEAT_MODE_
    UIDEBUG("Global:: _TME_CHEAT_MODE_");
    tme::variables::sv_cheat_armies_noblock = true;
    tme::variables::sv_cheat_nasties_noblock = true;
    tme::variables::sv_cheat_movement_free = true ;
    //variables::sv_cheat_movement_cheap = true ;
    //variables::sv_cheat_commands_free = true ;
#endif

#ifdef _TME_DEMO_MODE_
    UIDEBUG("Global:: _TME_DEMO_MODE_");
    tme::variables::sv_cheat_armies_noblock = true;
    tme::variables::sv_cheat_nasties_noblock = true;
    //variables::sv_cheat_movement_free = true ;
    //tme::variables::sv_cheat_movement_cheap = true ;
    //tme::variables::sv_cheat_commands_free = true ;
#endif
    
#if defined(_LOM_MAP_)
    auto builder = new TMEMapBuilder();
    mxmap* map = builder->Build( "lom_map.tmx" );
    TME_DebugInstallMap(map);
#endif
    
#if defined(_CITADEL_MAP_)
    auto builder = new TMEMapBuilderCitadel();
    mxmap* map = builder->Build( "citadel_wip.tmx" );
    TME_DebugInstallMap(map);
#endif
    
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



