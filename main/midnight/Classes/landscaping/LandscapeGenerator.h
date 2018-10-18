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
#define DIR_AMOUNT              512
#define DIR_STEPS               512
#define DIR_LEFT                (-DIR_AMOUNT/DIR_STEPS)
#define DIR_RIGHT               (DIR_AMOUNT/DIR_STEPS)
#define GSCALE                  4.0f


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
    tme::loc_t       loc;
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
    
    const float	HorizonCentreX = RES( (256*GSCALE)/2  );
    const float	HorizonCentreY = 0; //RES( (112*GSCALE) );
    
    const float	PanoramaWidth =  (float)RES((800.0f*GSCALE));
    const float	PanoramaHeight = (float)RES(32.0f*GSCALE);
    const float	LocationHeight = (float)RES(48.0f*GSCALE);
    
    const float viewportNear=0.25f;
    const float viewportFar=21.5f;
    
    // adjustment for the furthest of our visible locations being on the horizon
    const int     horizonAdjust = RES((3*GSCALE));
    
public:
    LandscapeGenerator();
    
    void Build(tme::loc_t location, f32 looking );
    void BuildPanorama();
    
    void ProcessQuadrant(s32 x, s32 y, s32 dx, s32 dy, s32 qDim);
    LandscapeItem* ProcessLocation(s32 x, s32 y);
    LandscapeItem* CalcCylindricalProjection(LandscapeItem* item);
    float RadiansFromFixedPointAngle(s32 fixed);
    f32 NormaliseXPosition(f32 x);
    
public:
    LandscapeItems* items;
    
    s32     location_infront_y;
    
    tme::loc_t	loc;
    f32     looking;
    f32     horizontalOffset;
    
};



#endif /* LandscapeGenerator_h */
