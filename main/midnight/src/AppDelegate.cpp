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



static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);


// iOS

static cocos2d::Size iPad1 = cocos2d::Size(1024,768);
static cocos2d::Size iPad2 = iPad1;
static cocos2d::Size iPad3 = cocos2d::Size(2048, 1536);
static cocos2d::Size iPadMini = iPad1;
static cocos2d::Size iPad4 = iPad3;
static cocos2d::Size iPadAir = iPad3;
static cocos2d::Size iPadMini2 = iPad3;
static cocos2d::Size iPadMini3 = iPad3;
static cocos2d::Size iPadAir2 = iPad3;
static cocos2d::Size iPadMini4 = iPad3;
static cocos2d::Size iPadPro12Inch = cocos2d::Size(2732, 2048);
static cocos2d::Size iPadPro9Inch = iPad3;
static cocos2d::Size iPad5 = iPad3;
static cocos2d::Size iPadPro10Inch = cocos2d::Size(2224, 1668);
static cocos2d::Size iPad6 = iPad3;
static cocos2d::Size iPadPro11Inch = cocos2d::Size(2388,1668);
static cocos2d::Size ipadAir3 = iPadPro10Inch;
static cocos2d::Size ipadMini5 = iPad3;
static cocos2d::Size iPad7 = cocos2d::Size(2160,1620);

static cocos2d::Size iPhone1 = cocos2d::Size(480, 320);
static cocos2d::Size iPhone3g = iPhone1;
static cocos2d::Size iPhone3gs = iPhone1;
static cocos2d::Size iPhone4 = cocos2d::Size(960, 640);
static cocos2d::Size iPhone4s = iPhone4;
static cocos2d::Size iPhone5 = cocos2d::Size(1136, 640);
static cocos2d::Size iPhone5s = iPhone5;
static cocos2d::Size iPhone5c = iPhone5;
static cocos2d::Size iPhoneSE = iPhone5;
static cocos2d::Size iPhone6 = cocos2d::Size(1334, 750);
static cocos2d::Size iPhone6s = iPhone6;
static cocos2d::Size iPhone6Plus = cocos2d::Size(1920, 1080);
static cocos2d::Size iPhone6sPlus = iPhone6Plus;
static cocos2d::Size iPhone7 = iPhone6;
static cocos2d::Size iPhone7Plus = iPhone6Plus;
static cocos2d::Size iPhone8 = iPhone6;
static cocos2d::Size iPhone8Plus = iPhone6Plus;
static cocos2d::Size iPhoneX = cocos2d::Size(2436, 1125);
static cocos2d::Size iPhoneXR = cocos2d::Size(1792, 828);
static cocos2d::Size iPhoneXS = iPhoneX;
static cocos2d::Size iPhone11 = iPhoneXR;
static cocos2d::Size iPhone11Pro = iPhoneX;
static cocos2d::Size iPhone11ProMax = cocos2d::Size(2688, 1242);
static cocos2d::Size iPhone12 = cocos2d::Size(2532, 1170);
static cocos2d::Size iPhone12Mini = cocos2d::Size(2340, 1080);
static cocos2d::Size iPhone12ProMax = cocos2d::Size(2778, 1284);
static cocos2d::Size iPhone12Pro = iPhone12;
static cocos2d::Size iPhoneSE2ndGen = iPhone6;



// Android
static cocos2d::Size GooglePixel3 = cocos2d::Size(2160, 1080);    // 2.05
static cocos2d::Size GooglePixel3XL = cocos2d::Size(2960, 1440);    // 2.05
static cocos2d::Size Nexus7 = cocos2d::Size(1920, 1200);    // 1.6
static cocos2d::Size SamsungGalaxyTab10 = cocos2d::Size(1280, 800);    // 1.6


static cocos2d::Size DesktopDefault = cocos2d::Size(2560, 1440);        // 1.7777
static cocos2d::Size DesktopModeSpace1 = cocos2d::Size(2880, 1620);    // 1.7777
static cocos2d::Size DesktopModeSpace2 = cocos2d::Size(3200, 1800);    // 1.7777
static cocos2d::Size DesktopLargerText1 = cocos2d::Size(2048, 1152);    // 1.7777
static cocos2d::Size DesktopLargerText2 = cocos2d::Size(1600, 900);    // 1.7777



static cocos2d::Size desktopResolutionSize = iPadPro12Inch;
static cocos2d::Size designResolutionSize = desktopResolutionSize; //cocos2d::Size(1024, 768);

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
    
    //auto console = director->getConsole();
    //console->listenOnTCP(1234);
    
    
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect(TME_ScenarioName(), cocos2d::Rect(0, 0, desktopResolutionSize.width, desktopResolutionSize.height));
        
        //glview = GLViewImpl::createWithFullScreen(TME_ScenarioName());
        
#else
        glview = GLViewImpl::create(TME_ScenarioName());
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    //glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_HEIGHT);
    
    
    auto res = resolutionmanager::getInstance();
    res->init();

    //director->setContentScaleFactor(res->ContentScale());
    
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
    LPCSTR resfolder = res->current_resolution.folder.c_str();

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
