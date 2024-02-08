//
//  resolutionmanager.h
//  TestHarness
//
//  Created by Chris Wild on 28/06/2017.
//
//

#ifndef resolutionmanager_h
#define resolutionmanager_h

#include "../cocos.h"
#include "../library/inc/mxtypes.h"
#include "../extensions/CustomDirector.h"

#include "settingsmanager.h"

using namespace chilli::types;

typedef struct {
    s32     reference;
    std::string   folder;
    f32     width;
    f32     height;
    u32     aspect;
    u32     fontset;
    f32     screen_scale;
    f32     content_scale;
    f32     aspect_scale;
    f32     graphics_scale;
} resolution_support ;

typedef struct {
    f32 left;
    f32 top;
    f32 right;
    f32 bottom;
} padding;


class resolutionmanager : public ringcontroller
{
public:
    resolutionmanager();
    virtual ~resolutionmanager();
    
    bool init();
    bool deinit();
    

    bool calcDisplayInfo();

    f32 Width() { return current_resolution.width; }
    f32 Height() { return current_resolution.height; }
    u32 Aspect() { return current_resolution.aspect; }
    f32 AspectScale() { return current_resolution.aspect_scale; }
    f32 Scale() { return current_resolution.screen_scale; }
    f32 ContentScale() { return current_resolution.content_scale; }
    f32 GraphicsScale() { return current_resolution.graphics_scale; }
    std::string& Folder() { return current_resolution.folder; }
  
    f32 phoneScale();


    
#if defined(_OS_DESKTOP_)
    bool IsDesktop() { return true; }
    size getDesktopSize();
    bool changeDisplayMode(CONFIG_SCREEN_MODE mode);
    cocos2d::GLView* setDisplayMode(CONFIG_SCREEN_MODE mode);
    cocos2d::GLView* setWindowedMode(cocos2d::Size size);
    cocos2d::Size calcWindowSize(f32 scale, f32 aspect);
    
#else
    bool IsDesktop() { return false; }
#endif

    bool IsPhone() { return isPhone; }
    bool IsTablet() { return isTablet; }
    bool IsPhoneScaleEnabled() { return isPhone && phoneScale() != 1.0f; }

    
    padding getSafeArea();
    
protected:
    s32 findPredefinedResolution(f32 width, f32 height);
    s32 findNearestResolution(f32 width, f32 height);
    s32 findAspectRatio(f32 width, f32 height);

private:
    
    CustomDirector* director;
    resolution_support current_resolution;
    
    bool isTablet;
    bool isPhone;
  
    CONFIG_SCREEN_MODE currentMode;
    
};

#define RES(x) \
    (f32)((x)*moonring::mikesingleton()->resolution->Scale())

#define NORES(x) \
    (x)

#define CONTENT_SCALE(x) \
    (f32)    ((x)*moonring::mikesingleton()->resolution->ContentScale())

#define PHONE_SCALE(x) (f32) \
    ((x)*moonring::mikesingleton()->resolution->phoneScale())

//#define TEST_PHONE_SCALE

constexpr f32 PhoneScale = 1.6f;


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

#if defined(_OS_DESKTOP_)
#define DesktopDebugScreenMode CONFIG_SCREEN_MODE::CF_WINDOW_LARGE
#define DesktopDebugResolution iPad3
#define _SWITCH_VIDEO_IMPLEMENTED_
#endif



#endif /* resolutionmanager_h */
