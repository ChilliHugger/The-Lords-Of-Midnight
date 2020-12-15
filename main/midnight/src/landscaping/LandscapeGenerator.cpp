
#include "LandscapeGenerator.h"
#include "ILandscape.h"

USING_NS_CC;

using namespace tme;

LandscapeGenerator::LandscapeGenerator()
{
	items = new Vector<LandscapeItem*>();
}

void LandscapeGenerator::Build(LandscapeOptions* options)
{
    this->options = options;
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    f32 aspect = 1024.0 / 768.0;
    f32 newWidth = visibleSize.height * aspect;
    options->resScale = newWidth / 1024.0;

    HorizonCentreX = LRES( (256*LANDSCAPE_GSCALE)/2  ) - options->lookOffsetAdjustment;
    HorizonCentreY = 0 ; //LRES( -112 );
    PanoramaWidth =  (float)LRES((800.0f*LANDSCAPE_GSCALE));
    PanoramaHeight = (float)LRES(38.0f*LANDSCAPE_GSCALE); // 32
    LocationHeight = (float)LRES(48.0f*LANDSCAPE_GSCALE);
    horizonAdjust = LRES((5*LANDSCAPE_GSCALE));
    horizonOffset = LRES( (112*LANDSCAPE_GSCALE) );
    

    loc = options->here;
    looking = 0;
    
    items->clear();
	
    if ( options->isInTunnel )
        return;
    
    BuildPanorama();

}
	
	
void LandscapeGenerator::BuildPanorama()
{
    s32	qDim;
    
    s32 x = loc.x/LANDSCAPE_DIR_STEPS;
    s32 y = loc.y/LANDSCAPE_DIR_STEPS;
    
    qDim = 8;
    
    for ( int y1=y-qDim; y1<=y+qDim; y1++ ) {
        for ( int x1=x-qDim; x1<=x+qDim; x1++ ) {

            auto cell = ProcessLocation(x1, y1);
            if ( cell!= nullptr )
                items->pushBack(cell);
        }
    }
 
    sort( items->begin( ), items->end( ), [ ]( const LandscapeItem* lhs, const LandscapeItem* rhs )
    {
        return lhs->position.z > rhs->position.z;
    });
    
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
    item->autorelease();
    
	
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

    if ( !options->isMoving && item->army ) {
        if (x== options->currentLocation.x && y==options->currentLocation.y)
            item->army = false;
    }
    
    // check the current lords army temporarily
    // popping up as we move in or out of a location
    if ( options->isMoving && item->army )
        if (   (x== options->moveFrom.x && y==options->moveFrom.y)
            || (x== options->moveTo.x && y==options->moveTo.y && !options->moveLocationHasArmy )  )
            item->army=false;
    
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
	float	x, y, xOff, yOff;
	double angle, objAngle, viewAngle;
    
    x = (float)( (item->loc.x*LANDSCAPE_DIR_STEPS) - loc.x) / (float)LANDSCAPE_DIR_STEPS;
    y = (float)( (item->loc.y*LANDSCAPE_DIR_STEPS) - loc.y) / (float)LANDSCAPE_DIR_STEPS;
    
    f32 looking_amount = looking;
        
    viewAngle = RadiansFromFixedPointAngle( looking_amount );
	
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
    if (item->position.x<=LRES(-225))
        item->position.x += PanoramaWidth;
    
    return item;
}

float LandscapeGenerator::RadiansFromFixedPointAngle(s32 fixed)
{
    float angle = (float)fixed;
    angle = angle*MX_PI2/4096.0f;
    return angle;
}

//
// X coordinafe is in Scaled Panorama units (ie: real screen units)
//
f32 LandscapeGenerator::NormaliseXPosition(f32 x)
{
    x = x-LRES(horizontalOffset) ;
    
    // Boundary in panoramic units
    f32 boundary = LANDSCAPE_DIR_STEPS*3;
    f32 maxScreenX = landscapeScreenWidth+LRES(512);
    f32 minScreenX = LRES(-512);
    
    // to the left
    if ( horizontalOffset<boundary && x>maxScreenX )
    {
        x -= PanoramaWidth;
    }

    // to the right
    if ( horizontalOffset>=boundary && x<minScreenX )
    {
        x += PanoramaWidth;
    }

    return x;
}
