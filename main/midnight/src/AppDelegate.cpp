#include "AppDelegate.h"
#include "system/resolutionmanager.h"

#include "panels/panel_splashscreen.h"

#include "tme/tme_interface.h"

// #define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;

constexpr bool IsDesktopFullScreen = false;
constexpr f32 DesktopAspectRatio = 1.7777; // 16:9
constexpr f32 DesktopWindowSizeScale = 0.75;
//#define DesktopDebugResolution GooglePixel3XL

static cocos2d::Size desktopResolutionSize =  iPhoneXR; //cocos2d::Size(2848, 1536); //cocos2d::Size(1024*3, 768*2);

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

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto res = resolutionmanager::getInstance();
  
    //auto console = director->getConsole();
    //console->listenOnTCP(1234);

    auto glview = director->getOpenGLView();
    if(!glview) {
        
#if (_OS_DESKTOP_)
        if(IsDesktopFullScreen)
        {
            res->setDisplayMode(CONFIG_SCREEN_MODE::CF_FULLSCREEN);
        }
        else
        {
#if defined(DesktopDebugResolution)
            glview = res->setWindowedMode(DesktopDebugResolution);
#else
            glview = res->setWindowedMode(DesktopWindowSizeScale, DesktopAspectRatio);
#endif
        }
        //glview->setCursorVisible(false);
#else
        glview = GLViewImpl::create(TME_ScenarioName());
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);
    
    res->init();

    director->setContentScaleFactor(res->ContentScale());
       
#if defined(_OS_DESKTOP_) && defined(MX_DEBUG)
    if ( desktopResolutionSize.height >= 1440 ) {
    //    glview->setFrameZoomFactor(0.5);
    }
#endif
    
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

    auto res = resolutionmanager::getInstance();
    

    LPCSTR scenario = TME_ScenarioShortName();
    LPCSTR resfolder = res->Folder().c_str();

    char main_path[MAX_PATH];
    
    sprintf( main_path, "%s/%s", scenario, resfolder );
    searchPaths.push_back(main_path);

    sprintf( main_path, "%s/%s/screens/%d", scenario, resfolder, res->Aspect() );
    searchPaths.push_back(main_path);

    sprintf( main_path, "all/%s",resfolder );
    searchPaths.push_back(main_path);
    
    sprintf( main_path, "all/%s/screens/%d", resfolder, res->Aspect() );
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
