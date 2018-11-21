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

#include "../tme_interface.h"
#include "../system/tmemanager.h"

void LandscapeTerrain::Init( LandscapeOptions* options )
{
    LandscapeNode::Init(options);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    setContentSize( visibleSize );
    
    BuildTerrain( options->generator->items );
    
}


void LandscapeTerrain::BuildTerrain( LandscapeItems* items )
{
    auto tint1 = Color4F(options->colour->CalcCurrentMovementTint(1));
    auto tint2 = Color4F(options->colour->CalcCurrentMovementTint(2));
    
    
    
    for(auto const& item: *items) {
        
        if ((item->position.z>=options->generator->viewportNear)&&(item->position.z<options->generator->viewportFar))
        {
            f32	alpha = 1.0f;
            
            
            
            // fade anything too close - ie as we move through the locations
            if (item->position.z<0.75f ) {
                alpha = (item->position.z - options->generator->viewportNear )*2.0f;
            }
            
            auto graphic = GetTerrainImage(item->terrain);
            if ( graphic ) {
                
                f32 y = item->position.y + horizonOffset ;
                
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
                
                
                graphic->setScale( graphic->getScale() * item->scale );
                graphic->setPosition(options->generator->NormaliseXPosition(item->position.x),
                                     this->getContentSize().height - y);
                graphic->setAnchorPoint(Vec2(0.5,0));
                
                graphic->getGLProgramState()->setUniformFloat("p_alpha", alpha);                    // alpha
                graphic->getGLProgramState()->setUniformVec4("p_left", Vec4(tint1.r,tint1.g,tint1.b,tint1.a));      // outline
                graphic->getGLProgramState()->setUniformVec4("p_right", Vec4(tint2.r,tint2.g,tint2.b,tint2.a));               // body

  
                this->addChild(graphic);
            }
        }
        
    }
    
}


Sprite* LandscapeTerrain::GetTerrainImage( mxterrain_t terrain )
{
    // TODO: Cache this!
    terrain_data_t*    d = (terrain_data_t*)TME_GetEntityUserData(MAKE_ID(INFO_TERRAININFO, terrain));
    if ( d == nullptr || strlen(d->file) == 0 ) {
        return nullptr;
    }
    
    auto image = Sprite::createWithSpriteFrameName( d->file.GetAt() );
    if ( image == nullptr )
        return image;
    
    image->setScale(1.0f);
    image->setBlendFunc(cocos2d::BlendFunc::ALPHA_NON_PREMULTIPLIED);
    
    if ( programState )
        image->setGLProgramState( programState->clone() );
    
    if ( terrain == TN_LAKE ) {
      //  image->setAnchorPoint(Vec2(0.5,1.0));
    }
    
    return image;
}

