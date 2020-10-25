
#include "resolutionmanager.h"

#include "base/CCDirector.h"

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

static resolutionmanager* singleton_rm = NULL ;


resolutionmanager* resolutionmanager::getInstance()
{
    if ( singleton_rm == NULL ) {
        singleton_rm = new resolutionmanager() ;
    }
    return singleton_rm;
}

void resolutionmanager::release()
{
   SAFEDELETE(singleton_rm);
}

resolutionmanager::resolutionmanager ( void )
{
}

resolutionmanager::~resolutionmanager ( void )
{
}

bool resolutionmanager::init ( void )
{
    calcDisplayInfo();
    
    return true ;
}


bool resolutionmanager::calcDisplayInfo ( void )
{
    //uimanager* ui = uimanager::singleton();
    
//#if defined(_OS_DESKTOP_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_PHONE_)
//    t_global* gl = t_global::singleton();
//#endif
    
    auto director = Director::getInstance();
    
    auto size = director->getOpenGLView()->getFrameSize();
    
    float height = size.width;
    float width = size.height;
    
    // landscape
    if ( height > width )
        std::swap(height, width);
    
    //UIDEBUG( "Height %d", (int)height);
    //UIDEBUG( "Width %d", (int)width);
    
    // too big aspect ratio?
    if ( (width/height) > 2.5 ) {
        width = height*1.8;
    }
    
    
    int dpi = Device::getDPI();
    float xInches = director->getWinSize().width / dpi;
    float yInches = director->getWinSize().height / dpi;
    float diagonalInches = sqrtf(xInches * xInches + yInches * yInches);
    diagonalInches = roundf(diagonalInches * 100.0f) / 100.0f;
    
    
    isTablet = (diagonalInches >= 7.0f);
    
    
    
    s32 res = findPredefinedResolution(width, height) ;
    if ( res ) {
        auto resolution = &resolutions[res-1];
        current_resolution.width = width;
        current_resolution.height = height;
        current_resolution.screen_scale = resolution->screen_scale;
        current_resolution.content_scale = resolution->content_scale;
        current_resolution.aspect = resolution->aspect;
        current_resolution.folder = resolution->folder;
        current_resolution.aspect_scale = (f32)width / resolutions[resolution->reference].width ;
    }else{
        res = findNearestResolution(width, height);
        s32 aspect = findAspectRatio(width, height);

        auto resolution = &resolutions[res-1];
        current_resolution.width = width;
        current_resolution.height = height;
        current_resolution.screen_scale = width / 1024;
        current_resolution.content_scale = resolution->content_scale;
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
    s32 aspect=-1;
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
    result.left = safeAreaLeftPadding;
    result.top = safeAreaTopPadding;
    result.right = safeAreaRightPadding;
    result.bottom = safeAreaBottomPadding;
    
    //auto r = Director::getInstance()->getSafeAreaRect();
    
    return result;
}

f32 resolutionmanager::phoneScale()
{
#if !defined(_OS_DESKTOP_)
    return isTablet ? 1.0f : 1.4f;
#else
    return 1.0f;
#endif

}



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
