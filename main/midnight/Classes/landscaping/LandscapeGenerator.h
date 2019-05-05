//
//  LandscapeGenerator.h
//  citadel
//
//  Created by Chris Wild on 09/08/2017.
//
//

#ifndef LandscapeGenerator_h
#define LandscapeGenerator_h

#include "LandscapeNode.h"

#include "../system/resolutionmanager.h"

#define BASE_HEIGHT             768
#define BASE_WIDTH              1024

#define PEOPLE_WINDOW_HEIGHT    256
#define LANDSCAPE_DIR_AMOUNT    400.0f
#define LANDSCAPE_DIR_STEPS     400.0f
#define LANDSCAPE_DIR           (LANDSCAPE_DIR_AMOUNT/LANDSCAPE_DIR_STEPS)
#define LANDSCAPE_DIR_LEFT      (-LANDSCAPE_DIR)
#define LANDSCAPE_DIR_RIGHT     LANDSCAPE_DIR
#define LANDSCAPE_GSCALE        4.0f
#define LANDSCAPE_FULL_WIDTH    (LANDSCAPE_DIR_AMOUNT*8)



typedef enum floor_t {
      floor_normal = 0
    , floor_snow
    , floor_river
    , floor_sea
    , floor_lake
    , floor_debug
} floor_t ;


class LandscapeItem : public cocos2d::Ref {
public:
    tme::loc_t       loc;
    mxterrain_t	terrain;
    floor_t		floor;
    bool		army;
    bool		mist;
    Vec3		position;
    f32         scale;
    
};

typedef Vector<LandscapeItem*> LandscapeItems;

FORWARD_REFERENCE(LandscapeOptions);

class LandscapeGenerator
{
public:
    
    const float	HorizonCentreX = RES( (256*LANDSCAPE_GSCALE)/2  );
    const float	HorizonCentreY = 0 ; //RES( -112 );
    
    const float	PanoramaWidth =  (float)RES((800.0f*LANDSCAPE_GSCALE));
    const float	PanoramaHeight = (float)RES(38.0f*LANDSCAPE_GSCALE); // 32
    const float	LocationHeight = (float)RES(48.0f*LANDSCAPE_GSCALE);
    
    const float viewportNear=0.25f;
    const float viewportFar=6.5f;
    
    // adjustment for the furthest of our visible locations being on the horizon
    const int     horizonAdjust = RES((5*LANDSCAPE_GSCALE));
    
public:
    LandscapeGenerator();
    
    void Build(LandscapeOptions* options);
    void BuildPanorama();
    
    void ProcessQuadrant(s32 x, s32 y, s32 dx, s32 dy, s32 qDim);
    LandscapeItem* ProcessLocation(s32 x, s32 y);
    LandscapeItem* CalcCylindricalProjection(LandscapeItem* item);
    float RadiansFromFixedPointAngle(s32 fixed);
    f32 NormaliseXPosition(f32 x);
    
public:
    LandscapeItems*     items;
    LandscapeOptions*   options;

    
    s32     location_infront_y;
    
    tme::loc_t	loc;
    f32     looking;
    f32     horizontalOffset;

    
};



#endif /* LandscapeGenerator_h */
