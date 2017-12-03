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
#include "mxtypes.h"

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
} resolution_support ;




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

    float Width() { return current_resolution.width; } const ;
    float Height() { return current_resolution.height; } const ;
    u32 Aspect() { return current_resolution.aspect; } const ;
    float Scale() { return current_resolution.screen_scale; } const ;

    
public:
    
    resolution_support current_resolution;
    bool isTablet;

};

#define RES(x) (s32)    ((x)*resolutionmanager::getInstance()->Scale())
#define ALPHA(x) (s32)    ((x)*255)

#endif /* resolutionmanager_h */
