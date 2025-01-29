//
//  resolutionmanager.h
//  TestHarness
//
//  Created by Chris Wild on 28/06/2017.
//
//

#ifndef resolutionmanager_h
#define resolutionmanager_h

#include "../axmol_sdk.h"
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
    bool changeDisplayMode(CF_SCREEN mode);
    ax::GLView* setDisplayMode(CF_SCREEN mode);
    ax::GLView* setWindowedMode(ax::Size size);
    ax::Size calcWindowSize(f32 scale, f32 aspect);
    
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
  
    CF_SCREEN currentMode;
    
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

static ax::Size iPad1 = ax::Size(1024,768);
static ax::Size iPad2 = iPad1;
static ax::Size iPad3 = ax::Size(2048, 1536);
static ax::Size iPadMini = iPad1;
static ax::Size iPad4 = iPad3;
static ax::Size iPadAir = iPad3;
static ax::Size iPadMini2 = iPad3;
static ax::Size iPadMini3 = iPad3;
static ax::Size iPadAir2 = iPad3;
static ax::Size iPadMini4 = iPad3;
static ax::Size iPadPro12Inch = ax::Size(2732, 2048);
static ax::Size iPadPro9Inch = iPad3;
static ax::Size iPad5 = iPad3;
static ax::Size iPadPro10Inch = ax::Size(2224, 1668);
static ax::Size iPad6 = iPad3;
static ax::Size iPadPro11Inch = ax::Size(2388,1668);
static ax::Size ipadAir3 = iPadPro10Inch;
static ax::Size ipadMini5 = iPad3;
static ax::Size iPad7 = ax::Size(2160,1620);

static ax::Size iPhone1 = ax::Size(480, 320);
static ax::Size iPhone3g = iPhone1;
static ax::Size iPhone3gs = iPhone1;
static ax::Size iPhone4 = ax::Size(960, 640);
static ax::Size iPhone4s = iPhone4;
static ax::Size iPhone5 = ax::Size(1136, 640);
static ax::Size iPhone5s = iPhone5;
static ax::Size iPhone5c = iPhone5;
static ax::Size iPhoneSE = iPhone5;
static ax::Size iPhone6 = ax::Size(1334, 750);
static ax::Size iPhone6s = iPhone6;
static ax::Size iPhone6Plus = ax::Size(1920, 1080);
static ax::Size iPhone6sPlus = iPhone6Plus;
static ax::Size iPhone7 = iPhone6;
static ax::Size iPhone7Plus = iPhone6Plus;
static ax::Size iPhone8 = iPhone6;
static ax::Size iPhone8Plus = iPhone6Plus;
static ax::Size iPhoneX = ax::Size(2436, 1125);
static ax::Size iPhoneXR = ax::Size(1792, 828);
static ax::Size iPhoneXS = iPhoneX;
static ax::Size iPhone11 = iPhoneXR;
static ax::Size iPhone11Pro = iPhoneX;
static ax::Size iPhone11ProMax = ax::Size(2688, 1242);
static ax::Size iPhone12 = ax::Size(2532, 1170);
static ax::Size iPhone12Mini = ax::Size(2340, 1080);
static ax::Size iPhone12ProMax = ax::Size(2778, 1284);
static ax::Size iPhone12Pro = iPhone12;
static ax::Size iPhoneSE2ndGen = iPhone6;

// Android
static ax::Size GooglePixel3 = ax::Size(2160, 1080);    // 2.05
static ax::Size GooglePixel3XL = ax::Size(2960, 1440);    // 2.05
static ax::Size Nexus7 = ax::Size(1920, 1200);    // 1.6
static ax::Size SamsungGalaxyTab10 = ax::Size(1280, 800);    // 1.6


static ax::Size DesktopDefault = ax::Size(2560, 1440);        // 1.7777
static ax::Size DesktopModeSpace1 = ax::Size(2880, 1620);    // 1.7777
static ax::Size DesktopModeSpace2 = ax::Size(3200, 1800);    // 1.7777
static ax::Size DesktopLargerText1 = ax::Size(2048, 1152);    // 1.7777
static ax::Size DesktopLargerText2 = ax::Size(1600, 900);    // 1.7777

#if defined(_OS_DESKTOP_)
//#define DesktopDebugScreenMode CONFIG_SCREEN_MODE::LARGE
//#define DesktopDebugResolution iPad3
#define _SWITCH_VIDEO_IMPLEMENTED_
#endif



#endif /* resolutionmanager_h */
