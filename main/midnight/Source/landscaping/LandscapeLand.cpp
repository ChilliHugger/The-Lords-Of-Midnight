//
//  LandscapeLand.cpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//

#include "LandscapeView.h"
#include "LandscapeGenerator.h"
#include "LandscapeNode.h"
#include "LandscapeSky.h"
#include "LandscapeLand.h"
#include "LandscapeTerrain.h"
#include "LandscapeDebug.h"
#include "LandscapeColour.h"
#include "../system/moonring.h"
#include "../system/shadermanager.h"
#include "../ui/uihelper.h"

USING_NS_AX;

// -----------------------------------------------------------------------------
// Tuning constants
// -----------------------------------------------------------------------------
constexpr std::string FLOOR_TILE_NAME = "floortiles";

// One flat colour per water terrain, indexed by floor_t (floor_none unused).
static const Color4F waterColour[] = {
    Color4F(0.0f, 0.0f, 0.0f, 0.0f),   // floor_none
    Color4F(0.10f, 0.35f, 0.75f, 1.0f), // floor_bay
    Color4F(0.00f, 0.15f, 0.65f, 1.0f), // floor_sea
    Color4F(0.15f, 0.50f, 1.00f, 1.0f), // floor_river
    Color4F(0.25f, 0.50f, 0.60f, 1.0f), // floor_marsh
    Color4F(0.05f, 0.25f, 0.85f, 1.0f), // floor_lake
};

// -----------------------------------------------------------------------------

LandscapeLand* LandscapeLand::create( LandscapeOptions* options )
{
    LandscapeLand* node = new (std::nothrow) LandscapeLand();
    if (node && node->initWithOptions(options))
    {
        node->autorelease();
        return node;
    }
    AX_SAFE_DELETE(node);
    return nullptr;
}

bool LandscapeLand::initWithOptions( LandscapeOptions* options )
{
    if ( !LandscapeNode::initWithOptions(options) )
        return false;

    auto floor = Sprite::createWithSpriteFrameName( "floor" );
    addChild(floor);
    floor->setPosition(Vec2::ZERO);
    floor->setAnchorPoint(Vec2::ZERO);
    floor->setName("floor");
    if ( options->terrainTimeShader )
        options->colour->updateTerrainNode(floor);

    if ( options->debugLand )
        floor->setColor(Color3B::YELLOW);

    // floorShader = mr->shader->GetTerrainTimeShader();

    auto container = Node::create();
    container->setName(FLOOR_TILE_NAME);
    container->setLocalZOrder(10);
    addChild(container);

    return true;
}

// -----------------------------------------------------------------------------
// Build
// -----------------------------------------------------------------------------

void LandscapeLand::Build()
{
    // Scale the base floor sprite to fill the full width of the node.
    auto floor = getChildByName("floor");
    float scalex = getContentSize().width / floor->getContentSize().width;
    floor->setScale(scalex, 1.0f);

    if ( options->showWater ) {
        DrawWater();
    }
}

// -----------------------------------------------------------------------------
// CalcQuadCorners
// Takes the 4 projected ground-plane corners of a location, converts them to
// node space, and shifts them for panorama wrap. Returns false if the quad
// should be culled.
// -----------------------------------------------------------------------------
bool LandscapeLand::CalcQuadCorners( LandscapeItem* item, Vec2* corners )
{
    if (!item->quadValid)
        return false;

    f32 h        = getContentSize().height;
    f32 sw       = getContentSize().width;
    f32 panorama = options->generator->PanoramaWidth;
    f32 offset   = LRES(options->generator->horizontalOffset);

    for (int ii = 0; ii < 4; ii++) {
        corners[ii].x = item->quadCorners[ii].x - offset;
        corners[ii].y = h - item->quadCorners[ii].y;
    }

    // Shift all corners together if the group is off-screen
    f32 minX = corners[0].x, maxX = corners[0].x;
    for (int ii = 1; ii < 4; ii++) {
        minX = std::min(minX, corners[ii].x);
        maxX = std::max(maxX, corners[ii].x);
    }

    if (maxX < 0) {
        for (int ii = 0; ii < 4; ii++) corners[ii].x += panorama;
    } else if (minX > sw) {
        for (int ii = 0; ii < 4; ii++) corners[ii].x -= panorama;
    }

    return QuadIsValid(corners);
}

// Only rejects torn wraparound quads. Renderer clips everything else.
bool LandscapeLand::QuadIsValid( Vec2* corners )
{
    f32 minX = corners[0].x, maxX = corners[0].x;
    for (int ii = 1; ii < 4; ii++) {
        minX = std::min(minX, corners[ii].x);
        maxX = std::max(maxX, corners[ii].x);
    }
    return (maxX - minX) < (options->generator->PanoramaWidth * 0.4f);
}

// -----------------------------------------------------------------------------
// DrawWater
// Every water location (bay, sea, river, marsh, lake) is drawn as its own
// projected ground quad, pinned to the landscape, filled with that terrain's
// flat colour. Items are already sorted far to near.
// -----------------------------------------------------------------------------
void LandscapeLand::DrawWater()
{
    auto container = getChildByName(FLOOR_TILE_NAME);
    auto cameraLoc = options->generator->loc;

    for (auto const& item : *options->generator->items) {
        if ( item->floor == floor_none )
            continue;

        Vec2 corners[4];

        // The cell the camera is actually inside (item->current is fixed at
        // the starting location, so it can't be used while moving) surrounds
        // the camera: its projected corners collapse onto the near clamp
        // line and straddle the panorama wrap (so CalcQuadCorners would
        // reject it). Fill the whole width instead, from the bottom of the
        // view up to the far edge of the cell.
        f32 half = LANDSCAPE_DIR_STEPS * 0.5f;
        bool cameraInside =
            fabsf( item->loc.x * LANDSCAPE_DIR_STEPS - cameraLoc.x ) <= half &&
            fabsf( item->loc.y * LANDSCAPE_DIR_STEPS - cameraLoc.y ) <= half;

        if ( cameraInside ) {
            f32 h  = getContentSize().height;
            f32 sw = getContentSize().width;
            f32 top = h - item->quadCorners[0].y;
            for (int ii = 1; ii < 4; ii++)
                top = std::max(top, h - item->quadCorners[ii].y);
            corners[0] = Vec2(0,  top);
            corners[1] = Vec2(sw, top);
            corners[2] = Vec2(sw, 0);
            corners[3] = Vec2(0,  0);
        } else if ( !CalcQuadCorners(item, corners) ) {
            continue;
        }

        auto tile = FloorTile::create(waterColour[(int)item->floor]);
        if ( tile != nullptr ) {
            tile->setCorners(corners);
            container->addChild(tile);
        }
    }
}

// -----------------------------------------------------------------------------
// RefreshPositions
// -----------------------------------------------------------------------------

void LandscapeLand::RefreshPositions()
{
    if ( options->showLand || options->showWater ) {
        for ( auto node : getChildren() ) {
            if ( node->getName() == "floor" )
                continue;
            auto imageItem = static_cast<ImageItem*>(node->getUserObject());
            if ( imageItem != nullptr && imageItem->landscapeItem != nullptr ) {
                f32 x = imageItem->landscapeItem->position.x + imageItem->horizontalOffset;
                node->setPositionX(options->generator->NormaliseXPosition(x));
            }
        }
    }
    
    // rebuild water quads for panning
    if ( options->showWater ) {
        auto container = getChildByName(FLOOR_TILE_NAME);
        container->removeAllChildrenWithCleanup(true);
        DrawWater();
    }
}

