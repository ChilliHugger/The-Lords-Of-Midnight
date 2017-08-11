
#include "LandscapeGenerator.h"




LandscapeGenerator::LandscapeGenerator()
{
    near=0.25f;
    far=6.5f;

	items = new Vector<LandscapeItem*>();
	
}

 
    
void LandscapeGenerator::Build(loc_t location, f32 direction)
{
    loc = location;
    loc.x *= DIR_STEPS;
    loc.y *= DIR_STEPS;
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
	item->floor = ( map.terrain == TN_LAKE ) ? floor_water : floor_normal;
	item->army = false;
	item->mist = false;

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


LandscapeItem* LandscapeGenerator::CalcCylindricalProjection(LandscapeItem* item)
{
    float	x, y, xOff, yOff, angle, objAngle, viewAngle, width, height;
    
    float	HorizonCentreX = RES(512.0f);
    float	HorizonCentreY = RES(430.0f);
    float	PanoramaWidth =  (float)RES((800.0f*GSCALE));
    float	PanoramaHeight = (float)RES(38.0f*GSCALE);
    int     adjust=0;
    
//    if ( t_global::singleton()->calc_landscape ) {
//        HorizonCentreX = this->getContentSize().width*0.50f;
//        HorizonCentreY = this->getContentSize().height*0.5833f;
//        
//        int h = this->getContentSize().height - 320 /*IMAGE(floor)->Height() * temp_aspect*/;
//        
//        near=0.25f;
//        far=6.5f;
//        f32 y_scale = (38*GSCALE) / 320.0f ;
//        
//        PanoramaWidth =  (float)RES((800.0f*GSCALE));
//        PanoramaHeight = (float)h*y_scale;
//        adjust = PanoramaHeight*(1.0f/far) ;
//    }
    
    
    location_infront_y = PanoramaHeight + HorizonCentreY - adjust;
    
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
    item->position.y = yOff + HorizonCentreY - adjust;
    
    return item;
}

float LandscapeGenerator::RadiansFromFixedPointAngle(s32 fixed)
{
    float angle = (float)fixed;
    angle = angle*MX_PI2/4096.0f;
    return angle;
}


