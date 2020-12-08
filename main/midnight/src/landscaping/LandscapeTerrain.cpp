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
            AddGraphic(GetTerrainImage(item->terrain),x,y,tint1,tint2,alpha, item->scale);
   
            if ( item->mist ) {
                
                std::string mist = "t_mist";
                AddGraphic(GetImage(mist),x-LRES(76)*item->scale,y,tint1,tint2,1.0f, item->scale);
                AddGraphic(GetImage(mist),x+LRES(76+76)*item->scale,y,tint1,tint2,1.0f, item->scale);
            }
            else if ( item->army ) {
#if defined(_DDR_)
                std::string army = "t_army2";
                AddGraphic(GetImage(army),x-LRES(76)*item->scale,y,tint1,tint2,1.0f, item->scale);
                AddGraphic(GetImage(army),x+LRES(76+76)*item->scale,y,tint1,tint2,1.0f, item->scale);
#else
                std::string army = "t_army0";
                AddGraphic(GetImage(army),x,y,tint1,tint2,alpha, item->scale);
#endif
            }
        }
        
    }
    
}

Sprite* LandscapeTerrain::AddGraphic(Sprite* graphic, f32 x, f32 y, Color4F tint1, Color4F tint2, f32 alpha, f32 scale)
{
    if ( graphic == nullptr )
        return nullptr;
    
    graphic->setScale( graphic->getScale() * scale);
    graphic->setPosition(options->generator->NormaliseXPosition(x), getContentSize().height - y);
    graphic->setAnchorPoint(Vec2(0.5,0)); // bottom center

    moonring::mikesingleton()->shader->UpdateSpriteDayNightShader(graphic,alpha,tint1,tint2);
    
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
 
    //auto visibleSize = Director::getInstance()->getVisibleSize();
    f32 imageScale = 1.0f;

    
    
    auto image = Sprite::createWithSpriteFrameName( imagename );
    if ( image != nullptr ) {
        
        image->setScale(imageScale);
        
        if ( options->shader ) {
             moonring::mikesingleton()->shader->AttachShader(image,options->shader);
        }
    }
    return image;
}
