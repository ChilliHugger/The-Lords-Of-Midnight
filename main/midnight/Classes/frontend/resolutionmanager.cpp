
#include "resolutionmanager.h"

//#include "2d/CCScene.h"
#include "base/CCDirector.h"
//#include "2d/CCCamera.h"
//#include "base/CCEventDispatcher.h"
//#include "base/CCEventListenerCustom.h"
//#include "base/ccUTF8.h"
//#include "renderer/CCRenderer.h"
//#include "renderer/CCFrameBuffer.h"



resolution_support  resolutions[] = {
    { 0, "gfx_h", 2048, 1536,   0, 0, 2.0, 1.0 },       // 3145728 ipad3
    { 1, "gfx_m", 1024, 768,    0, 1, 1.0, 1.0 },       // 786432 ipad
    { 2, "gfx_l",  512, 384,    0, 2, 0.5, 1.0 },       // 196608
    
    { 1, "gfx_m", 1136, 640,    2, 3, 1.109375, 1.0 },  // 727050 iphone5
    { 1, "gfx_m",  960, 640,    1, 1, 0.9375, 1.0 },    // 614400 iphone4
    { 2, "gfx_l",  480, 320,    1, 2, 0.46875, 1.0 },   // 153600 iphone 3g
    { 0, "gfx_h", 1920, 1080,   2, 0, 1.875, 1.0 },     // 2073600 1.7 iphone7/6+
    { 0, "gfx_h", 1334, 750 ,   2, 1, 1.302734, 1.0 },  // 1000500 1.7 iphone6/7
    { 0, "gfx_h", 2208, 1242,   2, 0, 2.15625, 1.0 },   // 2742336 1.7
    { 0, "gfx_h", 2001, 1125,   2, 0, 1.9541, 1.0 },    // 2742336 1.7
    { 0, "gfx_h", 2732, 2048,   2, 0, 2.66796, 1.0 },   // 5595136 ipad pro
    
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
        CC_SWAP( height, width, float );
    
    //UIDEBUG( "Height %d", (int)height);
    //UIDEBUG( "Width %d", (int)width);
    
    // too big aspect ratio?
    if ( (width/height) > 2.0 ) {
        width = height*1.8;
    }
    
    
    int dpi = Device::getDPI();
    float xInches = director->getWinSize().width / dpi;
    float yInches = director->getWinSize().height / dpi;
    float diagonalInches = sqrtf(xInches * xInches + yInches * yInches);
    diagonalInches = roundf(diagonalInches * 100.0f) / 100.0f;
    
    
    isTablet = (diagonalInches >= 7.0f);
    
    
    
    s32 res = -1 ;
    
    for ( u32 ii=0; ii<NUMELE(resolutions); ii++ ) {
        if ( width == resolutions[ii].width && height==resolutions[ii].height ) {
            res=ii;
            break;
        }
    }
    
    if ( res == -1 ) {
        
        s32 aspect=-1;
        
        if ( width >= 1536 ) {
            res=0;
        }else if ( width>512 ) {
            res=1;
        }else{
            res=2;
        }

        int best_width=99999;
        
        
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
        
        
        
        if ( aspect == -1 ) {
            //COMPLAIN("There is no available compatible resolution (%d,%d)", width,height);
        }
    
    }
    
    if ( res!=-1 ) {
        current_resolution.width = width;
        current_resolution.height = height;
        current_resolution.screen_scale = resolutions[res].screen_scale;
        current_resolution.target_scale = resolutions[res].target_scale;
        current_resolution.aspect = resolutions[res].aspect;
        current_resolution.folder = resolutions[res].folder;
        
        //ui->source_scale = resolutions[ui->target_device].screen_scale ;
        //ui->aspect_scale = (f32)width / resolutions[ui->target_device].width ;

    }
    
    return true;
}
