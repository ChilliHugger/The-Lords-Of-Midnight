
#include "resolutionmanager.h"
#include "moonring.h"
#include "settingsmanager.h"
#include "../platform/Extensions.h"

#include "base/Director.h"
#include "../ui/uielement.h"
#include "../ui/helper.h"

USING_NS_CC;

float safeAreaTopPadding = 0.0f;
float safeAreaBottomPadding =  0.0f;
float safeAreaLeftPadding = 0.0f;
float safeAreaRightPadding = 0.0f;

resolution_support  resolutions[] = {
//  reference   folder,     width,  height, aspect, font,   screen_scale,   content_scale
    { 0,        "gfx_h",    2048,   1536,   0,      0,      2.0f,           2.0f },  // 3145728 ipad3
    { 1,        "gfx_m",    1024,    768,   0,      1,      1.0f,           1.0f },  // 786432 ipad
    { 2,        "gfx_l",     512,    384,   0,      2,      0.5f,           0.5f },  // 196608
                            
    //{ 2,        "gfx_l",     480,    320,   1,      2,      0.46875f,       1.0f },  // 153600 iphone 3g
    { 1,        "gfx_m",     960,    640,   1,      1,      0.9375f,        1.0f },  // 614400 iphone4
    { 1,        "gfx_m",    1136,    640,   2,      3,      1.109375f,      1.0f },  // 727050 iphone5
    { 0,        "gfx_m",    1334,    750,   2,      1,      1.302734f,      1.0f },  // 1000500 1.7 iphone6/7
    { 0,        "gfx_m",    1792,    828,   2,      0,      1.75f,          2.0f },  // 1483776 iphone XR 6.1         // 2.16425121
    { 0,        "gfx_h",    1920,   1080,   2,      0,      1.875f,         2.0f },  // 2073600 1.7 iphone7/6+
    { 0,        "gfx_h",    2160,   1620,   2,      0,      2.109375f,      2.0f },  // iPad 7
    
    { 0,        "gfx_h",    2208,   1242,   2,      0,      2.15625f,       2.0f },  // 2742336 1.7
    { 0,        "gfx_h",    2224,   1668,   0,      0,      2.171875f,      2.0f },  // 3709632 ipad 10.5
    { 0,        "gfx_h",    2388,   1668,   0,      0,      2.33203125f,    2.0f },  //  ipad 11
    { 0,        "gfx_h",    2436,   1125,   2,      0,      2.37890625f,    2.0f },  // 2740500 iphoneX              // 2.1653
    { 0,        "gfx_h",    2688,   1242,   2,      0,      2.625f,         2.0f },  // 3338496 iphoneXS max         // 2.16425121
    { 0,        "gfx_h",    2732,   2048,   0,      0,      2.66796f,       2.0f },  // 5595136 ipad pro


//    iPhone XS: 5.8-inch OLED, 2436x1125, 458 ppi
//    iPhone XS Max: 6.5-inch OLED, 2688x1242, 458 ppi
//    iPhone XR: 6.1-inch LCD, 1792x828, 326 ppi
};

// TODO:: 1.25/0.8
f32 aspect_scales[] = { 1.3337f, 1.5f, 1.8f, 1.6665f };

resolutionmanager::resolutionmanager ( void )
{
    director = static_cast<CustomDirector*>(Director::getInstance());
}

resolutionmanager::~resolutionmanager ( void )
{
}

bool resolutionmanager::calcDisplayInfo ( void )
{
    auto size = director->getGLView()->getFrameSize();
    
    float height = size.width;
    float width = size.height;
    
    // landscape
    if ( height > width )
        std::swap(height, width);
    
    int dpi = Device::getDPI();
    float xInches = director->getWinSize().width / dpi;
    float yInches = director->getWinSize().height / dpi;
    float diagonalInches = sqrtf(xInches * xInches + yInches * yInches);
    diagonalInches = roundf(diagonalInches * 100.0f) / 100.0f;
    
#if !defined(_OS_DESKTOP_)
    isTablet = (diagonalInches >= 7.0f);
    isPhone = !isTablet;
#else
    isTablet = false;
    isPhone = false;
#endif
    
#if defined(TEST_PHONE_SCALE)
    isPhone = true;
    isTablet = false;
#endif
    
    
    s32 res = 1;

    {
        s32 aspect = findAspectRatio(width, height);

#if !defined(_OS_DESKTOP_)
        res = (width<2048) ? 2 : 1;
#endif
        
        auto resolution = &resolutions[res-1];
        current_resolution.width = width;
        current_resolution.height = height;
        current_resolution.graphics_scale = resolution->content_scale;
        
        current_resolution.screen_scale = height / 768;
        current_resolution.content_scale = (768.0*resolution->content_scale) / height;
        
        
        current_resolution.aspect = aspect-1;
        current_resolution.folder = resolution->folder;
        current_resolution.aspect_scale = (f32)width / resolutions[resolution->reference].width ;
    }
        
    CCLOG("Screen      (%f,%f)", current_resolution.width, current_resolution.height);
    CCLOG("Scale        %f", current_resolution.screen_scale);
    CCLOG("Target       %f", current_resolution.content_scale);
    CCLOG("Aspect       %u", current_resolution.aspect);
    CCLOG("Aspect Scale %f", current_resolution.aspect_scale);

    return true;
}

s32 resolutionmanager::findPredefinedResolution(f32 width, f32 height)
{
    for ( u32 ii=0; ii<NUMELE(resolutions); ii++ ) {
        if ( width == resolutions[ii].width && height==resolutions[ii].height ) {
            return ii+1;
        }
    }
    return 0;
}

s32 resolutionmanager::findNearestResolution(f32 width, f32 height)
{
    s32 res=-1;
    
    if ( width >= 1536 ) {
        res=0;          // high
    }else if ( width>512 ) {
        res=1;          // medium
    }else{
        res=2;          // low
    }

    return res+1;
}


s32 resolutionmanager::findAspectRatio(f32 width, f32 height)
{
    s32 aspect=0;
    s32 best_width=99999;

    for  ( u32 ii=0; ii<NUMELE(aspect_scales); ii++ )
    {
        u32 w = (f32)height*aspect_scales[ii];
        s32 diff  = width-w;
        if ( diff == 0 ) {
            aspect=ii;
            best_width=0;
            break;
        }
        
        if ( diff>0 ) {
            if ( diff<best_width ) {
                best_width=diff;
                aspect=ii;
            }
        }
    }
    
    return aspect+1;
}

padding resolutionmanager::getSafeArea()
{
    padding result;

    int orientation = 0;
    
#if defined(_OS_IOS_)
    orientation = chilli::extensions::getOrientation();
#endif

    auto r = director->getSafeAreaRect();
    
    result.left = (result.right = r.origin.x) / Scale();
    result.top = result.bottom = 0;
    
    if(orientation == 1) {
        result.right = 8;
    }
    if(orientation == 2) {
        result.left = 8;
    }
    
    return result;
}

f32 resolutionmanager::phoneScale()
{
#if defined(TEST_PHONE_SCALE)
    return PhoneScale;
#else
    #if !defined(_OS_DESKTOP_)
        return isTablet ? scale_normal : PhoneScale;
    #else
        return scale_normal;
    #endif
#endif

}

bool resolutionmanager::init()
{
    auto glview = director->getGLView();

    if(!glview)
    {
        
#if (_OS_DESKTOP_)
        
#if defined(DesktopDebugResolution)
        glview = setWindowedMode(DesktopDebugResolution);
#elif defined(DesktopDebugScreenMode)
        glview = setDisplayMode(DesktopDebugScreenMode);
#else
        glview = setDisplayMode(mr->settings->screen_mode);
#endif
        glview->setCursorVisible(false);
#else
        glview = GLViewImpl::create(TME_ScenarioName());
#endif
        director->setGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);
    
    calcDisplayInfo();

    auto windowSize = director->getGLView()->getFrameSize();
   
    glview->setDesignResolutionSize(windowSize.width, windowSize.height, ResolutionPolicy::EXACT_FIT);
    
    director->setContentScaleFactor(ContentScale());
    
    uihelper::initialiseFonts();

    return true;
}


#if defined(_OS_DESKTOP_)
size resolutionmanager::getDesktopSize()
{
    s32 width,height;
    chilli::extensions::getDesktopSize(width,height);
    return size(width,height);
}

GLView* resolutionmanager::setWindowedMode(cocos2d::Size size)
{
    return GLViewImpl::createWithRect(TME_ScenarioName(), cocos2d::Rect(0, 0, size.width, size.height));
}

Size resolutionmanager::calcWindowSize(f32 scale, f32 aspect)
{
    auto desktopSize = getDesktopSize();

    f32 height = desktopSize.cy * scale;
    f32 width = (int)ROUNDFLOAT(height * aspect);

    return cocos2d::Size(width,height);
}

GLView* resolutionmanager::setDisplayMode(CONFIG_SCREEN_MODE mode)
{
    f32 scale = 0.75;
    f32 aspect = 1.7777;

    switch(mode)
    {
        case CONFIG_SCREEN_MODE::CF_FULLSCREEN:
            return GLViewImpl::createWithFullScreen(TME_ScenarioName());
            break;
        
        case CONFIG_SCREEN_MODE::CF_WINDOW_SMALL:
            scale = 0.25f;
            break;
            
        case CONFIG_SCREEN_MODE::CF_WINDOW_MEDIUM:
            scale = 0.50f;
            break;
            
        case CONFIG_SCREEN_MODE::CF_WINDOW_LARGE:
            scale=0.75f;
            break;
    }
 
    auto windowSize = calcWindowSize(scale, aspect);

    return setWindowedMode(windowSize);

}

bool resolutionmanager::changeDisplayMode(CONFIG_SCREEN_MODE mode)
{
    Size windowSize;
    f32 scale = 0.0;
    f32 aspect = 1.7777;

    auto glView = dynamic_cast<GLViewImpl*>(director->getGLView());
    
    switch(mode)
    {
        case CONFIG_SCREEN_MODE::CF_FULLSCREEN:
            glView->setFullscreen();
            windowSize = director->getGLView()->getFrameSize();
            break;

        case CONFIG_SCREEN_MODE::CF_WINDOW_SMALL:
            scale = 0.25f;
            break;
            
        case CONFIG_SCREEN_MODE::CF_WINDOW_MEDIUM:
            scale = 0.50f;
            break;
            
        case CONFIG_SCREEN_MODE::CF_WINDOW_LARGE:
            scale=0.75f;
            break;
    }

    
    if(mode!=CONFIG_SCREEN_MODE::CF_FULLSCREEN)
    {
        windowSize = calcWindowSize(scale, aspect);
        glView->setWindowed(windowSize.width,windowSize.height);
    }
    
    glView->setDesignResolutionSize(windowSize.width, windowSize.height, ResolutionPolicy::EXACT_FIT);
    
    // calculate scales, aspect ratios, and assets
    calcDisplayInfo();
    
    director->setContentScaleFactor(ContentScale());
   
    director->purgeCachedData();

    mr->reloadAssets();
  
    // Setup fonts
    uihelper::initialiseFonts();

    return true;
}

#endif



/*
 
                                    Pixel Size    Viewport
 iPhone
 iPhone XR                           828 x 1792    414 x 896
 iPhone XS                          1125 x 2436    375 x 812
 iPhone XS Max                      1242 x 2688    414 x 896
 iPhone X                           1125 x 2436    375 x 812
 iPhone 8 Plus                      1080 x 1920    414 x 736
 iPhone 8                            750 x 1334    375 x 667
 iPhone 7 Plus                      1080 x 1920    414 x 736
 iPhone 7                            750 x 1334    375 x 667
 iPhone 6 Plus/6S Plus              1080 x 1920    414 x 736
 iPhone 6/6S                         750 x 1334    375 x 667
 iPhone 5                            640 x 1136    320 x 568
 iPod
 iPod Touch                          640 x 1136    320 x 568
 iPad
 iPad Pro                           2048 x 2732   1024 x 1366
 iPad Third & Fourth Generation     1536 x 2048    768 x 1024
 iPad Air 1 & 2                     1536 x 2048    768 x 1024
 iPad Mini 2 & 3                    1536 x 2048    768 x 1024
 iPad Mini                           768 x 1024    768 x 1024
  

 ss_androidAndroid Devices
 Pixel Size    Viewport
 Phones
 Nexus 6P                           1440 x 2560    412 x 732
 Nexus 5X                           1080 x 1920    412 x 732
 Google Pixel 3 XL                  1440 x 2960    412 x 847
 Google Pixel 3                     1080 x 2160    412 x 824
 Google Pixel 2 XL                  1440 x 2560    412 x 732
 Google Pixel XL                    1440 x 2560    412 x 732
 Google Pixel                       1080 x 1920    412 x 732
 Samsung Galaxy Note 9              1440 x 2960    360 x 740
 Samsung Galaxy Note 5              1440 x 2560    480 x 853
 LG G5                              1440 x 2560    480 x 853
 One Plus 3                         1080 x 1920    480 x 853
 Samsung Galaxy S9+                 1440 x 2960    360 x 740
 Samsung Galaxy S9                  1440 x 2960    360 x 740
 Samsung Galaxy S8+                 1440 x 2960    360 x 740
 Samsung Galaxy S8                  1440 x 2960    360 x 740
 Samsung Galaxy S7 Edge             1440 x 2560    360 x 640
 Samsung Galaxy S7                  1440 x 2560    360 x 640
 Tablets
 Nexus 9                            1536 x 2048    768 x 1024
 Nexus 7 (2013)                     1200 x 1920    600 x 960
 Samsung Galaxy Tab 10              800 x 1280    800 x 1280
 Chromebook Pixel                   2560 x 1700    1280 x 850

 */
