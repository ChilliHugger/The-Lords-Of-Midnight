#include "AppDelegate.h"
#include "system/resolutionmanager.h"
#include "system/moonring.h"
#include "panels/panel_splashscreen.h"

#include "tme/tme_interface.h"

// #define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace ax::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_AX;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

#if (_OS_DESKTOP_)
    glContextAttrs.vsync = true ;
#endif

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();
  
    auto mr = moonring::mikesingleton();
  
    //auto console = director->getConsole();
    //console->listenOnTCP(1234);

    mr->resolution->init();
    
    InitialisePaths();
    
    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = panel_splashscreen::create();
    if ( scene == nullptr )
        return false;
    
    // run
    director->runWithScene(scene);

    return true;
}

void AppDelegate::InitialisePaths()
{
    std::vector<std::string> searchPaths;
    
    auto fileutils = FileUtils::getInstance();

#ifdef _OS_WINDOWS_
    char path[MAX_PATH];
    chilli::os::filemanager::GetCurrentDirectory(MAX_PATH,path);
    fileutils->setDefaultResourceRootPath(path);
#endif

    auto res = moonring::mikesingleton()->resolution;
    

    auto scenario = TME_ScenarioShortName();
    auto resfolder = res->Folder();

    char main_path[MAX_PATH];
    
    snprintf( main_path, NUMELE(main_path), "%s/%s", scenario.c_str(), resfolder.c_str() );
    searchPaths.push_back(main_path);

    snprintf( main_path, NUMELE(main_path), "%s/%s/screens/%d", scenario.c_str(), resfolder.c_str(), res->Aspect() );
    searchPaths.push_back(main_path);

    snprintf( main_path, NUMELE(main_path), "all/%s",resfolder.c_str() );
    searchPaths.push_back(main_path);
    
    snprintf( main_path, NUMELE(main_path), "all/%s/screens/%d", resfolder.c_str(), res->Aspect() );
    searchPaths.push_back(main_path);
    
    
    fileutils->setSearchPaths(searchPaths);
}



// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}

#if defined(_OS_IOS_)
void AppDelegate::applicationScreenSizeChanged(int newWidth, int newHeight)
{
    ax::Application::applicationScreenSizeChanged(newWidth, newHeight);
}
#endif
