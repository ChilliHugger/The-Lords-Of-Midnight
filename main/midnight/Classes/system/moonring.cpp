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
    return moonring::mikesingleton()->serialize(version, ar );
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


LPCSTR moonring::getWritablePath()
{
    auto path = cocos2d::FileUtils::getInstance()->getWritablePath();
    
#if defined(_OS_DESKTOP_)
    path.append(TME_ScenarioName()).c_str();
#endif
    
    writeablepath = path.c_str();
    
    return (LPCSTR)writeablepath;
}

void moonring::showPage( panelmode_t mode, mxid object )
{
    panels->setPanelMode(mode);
    panels->currentPanel()->setObject(object);
}

// Stories

storyid_t moonring::startNewStory()
{
    //destroyGamePanels();

    config->setGameDefaults();
    
    selectmodel.setDefaults();
    
    storyid_t id = stories->alloc();
    
    stories->create(id);
    
    UIDEBUG("Resolve TME Data");
    
    tme->ResolveTMEData();
    tme->resetTMEData();
    
    //createGamePanels();
    
    help->Load( id );
    
    return id;
}

void moonring::closeStory()
{
    stories->save();
    stories->cleanup();
    showPage( MODE_MAINMENU, 0 );
}

storyid_t moonring::getCurrentStory()
{
    return stories->first_used_story();
}

void moonring::continueStory( storyid_t id )
{
    //destroyGamePanels();
    
    stories->load(id);
    help->Load( id );
    
    //createGamePanels();
    
    TME_CurrentCharacter( TME_CurrentCharacter().id );
    
    // start game
    if (!checkGameOverConditions() ) {
        character& c = TME_CurrentCharacter();
        auto mode = Character_IsDead(c) ? MODE_THINK : MODE_LOOK;
        
        panels->setPanelMode(mode,TRANSITION_PUSHUP);

        if ( mode == MODE_LOOK ) {
            panels->currentPanel()->setObject(c.id);
        }
        
    }
}

// Grouping

bool moonring::swapGroupLeader(mxid id)
{
    character c;
    TME_GetCharacter(c, id );
    if ( Character_SwapGroupLeader(c,c.following) ) {
        stories->save();
        return true;
    }
    return false;
}

bool moonring::joinGroup(mxid id, mxid leaderid )
{
    character c;
    TME_GetCharacter(c, id );
    if ( Character_Follow(c, leaderid) ) {
        stories->save();
        return true;
    }
    return false;
}

bool moonring::leaveGroup(mxid id)
{
    character c;
    TME_GetCharacter(c, id );
    if ( Character_UnFollow(c, c.following) ) {
        stories->save();
        return true;
    }
    return false;
}

bool moonring::disbandGroup(mxid id)
{
    character c;
    TME_GetCharacter(c, id );
    if ( Character_Disband(c) ) {
        stories->save();
        return true;
    }
    return false;
}
//

bool moonring::look()
{
    if ( checkGameOverConditions() )
        return true;
    
    if ( Character_IsDead(TME_CurrentCharacter()) )
        showPage ( MODE_SELECT );
    else
        showPage(MODE_LOOK, TME_CurrentCharacter().id);
    return true;
}

bool moonring::look( mxdir_t dir )
{
    character&    c = TME_CurrentCharacter();
    Character_Look(c, dir);
    return false;
}

bool moonring::selectCharacter(mxid id)
{
    character c;
    
    TME_GetCharacter(c,id);
    
    if ( Character_IsDead(c) ) {
        TME_SelectChar(c.id);
        showPage( MODE_THINK );
        stories->save();
        return true;
    }
    
    // change the current character
    TME_SelectChar(id);
    stories->save();
    
    TME_RefreshCurrentCharacter();
    TME_GetCharacterLocationInfo ( TME_CurrentCharacter() );

    panels->setPanelMode(MODE_LOOK,TRANSITION_FADEIN);
    panels->currentPanel()->setObject(TME_CurrentCharacter().id);
    
    return true;
}

bool moonring::undo( savemode_t mode )
{
    stories->undo(mode);
    TME_RefreshCurrentCharacter();
    TME_GetCharacterLocationInfo ( TME_CurrentCharacter() );
    return true;
}

bool moonring::night()
{
    stories->save(savemode_night);
    showPage( MODE_NIGHT );
    return true;
}

bool moonring::approach()
{
    character& c = TME_CurrentCharacter();
    
    mxid currentCharacter = TME_CurrentCharacter().id;
    
    // if the recruit is successful then
    // make the new character the current character
    
    if ( Character_Approach(c) ) {
        if ( currentCharacter != TME_CurrentCharacter().id ) {
            // TODO: Add new lord to select screen
            //mr->ShowPage(MODE_SELECT, TME_CurrentCharacter().id );
            //select->AddNewLord(TME_CurrentCharacter().id);
  
        }
    }else{
        TME_RefreshCurrentCharacter();
    }
    
    stories->save();
    showPage ( MODE_THINK );

    
    return true;
}

bool moonring::seek()
{
    character& c = TME_CurrentCharacter();
    mxid id = Character_Seek(c);
    stories->save();
    showPage(MODE_THINK_SEEK, id );
    return true;
}

bool moonring::hideunhide()
{
    character& c = TME_CurrentCharacter();
    if ( Character_Hide(c) ) {
        stories->save();
        showPage(MODE_THINK);
        return true;
    }
    return false;
}

bool moonring::fight()
{
    character& c = TME_CurrentCharacter();
    mxid fight = Character_Fight(c);
    if ( fight != IDT_NONE ) {
        stories->save();
        showPage ( MODE_THINK_FIGHT, fight );
        return true;
    }
    return false;
}

bool moonring::think()
{
    character& c = TME_CurrentCharacter();
    showPage(MODE_THINK_PERSON, Character_LocationObject(c));
    return true;
}

bool moonring::recruitMen()
{
    character& c = TME_CurrentCharacter();
    if ( Character_RecruitMen(c) ) {
        stories->save();
        showPage ( MODE_THINK_RECRUITGUARD );
        return true;
    }
    return false;
}

bool moonring::postMen()
{
    character& c = TME_CurrentCharacter();
    if ( Character_PostMen(c) ) {
        stories->save();
        showPage ( MODE_THINK_RECRUITGUARD );
        return true;
    }
    return false;
}

bool moonring::attack()
{
    character& c = TME_CurrentCharacter();
    if ( Character_Attack(c) ) {
        //setObject(characterId);
        stories->save();
        showPage(MODE_THINK_ARMY);
        return true;
    }else{
        TME_GetCharacterLocationInfo(c);
        if ( location_stubborn_lord_attack!=IDT_NONE ) {
            //setObject(location_stubborn_lord_attack);
            stories->save();
            showPage(MODE_THINK_PERSON);
            return true;
        }
    }
    return false;
}

#if defined(_DDR_)

bool moonring::give()
{
    character& c = TME_CurrentCharacter();
    if ( Character_Give(c,location_someone_to_give_to) ) {
        stories->save();
        showPage( MODE_THINK_PLACE );
        return true;
    }
    return false;
}

bool moonring::take()
{
    character& c = TME_CurrentCharacter();
    if ( Character_Take(c) ) {
        stories->save();
        showPage( MODE_THINK_PLACE );
        return true;
    }
    return false;
}

bool moonring::use()
{
    character& c = TME_CurrentCharacter();
    if ( Character_Use(c) ) {
        stories->save();
        showPage( MODE_THINK_SEEK );
        return true;
    }
    return false;
}

bool moonring::rest()
{
    character& c = TME_CurrentCharacter();
    Character_Rest(c);
    stories->save();
    showPage(MODE_THINK);
    return true;
}

bool moonring::enterTunnel()
{
    character& c = TME_CurrentCharacter();
    Character_EnterTunnel(c);
    stories->save();
    return true;
}

#endif

//
bool moonring::dawn()
{
    stories->save(savemode_dawn);
 
    TME_RefreshCurrentCharacter();
    
//    m_gameover_t gameover = TME_CheckWinLose();
//    
//    if ( gameover == MG_WIN ) {
//        showPage(MODE_WIN);
//        return true;
//    }
//
//    if ( gameover == MG_LOSE ) {
//        showPage(MODE_LOSE);
//        return true;
//    }
    
    if ( Character_IsDead(TME_CurrentCharacter()) ) {
        showPage( MODE_THINK );
        return true;
    }
    
    showPage(MODE_LOOK, TME_CurrentCharacter().id);
    
    return true;
}

// Game Over

bool moonring::checkGameOverConditions ( void )
{
    m_gameover_t win = TME_CheckWinLose();
    if ( win != MG_NONE ) {
        auto mode =  win == MG_WIN ? MODE_WIN : MODE_LOSE ;
        panels->setPanelMode(mode);
        return TRUE;
    }
    return FALSE;
}

bool moonring::serialize( u32 version, archive& ar )
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
        selectmodel.serialize(version, ar);
        ////p_think->Serialize(version, ar);
        
        // Version 10
        stories->Serialize(version, ar);
        
        
    }else{

        
        ar >> version ;
        
        //p_map->Serialize(version, ar);
        ////p_look->Serialize(version, ar);
        selectmodel.serialize(version, ar);
        ////p_think->Serialize(version, ar);
        
        if ( version>= 10 )
            stories->Serialize(version, ar);
        
    }
    
    return TRUE;
}



void moonring::initialise( progressmonitor* monitor )
{

    isDataLoaded = false;
    int count=0;
    
    // let's give up some time to the ui Immediatley
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // load images
    for (std::string file_name : {"rest-0", "rest-1", "language-0", "terrain/terrain-0", "terrain/terrain-1" }) {
        Director::getInstance()->getTextureCache()->addImageAsync(file_name + ".png", [&,file_name](Texture2D* loaded_texture) {
            
            auto cache = SpriteFrameCache::getInstance();
            
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
        glProgramState->setUniformVec4("p_left", Vec4(0,0,(165.0f/255.0f),alpha_normal));      // outline
        glProgramState->setUniformVec4("p_right", Vec4(1,1,1,alpha_normal));               // body
        glProgramState->setUniformFloat("p_alpha", alpha_normal);               // alpha
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
    //tme::variables::sv_cheat_movement_free = true ;
    tme::variables::sv_cheat_movement_cheap = true ;
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



