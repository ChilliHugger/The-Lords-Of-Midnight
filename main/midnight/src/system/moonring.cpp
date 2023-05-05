//
//  moonring.cpp
//  midnight
//
//  Created by Chris Wild on 07/12/2017.
//

#include "../panels/panel_look.h"
#include "../panels/panel_think.h"
#include "../platform/Extensions.h"

#include "moonring.h"
#include "helpmanager.h"
#include "settingsmanager.h"
#include "storymanager.h"
#include "keyboardmanager.h"
#include "tmemanager.h"
#include "panelmanager.h"
#include "projectconfig.h"
#include "progressmonitor.h"
#include "configmanager.h"
#include "shadermanager.h"
#include "../frontend/language.h"
#include <string>

#if defined(_USE_VERSION_CHECK_)
    #include "network/HttpClient.h"
    #include "network/HttpRequest.h"
    using namespace cocos2d::network;
#endif

USING_NS_CC;

using namespace chilli::lib;

static bool mySerialize ( u32 version, chilli::lib::archive& ar )
{
    return moonring::mikesingleton()->serialize(version, ar );
}

static moonring* singleton_mr = nullptr ;

moonring* moonring::mikesingleton()
{
    if ( singleton_mr == nullptr ) {
        singleton_mr = new moonring() ;
    }
    return singleton_mr;
}

void moonring::release()
{
   SAFEDELETE(singleton_mr);
}


moonring::moonring() :
#if defined(_USE_VERSION_CHECK_)
    isUpdateAvailable(false),
    versionCheckCompleted(false),
    updateUrl(""),
#endif
    writeablepath(""),
    isDataLoaded(false)
{
    resolution = new resolutionmanager();
    resolution->InjectMoonRing(this);
    
    shader = new shadermanager();
    shader->InjectMoonRing(this);
    
    help = new helpmanager();
    help->InjectMoonRing(this);
    
    settings = new settingsmanager();
    settings->InjectMoonRing(this);

    stories = new storymanager();
    stories->InjectMoonRing(this);

    keyboard = new keyboardmanager();
    tme = new tmemanager();
    
    panels = new panelmanager();
    panels->InjectMoonRing(this);
    
    project = new projectconfig();
    project->InjectMoonRing(this);
    
    config = new configmanager();
    config->InjectMoonRing(this);
    
    stories->SetLoadSave(&mySerialize);
    
    settings->Load();
    
    keyboard->SetKeyboardMode((CONFIG_KEYBOARD_MODE)settings->keyboard_mode);

}



moonring::~moonring()
{
    SAFEDELETE(config);
    SAFEDELETE(project);
    SAFEDELETE(panels);
    SAFEDELETE(tme);
    SAFEDELETE(keyboard);
    SAFEDELETE(stories);
    SAFEDELETE(settings);
    SAFEDELETE(help);
    SAFEDELETE(shader);
    SAFEDELETE(resolution);
}


std::string moonring::getWritablePath()
{
    if(!writeablepath.empty())
    {
        return writeablepath;
    }
    
    std::string path;
    
#if defined(_OS_OSX_)
    path = chilli::extensions::getApplicationSupportPath();
    path += "com.chillihugger.midnight/";
#else
    path = cocos2d::FileUtils::getInstance()->getWritablePath();
#endif

#if defined(_OS_DESKTOP_)
    path += TME_ScenarioName();
#endif
    
    writeablepath = path;
    
    return writeablepath;
}

void moonring::showPage( panelmode_t mode, mxid object )
{
    panels->setPanelMode(mode,TRANSITION_FADEIN);
    panels->currentPanel()->setObject(object);
}

// Stories

storyid_t moonring::startNewStory()
{
    selectmodel.setDefaults();
    mapmodel.setDefaults();
    
    storyid_t id = stories->alloc();
    if(id == STORY_NONE)
    {
        return id;
    }
    
    stories->create(id, settings->game_rules, settings->game_difficulty );
    
    UIDEBUG("Resolve TME Data");
    
    tme->ResolveTMEData();
    tme->resetTMEData();
    
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
    stories->load(id);
    help->Load( id );
    
    // setup any options
    tme::variables::sv_auto_seek = settings->autoseek;
    tme::variables::sv_auto_approach = settings->autoapproach;
    
    TME_CurrentCharacter( TME_CurrentCharacter().id );
    
    // start game
    if (!checkGameOverConditions() ) {
        character& c = TME_CurrentCharacter();
        
        if ( Character_IsDead(c) ) {
            TME_SelectChar(c.id);
            showPage( MODE_THINK );
            return;
        }
     
        auto mode = MODE_LOOK;
     
#if defined(_DEBUG_GAME_PANEL_)
#define IS_DEBUG_PANEL(x)       c_stricmp(config->start_on_panel,x) == 0

        if(!config->start_on_panel.IsEmpty())
        {
            if(IS_DEBUG_PANEL("MODE_LOOK"))
                mode = MODE_LOOK;
            else if(IS_DEBUG_PANEL("MODE_THINK"))
                mode = MODE_THINK;
            else if(IS_DEBUG_PANEL("MODE_MAP_DETAILED"))
                mode = MODE_MAP_DETAILED;
            
            if(mode==MODE_THINK)
            {
                TME_SelectChar(c.id);
                showPage( MODE_THINK );
                return;
            }
        }
#endif
        
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
    if(dir == DR_NONE) {
        return false;
    }
    
    character&    c = TME_CurrentCharacter();
    Character_Look(c, dir);
    showPage(MODE_LOOK,TME_CurrentCharacter().id);
    return true;
}

bool moonring::selectCharacter(mxid id)
{
    character c;
    
    TME_GetCharacter(c,id);
    
    if ( Character_IsDead(c) ) {
        TME_SelectChar(c.id);
        showPage( MODE_THINK );
        return true;
    }
    
    // change the current character
    TME_SelectChar(id);
    TME_RefreshCurrentCharacter();
    TME_GetCharacterLocationInfo ( TME_CurrentCharacter() );

    showPage(MODE_LOOK,TME_CurrentCharacter().id);
    
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
    mxid newId = c.id;

    if (Character_Approach(c)) {
        return afterApproach();
    }
    
    stories->save();
    TME_RefreshCurrentCharacter();
    showPage ( MODE_THINK );
    return true;
}

bool moonring::afterApproach()
{
    stories->save();
    
    character& c = TME_CurrentCharacter();

    switch (settings->approach_mode) {
        case CF_APPROACH_SWAP:
            TME_SelectChar(c.lastcommandid);
            showPage ( MODE_THINK_APPROACH );
            break;

        case CF_APPROACH_STAY:
            TME_RefreshCurrentCharacter();
            showPage ( MODE_THINK_APPROACH );
            break;
    }
        
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

bool moonring::hide()
{
    character& c = TME_CurrentCharacter();
    if ( Character_Hide(c) ) {
        stories->save();
        showPage(MODE_THINK);
        return true;
    }
    return false;
}

bool moonring::unhide()
{
    character& c = TME_CurrentCharacter();
    if ( Character_UnHide(c) ) {
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
#if defined(_DDR_)
    showPage(MODE_THINK_PLACE);
#else
    character& c = TME_CurrentCharacter();
    showPage(MODE_THINK_PERSON, Character_LocationObject(c));
#endif
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
#endif // DDR

#if defined(_TUNNELS_)
bool moonring::enterTunnel()
{
    character& c = TME_CurrentCharacter();
    Character_EnterTunnel(c);
    stories->save();
    look();
    return true;
}
#endif // _TUNNELS_

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
        
        mapmodel.serialize(version, ar);
        selectmodel.serialize(version, ar);
        
        // Version 10
        stories->Serialize(version, ar);
        
        
    }else{

        ar >> version ;
        
        mapmodel.serialize(version, ar);
        selectmodel.serialize(version, ar);
        
        if ( version>= 10 )
            stories->Serialize(version, ar);
    }
    
    return TRUE;
}


// TODO: Move these to a resource manager

std::string imageFiles[] = {
    "rest-0", "rest-1",
    "language-0",
    "terrain/terrain-0", "terrain/terrain-1",
    "terrain/tunnels-0"
};


#if defined(_OS_DESKTOP_)
void moonring::reloadAssets()
{
    auto cache = SpriteFrameCache::getInstance();
    for (std::string file_name : imageFiles )
    {
        auto plist = file_name + ".plist";
        cache->removeSpriteFramesFromFile(plist);
        cache->addSpriteFramesWithFile(plist);
    }
}
#endif

void moonring::initialise( progressmonitor* monitor )
{

    isDataLoaded = false;
    int count=0;
    int delay=100;
    
    // let's give up some time to the ui Immediatley
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));

    // load images
    for (std::string file_name : imageFiles ) {
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
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));

    }
    
    // wait for the above to finish
    std::unique_lock<std::mutex> mlock(mutex);
    condition.wait(mlock, std::bind(&moonring::isDataLoaded, this));
    isDataLoaded=false;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));

    
    RUN_ON_UI_THREAD([=](){
        
        // load shader
        shader->Init();
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
    TME_Init(RF_DEFAULT, DF_NORMAL);
    
    std::string configFilename = std::string( getWritablePath() ) + "/config.cfg";
    config->LoadXmlConfig( configFilename );
    
#ifdef _TME_CHEAT_MODE_
    UIDEBUG("Global:: _TME_CHEAT_MODE_");
    tme::variables::sv_cheat_armies_noblock = true;
    tme::variables::sv_cheat_nasties_noblock = true;
    tme::variables::sv_cheat_movement_free = true ;
    tme::variables::sv_cheat_always_win_fight = true;
    //variables::sv_cheat_commands_free = true ;
#endif

#ifdef _TME_DEMO_MODE_
    UIDEBUG("Global:: _TME_DEMO_MODE_");
    tme::variables::sv_cheat_armies_noblock = true;
    tme::variables::sv_cheat_nasties_noblock = true;
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

#if defined(_OS_DESKTOP_)
void moonring::changeDisplayMode()
{
    auto screenMode = settings->screen_mode ;
    
    if ( !settings->fullscreensupported
        && screenMode == CONFIG_SCREEN_MODE::CF_FULLSCREEN)
    {
        return;
    }

    if ( resolution->changeDisplayMode( screenMode ) )
    {
        settings->Save();
        
        resolution->calcDisplayInfo();
        
        panels->setPanelMode(MODE_MAINMENU);
    }
}
#endif



void moonring::complain (LPCSTR format, ... )
{
    static char msg_buffer[1024];

    va_list arglist ;
    
    va_start( arglist, format ) ;
    vsnprintf( msg_buffer, NUMELE(msg_buffer), format, arglist );
    va_end( arglist ) ;
    
    cocos2d::log("%s",msg_buffer);
    
    exit(-1);
}


void  moonring::log(LPCSTR format, ...)
{
    static char msg_buffer[1024];

    va_list arglist;

    va_start(arglist, format);
    vsnprintf(msg_buffer, NUMELE(msg_buffer), format, arglist);
    va_end(arglist);

    //CCLOG("UI: %s", msg_buffer);
    cocos2d::log("UI: %s", msg_buffer);
}


#if defined(_USE_VERSION_CHECK_)
void moonring::getVersion(const GetVersionCallback& callback)
{
    if(callback==nullptr)
    {
        return;
    }

    if(versionCheckCompleted)
    {
        callback(isUpdateAvailable,updateUrl);
        return;
    }

    // TODO:
    // Ideally this should be cached and maybe only downloaded
    // once a day etc...

    auto request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::GET);
    request->setUrl(_VERSION_URL);
    request->setResponseCallback([&,callback](HttpClient* client, HttpResponse* response)
    {
        try
        {
            if(response->isSucceed())
            {
                auto data = response->getResponseData();
                auto input = std::string( data->begin(), data->end() );
                auto lines = StringExtensions::split_by_newline(input);
                UIDEBUG("Version Check: %s (%s)",lines[0].c_str(), lines[1].c_str());
               
                auto buildNo = chilli::extensions::getBuildNo();
                auto availableBuildNo = StringExtensions::atoi(lines[1]);
                auto currentBuildNo =  StringExtensions::atoi(buildNo);
                
                isUpdateAvailable = currentBuildNo<availableBuildNo;
                updateUrl = lines[2];
                versionCheckCompleted = true;
                callback(isUpdateAvailable,updateUrl);
            }
        } catch (const std::exception& e) {
        }
    });

    HttpClient::getInstance()->send(request);
    request->release();
}
#endif
