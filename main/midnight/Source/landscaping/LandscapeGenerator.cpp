
#include "LandscapeGenerator.h"
#include "ILandscape.h"
#include "../system/moonring.h"

USING_NS_AX;
USING_NS_TME;

mxterrain_t toGeneralisedTerrain(mxterrain_t t);


LandscapeGenerator::LandscapeGenerator() :
    options(nullptr),
    items(new Vector<LandscapeItem*>()),
    mr(nullptr)
{
}

LandscapeGenerator::~LandscapeGenerator()
{
    if(items!=nullptr)
    {
        UIDEBUG("LandscapeGenerator: Clear Items");
        items->clear();
        SAFEDELETE(items);
    }
}


void LandscapeGenerator::Build(LandscapeOptions* options)
{
    this->options = options;
    this->mr = options->mr;
    
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
    UIDEBUG("LandscapeGenerator: BuildPanorama");

    s32	qDim;
    
    s32 x = loc.x/LANDSCAPE_DIR_STEPS;
    s32 y = loc.y/LANDSCAPE_DIR_STEPS;
    
    qDim = 8;
    
    s32 id = 1;
    
    for ( int y1=y-qDim; y1<=y+qDim; y1++ ) {
        for ( int x1=x-qDim; x1<=x+qDim; x1++ ) {

            auto cell = ProcessLocation(x1, y1, id);
            if ( cell!= nullptr ) {
                items->pushBack(cell);
            }
            id++;
        }
    }
 
    sort( items->begin( ), items->end( ), [ ]( const LandscapeItem* lhs, const LandscapeItem* rhs )
    {
        return lhs->position.z > rhs->position.z;
    });
    
}

LandscapeItem* LandscapeGenerator::ProcessLocation(s32 x, s32 y, s32 id)
{
    maplocation     map;
    terraininfo     tinfo;

    LandscapeItem* item = new LandscapeItem();
    item->autorelease();
    
    auto locId = MAKE_LOCID(x, y);
    
    item->id = id;
    item->loc = loc_t(x,y);
    item->floor = floor_none;
    item->army = false;
    item->mist = false;
    item->position = Vec3(0,0,0);
    item->current = (item->loc == options->currentLocation);
        
    
    TME_GetLocation( map, locId );
    TME_GetTerrainInfo ( tinfo, MAKE_ID(INFO_TERRAININFO, map.terrain) );

    switch ( map.terrain ) {
        case TN_BAY:    item->floor = floor_bay;    break;
        case TN_SEA:    item->floor = floor_sea;    break;
        case TN_RIVER:  item->floor = floor_river;  break;
        case TN_MARSH:  item->floor = floor_marsh;  break;
        case TN_LAKE3:  item->floor = floor_lake;   break;
        default:        item->floor = floor_none;   break;
    }


    item->terrain = map.terrain;
    
#if defined(_LOM_)
    item->graffiti = ( x == 4 && y == 10 );  // Lith in the domain of moon
    
    // Impassable mountains
    if (map.flags&lf_impassable && map.terrain == TN_MOUNTAIN) {
        item->terrain = TN_MOUNTAIN2;
    }
#endif

    if( map.flags&lf_army && tinfo.flags&tif_army )
        item->army = true;

    if ( !options->isMoving && item->army && item->current)
        item->army = false;
    
    // check the current lords army temporarily
    // popping up as we move in or out of a location
    if ( options->isMoving && item->army )
        if (   (x== options->moveFrom.x && y==options->moveFrom.y)
            || (x== options->moveTo.x && y==options->moveTo.y && !options->moveLocationHasArmy )  )
            item->army=false;
    
    // if there is mist here then we need to draw the mist
    if ( map.flags&lf_mist && !tme::variables::sv_display_no_mist)
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
    
    // Calculate quad corners in raw projection space
    // before any NormaliseXPosition is applied
    item->quadValid = false;

    // Note: no near-distance cutoff here (unlike the billboard terrain
    // sprites, which use viewportNear to avoid oversized close-up
    // billboards) — the flat ground quad math is safe arbitrarily close to
    // the camera (projectCorner clamps both dist and screenY), and cutting
    // it off left a permanent hole in the floor patchwork right under the
    // player's own standing location.
    if (item->floor != floor_none && item->position.z < viewportFar)
    {
        const f32 p = 0.5f;
        f32 gx = item->loc.x;
        f32 gy = item->loc.y;

        // Project all 4 corners
        auto projectCorner = [&](f32 cx, f32 cy) -> ax::Vec2
        {
            f32 dx = cx - (loc.x / LANDSCAPE_DIR_STEPS);
            f32 dy = cy - (loc.y / LANDSCAPE_DIR_STEPS);

            f32 dist = sqrtf(dx*dx + dy*dy);
            if (dist < 0.0001f) dist = 0.0001f;

            f32 cObjAngle  = atan2f(dx, -dy);
            f32 cAngle     = cObjAngle - viewAngle;

            if (cAngle >  MX_PI) cAngle -= MX_PI2;
            if (cAngle < -MX_PI) cAngle += MX_PI2;

            f32 screenX = cAngle * PanoramaWidth / MX_PI2 + HorizonCentreX;
            f32 screenY = (PanoramaHeight / dist) + HorizonCentreY - horizonAdjust;
            screenY     = std::min(screenY, horizonOffset);

            return ax::Vec2(screenX, screenY);
        };

        item->quadCorners[0] = projectCorner(gx - p, gy - p);
        item->quadCorners[1] = projectCorner(gx + p, gy - p);
        item->quadCorners[2] = projectCorner(gx + p, gy + p);
        item->quadCorners[3] = projectCorner(gx - p, gy + p);

        // Wraparound: bring all corners within half a panorama of corner[0]
        f32 ref = item->quadCorners[0].x;
        for (int ii = 1; ii < 4; ii++) {
            f32& x = item->quadCorners[ii].x;
            while (x - ref >  PanoramaWidth * 0.5f) x -= PanoramaWidth;
            while (x - ref < -PanoramaWidth * 0.5f) x += PanoramaWidth;
        }

        item->quadValid = true;
    }
    
    
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

mxterrain_t toGeneralisedTerrain(mxterrain_t t)
{
    switch (t) {
        case TN_PLAINS2:
        case TN_PLAINS3:
        case TN_LAND:
        case TN_PLAIN:
            return TN_PLAINS;
            
        case TN_FOREST2:
        case TN_FOREST3:
        case TN_TREES:
            return TN_FOREST;
            
        case TN_MOUNTAIN2:
        case TN_MOUNTAIN3:
        case TN_ICY_MOUNTAIN:
            return TN_MOUNTAIN;
            
        case TN_WATCHTOWER:
            return TN_TOWER;
            
        case TN_ICYWASTE:
            return TN_FROZENWASTE;
            
        case TN_LAKE3:
            return TN_LAKE;
            
        case TN_HILLS3:
        case TN_DOWNS:
        case TN_FOOTHILLS:
            return TN_HILLS;
            
        case TN_STONES:
            return TN_LITH;
            
        default:
            return t;
    }
}
