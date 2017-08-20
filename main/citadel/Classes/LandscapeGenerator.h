//
//  LandscapeGenerator.h
//  citadel
//
//  Created by Chris Wild on 09/08/2017.
//
//

#ifndef LandscapeGenerator_h
#define LandscapeGenerator_h

#include "cocos2d.h"
#include "tme_interface.h"

USING_NS_CC;

#define BASE_HEIGHT             768
#define BASE_WIDTH              1024

#define PEOPLE_WINDOW_HEIGHT    256
#define DIR_AMOUNT              512
#define DIR_STEPS               512
#define DIR_LEFT                (-DIR_AMOUNT/DIR_STEPS)
#define DIR_RIGHT               (DIR_AMOUNT/DIR_STEPS)
#define GSCALE                  4.0f

#define RES(x) (s32)	((x)*1.0)
//#define ASP(x) (s32)	((x)*uimanager::singleton()->aspect_scale)
//#define SRC(x) (s32)	((x)*uimanager::singleton()->source_scale)

typedef enum floor_t {
      floor_normal = 0
    , floor_snow
    , floor_river
    , floor_sea
    , floor_lake
    , floor_debug
} floor_t ;


class LandscapeItem : public Ref {
public:
    loc_t       loc;
    mxterrain_t	terrain;
    floor_t		floor;
    bool		army;
    bool		mist;
    Vec3		position;
    f32         scale;
    
};

typedef Vector<LandscapeItem*> LandscapeItems;

class LandscapeGenerator
{
public:
    LandscapeGenerator();
    
    void Build(loc_t location, f32 looking );
    void BuildPanorama();
    
    void ProcessQuadrant(s32 x, s32 y, s32 dx, s32 dy, s32 qDim);
    LandscapeItem* ProcessLocation(s32 x, s32 y);
    LandscapeItem* CalcCylindricalProjection(LandscapeItem* item);
    float RadiansFromFixedPointAngle(s32 fixed);
    
public:
    LandscapeItems* items;
    
    s32     location_infront_y;
    
    loc_t	loc;
    f32     looking;
    
    f32     near;
    f32     far;
    
};



#endif /* LandscapeGenerator_h */
