//
//  LandscapeTerrain.cpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#include "LandscapeView.h"
#include "LandscapeGenerator.h"
#include "LandscapeNode.h"
#include "LandscapeSky.h"
#include "LandscapeLand.h"
#include "LandscapeTerrain.h"
#include "LandscapeDebug.h"
#include "LandscapeColour.h"

#include "../tme/tme_interface.h"
#include "../system/tmemanager.h"
#include "../system/moonring.h"
#include "../system/shadermanager.h"


LandscapeTerrain* LandscapeTerrain::create( LandscapeOptions* options )
{
    LandscapeTerrain* node = new (std::nothrow) LandscapeTerrain();
    if (node && node->initWithOptions(options))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool LandscapeTerrain::initWithOptions( LandscapeOptions* options )
{
    if ( !LandscapeNode::initWithOptions(options) )
        return false;

    return true;
}


void LandscapeTerrain::Build()
{
    auto items  = options->generator->items;
     
    auto tint1 = Color4F(options->colour->CalcCurrentMovementTint(TINT::TerrainOutline));
    auto tint2 = Color4F(options->colour->CalcCurrentMovementTint(TINT::TerrainFill));
    
    for(auto const& item: *items) {
        
        if ((item->position.z>=options->generator->viewportNear)&&(item->position.z<options->generator->viewportFar))
        {
            f32    alpha = 1.0f;
            // fade anything too close - ie as we move through the locations
            if (item->position.z<0.75f ) {
                alpha = (item->position.z - options->generator->viewportNear )*2.0f;
            }
            
            f32 y = item->position.y + options->generator->horizonOffset ;
            f32 x = item->position.x;
            f32 scale = item->scale;
            
            if ( item->position.z >= 8.5f  && y < 0 ) {
                
                f32 range = options->generator->viewportFar - 8.5f ;
                f32 diff = options->generator->viewportFar - item->position.z ;
                alpha = -1.0 + (2.0*(diff/range));
                
                y = 0 ;
                
                if ( item->terrain == TN_MOUNTAIN
                    || item->terrain == TN_FROZENWASTE
                    || item->terrain == TN_MOUNTAIN2
                    || item->terrain == TN_ICYWASTE
                    )
                    alpha=1.0;
                
            }
            
            
            // terrain
#if defined(_LOM_)
            if(item->graffiti) {
                std::string graffiti = "t_lith1";
                AddGraphic(GetImage(graffiti),x,y,tint1,tint2,alpha,scale,item);
            }else
#endif
            {
                AddGraphic(GetTerrainImage(item->terrain),x,y,tint1,tint2,alpha,scale,item);
            }
    
            if ( item->mist ) {
                
                std::string mist = "t_mist";
                AddDoubleGraphic(mist,x,y,tint1,tint2,alpha,scale,item);
            }
            else if ( item->army ) {
#if defined(_DDR_)
                std::string army = "t_army2";
                AddDoubleGraphic(army,x,y,tint1,tint2,alpha,scale,item);
#else
                std::string army = "t_army0";
                AddGraphic(GetImage(army),x,y,tint1,tint2,alpha,scale,item);
#endif
            }
        }
        
    }
    
}

void LandscapeTerrain::AddDoubleGraphic(
    std::string& name,
    f32 x, f32 y,
    Color4F tint1, Color4F tint2, f32 alpha, f32 scale,
    LandscapeItem* item)
{
    AddGraphic(GetImage(name),x,y,tint1,tint2,alpha,scale,item, -LRES(76)*scale);
    AddGraphic(GetImage(name),x,y,tint1,tint2,alpha,scale,item, +LRES(76+76)*scale);
}

Sprite* LandscapeTerrain::AddGraphic(
    Sprite* graphic, f32 x, f32 y,
    Color4F tint1, Color4F tint2, f32 alpha, f32 scale,
    LandscapeItem* item,
    f32 horizontalOffset)
{
    if ( graphic == nullptr )
        return nullptr;
    
    x+= horizontalOffset;
    
    graphic->setScale( graphic->getScale() * scale);
    graphic->setPosition(options->generator->NormaliseXPosition(x), getContentSize().height - y);
    graphic->setAnchorPoint(Vec2(0.5,0)); // bottom center

    mr->shader->UpdateTerrainTimeShader(graphic,alpha,tint1,tint2);
    
    auto imageItem = new ImageItem(item,horizontalOffset);
    imageItem->autorelease();
    
    graphic->setUserObject(imageItem);
    addChild(graphic);
    
    return graphic;
}

Sprite* LandscapeTerrain::GetTerrainImage( mxterrain_t terrain )
{
    // TODO: Cache this!
    terrain_data_t*    d = static_cast<terrain_data_t*>(TME_GetEntityUserData(MAKE_ID(INFO_TERRAININFO, terrain)));
    if ( d == nullptr || d->file.empty() ) {
        return nullptr;
    }
    
    auto image = GetImage( d->file );
    if ( image == nullptr )
        return image;
    
    if ( terrain == TN_LAKE ) {
      //  image->setAnchorPoint(Vec2(0.5,1.0));
    }
    
    return image;
}

Sprite* LandscapeTerrain::GetImage( std::string& imagename )
{
    if ( imagename.empty() )
        return nullptr;
 
    f32 imageScale = 1.0f;

    auto image = Sprite::createWithSpriteFrameName( imagename );
    if ( image != nullptr ) {
        
        image->setScale(imageScale);
        
        if ( options->terrainTimeShader ) {
            mr->shader->AttachShader(image,options->terrainTimeShader);
        }
    }
    return image;
}

void LandscapeTerrain::RefreshPositions()
{
    for ( auto node : getChildren() ) {
        auto imageItem = static_cast<ImageItem*>(node->getUserObject());
        if (imageItem->landscapeItem!=nullptr) {
            f32 x = imageItem->landscapeItem->position.x+imageItem->horizontalOffset;
            node->setPositionX(options->generator->NormaliseXPosition(x));
        }
        node->setTag(0);
    }
}
