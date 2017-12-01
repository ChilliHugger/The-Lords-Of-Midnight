
#include "LandscapeGenerator.h"


using namespace tme;

LandscapeGenerator::LandscapeGenerator()
{
	items = new Vector<LandscapeItem*>();
}

void LandscapeGenerator::Build(loc_t location, f32 direction)
{
    loc = location;
    looking = direction;
    
    items->clear();
	
    BuildPanorama();
    
//    int count=0;
//    for(auto const& item: *items) {
//        CCLOG("%d,%d,%d,%d", count,item->loc.x, item->loc.y, item->terrain);
//        count++;
//    }
    
}
	
	
void LandscapeGenerator::BuildPanorama()
{
    s32	qDim;
    
    s32 x = loc.x/DIR_STEPS;
    s32 y = loc.y/DIR_STEPS;
    
    qDim = 8;
	
    //x = x - qDim;
    //y = y - qDim;
    
    
    for ( int y1=y-qDim; y1<=y+qDim; y1++ ) {
        for ( int x1=x-qDim; x1<=x+qDim; x1++ ) {
            
//            if ( x1<0 || y1<0 )
//                continue;
            
            auto cell = ProcessLocation(x1, y1);
            if ( cell!= nullptr )
                items->pushBack(cell);
        }
    }
 
    sort( items->begin( ), items->end( ), [ ]( const LandscapeItem* lhs, const LandscapeItem* rhs )
    {
        return lhs->position.z > rhs->position.z;
    });
    
    
//    ProcessQuadrant(x, y, 0, 0, qDim);
//    ProcessQuadrant(x + qDim, y, 1, 0, qDim);
//    ProcessQuadrant(x, y + qDim, 0, 1, qDim);
//    ProcessQuadrant(x + qDim, y + qDim, 1, 1, qDim);
    
}


void LandscapeGenerator::ProcessQuadrant(s32 x, s32 y, s32 dx, s32 dy, s32 qDim)
{
    s32	qx1, qy1, qx2, qy2;
    
    qDim = qDim>>1;
    
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
    item->floor = floor_normal;
    
    
    if ( map.terrain == TN_LAKE3 )
        item->floor = floor_lake;
    
    if ( map.terrain == TN_RIVER )
        item->floor = floor_river ;
    
    if ( map.terrain == TN_SEA || map.terrain == TN_BAY  )
        item->floor = floor_sea ;
        
    if (map.terrain == TN_ICYWASTE || map.terrain == TN_FROZENWASTE)
        item->floor = floor_snow ;
	
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
    
    // We are running a panorama that runs from N to NW along a linear
    // so place all locations to the right
    if (item->position.x<=-225)
        item->position.x += 3200;
    
    return item;
}

float LandscapeGenerator::RadiansFromFixedPointAngle(s32 fixed)
{
    float angle = (float)fixed;
    angle = angle*MX_PI2/4096.0f;
    return angle;
}


f32 LandscapeGenerator::NormaliseXPosition(f32 x)
{
    x = x-horizontalOffset ;
    if ( horizontalOffset<1024 && x>1600 )
        x -= 3200;
    if ( horizontalOffset>=1024 && x<-512 )
        x += 3200;
    return x;
}
