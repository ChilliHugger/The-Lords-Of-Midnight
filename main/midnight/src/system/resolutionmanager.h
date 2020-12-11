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


class resolutionmanager
{
private:
    resolutionmanager();
    ~resolutionmanager();
    
public:
    
    static resolutionmanager* getInstance();
    static void release();
    
    bool init();
    bool deinit();
    
    bool calcDisplayInfo();

    f32 Width() { return current_resolution.width; } ;
    f32 Height() { return current_resolution.height; } ;
    u32 Aspect() { return current_resolution.aspect; } ;
    f32 AspectScale() { return current_resolution.aspect_scale; }  ;
    f32 Scale() { return current_resolution.screen_scale; }  ;
    f32 ContentScale() { return current_resolution.content_scale; } ;
    f32 GraphicsScale() { return current_resolution.graphics_scale; } ;
  
    f32 phoneScale();
    
    padding getSafeArea();
    
protected:
    s32 findPredefinedResolution(f32 width, f32 height);
    s32 findNearestResolution(f32 width, f32 height);
    s32 findAspectRatio(f32 width, f32 height);

public:
    
    resolution_support current_resolution;
    bool isTablet;

};

#define RES(x) (f32)((x)*resolutionmanager::getInstance()->Scale())
// FJAR: what is DIS?
#define DIS(x) (f32)((x)*resolutionmanager::getInstance()->Scale())
#define NORES(x)    (x)
#define CONTENT_SCALE(x) (f32)    ((x)*resolutionmanager::getInstance()->ContentScale())
//#define CONTENT_SCALE(x) (f32)    (x)






#endif /* resolutionmanager_h */
