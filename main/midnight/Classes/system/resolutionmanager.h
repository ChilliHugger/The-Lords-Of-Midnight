//
//  resolutionmanager.h
//  TestHarness
//
//  Created by Chris Wild on 28/06/2017.
//
//

#ifndef resolutionmanager_h
#define resolutionmanager_h

#include "cocos2d.h"
#include "../library/libinc/mxtypes.h"

USING_NS_CC;

using namespace chilli::types;

typedef struct {
    s32     reference;
    c_str   folder;
    f32     width;
    f32     height;
    u32     aspect;
    u32     fontset;
    f32     screen_scale;
    f32     target_scale;
    f32     aspect_scale;
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
    f32 ContentScale() { return current_resolution.target_scale; } ;
    f32 phoneScale();
    
    padding getSafeArea();
    
public:
    
    resolution_support current_resolution;
    bool isTablet;

};

#define RES(x) (f32)    ((x))
#define DIS(x) (f32)    ((x)*resolutionmanager::getInstance()->AspectScale())

//#define RES(x) (f32)    ((x)*resolutionmanager::getInstance()->Scale())
//#define ASP(x) (f32)    ((x)*resolutionmanager::getInstance()->AspectScale())




#endif /* resolutionmanager_h */
