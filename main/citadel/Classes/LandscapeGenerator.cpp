
#include "LandscapeGenerator.h"




LandscapeGenerator::LandscapeGenerator()
{
    near=0.25f;
    far=11.5f;

	items = new Vector<LandscapeItem*>();
	
}

 
    
void LandscapeGenerator::Build(loc_t location, f32 direction)
{
    loc = location;
    //loc.x *= DIR_STEPS;
    //loc.y *= DIR_STEPS;
    looking = direction; //direction * DIR_AMOUNT ;

    
    items->clear();
	
    BuildPanorama();
}
	
	
void LandscapeGenerator::BuildPanorama()
{
    s32	qDim;
    
    s32 x = loc.x/DIR_STEPS;
    s32 y = loc.y/DIR_STEPS;
    
    qDim = 8;
	
    x = x - qDim;
    y = y - qDim;
    
    ProcessQuadrant(x, y, 0, 0, qDim);
    ProcessQuadrant(x + qDim, y, 1, 0, qDim);
    ProcessQuadrant(x, y + qDim, 0, 1, qDim);
    ProcessQuadrant(x + qDim, y + qDim, 1, 1, qDim);
    
}


void LandscapeGenerator::ProcessQuadrant(s32 x, s32 y, s32 dx, s32 dy, s32 qDim)
{
    s32	qx1, qy1, qx2, qy2;
    
    qDim = qDim>>1; // 2
    
    if (qDim)
    {
        qx1 = x + dx*qDim;
        qx2 = x + (1 - dx)*qDim;
        qy1 = y + dy*qDim;
        qy2 = y + (1 - dy)*qDim;
        
        ProcessQuadrant( qx1, qy1, dx, dy, qDim);
        ProcessQuadrant( qx2, qy1, dx, dy, qDim);
        ProcessQuadrant( qx1, qy2, dx, dy, qDim);
        ProcessQuadrant( qx2, qy2, dx, dy, qDim);
    }
    else
    {
        auto cell = ProcessLocation(x, y);
		if ( cell!= nullptr )
			items->pushBack(cell);
    }
}

LandscapeItem* LandscapeGenerator::ProcessLocation(s32 x, s32 y)
{
    maplocation		map;
	terraininfo		tinfo;

	LandscapeItem* item = new LandscapeItem();
	
	TME_GetLocation( map, MAKE_LOCID(x, y) );
	TME_GetTerrainInfo ( tinfo, MAKE_ID(INFO_TERRAININFO, map.terrain) );

	item->loc = loc_t(x,y);
	item->floor = ( map.terrain > TN_UNUSED1 ) ? floor_water : floor_normal;
	item->army = false;
	item->mist = false;
    item->position = Vec3(0,0,0);
    item->terrain = map.terrain;

	if( map.flags&lf_army && tinfo.flags&tif_army )
		item->army = true;

//    if ( moveCount && bDrawArmy )
//        if ( (x== moveFrom.x && y==moveFrom.y) || (x== moveTo.x && y==moveTo.y && !bMoveLocationHasArmy )  )
//            bDrawArmy=FALSE;
    
    // if there is mist here then we need to draw the mist
	if ( map.flags&lf_mist ) 
		item->mist = true;

    return CalcCylindricalProjection(item);
    
}


// spectrum screen was 256x192
// Sky was 112  height
// floor was 80 height
// location in front was at 48 pixels from the bottom
// thus the panorama height was 32
// we need a 3 pixel horizon adjustment to put the far locations on the horizon

LandscapeItem* LandscapeGenerator::CalcCylindricalProjection(LandscapeItem* item)
{
    float	x, y, xOff, yOff, angle, objAngle, viewAngle;
    

    const float	HorizonCentreX = RES( (256*GSCALE)/2  );
    const float	HorizonCentreY = RES( (112*GSCALE) );
    
    const float	PanoramaWidth =  (float)RES((800.0f*GSCALE));
    const float	PanoramaHeight = (float)RES(32.0f*GSCALE);
    
    // adjustment for the furthest of our visible locations being on the horizon
    int     horizonAdjust = RES((3*GSCALE));
    
    location_infront_y = PanoramaHeight + HorizonCentreY - horizonAdjust;
    
    x = (float)( (item->loc.x*DIR_STEPS) - loc.x) / (float)DIR_STEPS;
    y = (float)( (item->loc.y*DIR_STEPS) - loc.y) / (float)DIR_STEPS;
    
    viewAngle = RadiansFromFixedPointAngle( looking );
	
    objAngle = atan2f(x, -y);
	
    angle = objAngle - viewAngle;
	
    if (angle>MX_PI)
        angle -= MX_PI2;
	
    if (angle<-MX_PI)
        angle += MX_PI2;
    
    //	convert angle to horizon centre xOffset (cylindrical projection)
    xOff = angle*PanoramaWidth/(MX_PI2);
    
    //	now do the horizon centre yOffset perspective projection
    item->position.z = sqrtf(x*x + y*y);
    
    item->scale = 1.0f/item->position.z;
    
    yOff = PanoramaHeight*item->scale;
    
    item->position.x = xOff + HorizonCentreX;
    item->position.y = yOff + HorizonCentreY - horizonAdjust;
    
    return item;
}

float LandscapeGenerator::RadiansFromFixedPointAngle(s32 fixed)
{
    float angle = (float)fixed;
    angle = angle*MX_PI2/4096.0f;
    return angle;
}


