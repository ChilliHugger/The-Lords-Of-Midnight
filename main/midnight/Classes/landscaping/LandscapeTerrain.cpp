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

const std::string terrain_graphics[] = {
    
    // lom
    ""                  //    TN_PLAINS
    , "t_citadel0"      //    TN_CITADEL
    , "t_forest0"       //    TN_FOREST
    , "t_henge0"        //    TN_HENGE
    , "t_tower0"        //    TN_TOWER
    , "t_village0"      //    TN_VILLAGE
    , "t_downs0"        //    TN_DOWNS
    , "t_keep0"         //    TN_KEEP
    , "t_snowhall0"     //    TN_SNOWHALL
    , "t_lake0"         //    TN_LAKE
    , "t_frozenwaste0"  //    TN_FROZENWASTE
    , "t_ruin0"         //    TN_RUIN
    , "t_lith0"         //    TN_LITH
    , "t_cavern0"       //    TN_CAVERN
    , "t_mountain0"     //    TN_MOUNTAIN
    , ""                //    TN_UNUSED_15
    
    // ddr
    , ""                //    TN_PLAINS2
    , ""                //    TN_MOUNTAIN2
    , ""                //    TN_FOREST2
    , ""                //    TN_HILLS
    , ""                //    TN_GATE
    , ""                //    TN_TEMPLE
    , ""                //    TN_PIT
    , ""                //    TN_PALACE
    , ""                //    TN_FORTRESS
    , ""                //    TN_HALL
    , ""                //    TN_HUT
    , ""                //    TN_WATCHTOWER
    , ""                //    TN_CITY
    , ""                //    TN_FOUNTAIN
    , ""                //    TN_STONES
    , ""                //    TN_ICYWASTE
    , ""                //    TN_UNUSED_32
    
    // citadel
    , ""                //    TN_LAND
    , ""                //    TN_ISLE
    , ""                //    TN_LAKELAND
    , ""                //    TN_PLAIN
    , ""                //    TN_PLAINS3
    , "t_forest0"                //    TN_FOREST3
    , ""                //    TN_UNUSED_39
    , "t_trees0"                //    TN_TREES
    , "t_mountain0"                //    TN_MOUNTAIN3
    , "t_mountain0"                //    TN_ICY_MOUNTAIN
    , "t_downs0"                //    TN_DOWNS3
    , "t_downs0"                //    TN_HILLS3
    , "t_downs0"                //    TN_FOOTHILLS
    , ""                //    TN_VALLEY
    , ""                //    TN_BAY
    , ""                //    TN_SEA
    , ""                //    TN_RIVER
    , ""                //    TN_MARSH
    , ""                //    TN_LAKE3
    , ""                //    TN_UNUSED_52
    , ""                //    TN_UNUSED_53
    , "t_mist0"                //    TN_UNUSED_54
    , "t_mist0"                //    TN_MIST
    , ""                //    TN_UNUSED_56
    
};




void LandscapeTerrain::Init( LandscapeOptions* options )
{
    LandscapeNode::Init(options);

    setContentSize( Size(RES(1024),RES(768)) );
    
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
                    
                    if ( item->terrain == TN_MOUNTAIN || item->terrain == TN_FROZENWASTE)
                        alpha=1.0;
                    
                }
                
                
                graphic->setPosition(options->generator->NormaliseXPosition(item->position.x), this->getContentSize().height - y);
                graphic->setScale( graphic->getScale() * item->scale );
                
                graphic->getGLProgramState()->setUniformFloat("p_alpha", alpha);                    // alpha
                graphic->getGLProgramState()->setUniformVec4("p_left", Vec4(tint1.r,tint1.g,tint1.b,tint1.a));      // outline
                graphic->getGLProgramState()->setUniformVec4("p_right", Vec4(tint2.r,tint2.g,tint2.b,tint2.a));               // body

                //graphic->setColor(Color3B(tint0));
                
                this->addChild(graphic);
            }
        }
        
    }
    
}


Sprite* LandscapeTerrain::GetTerrainImage( mxterrain_t terrain )
{
    if ( terrain_graphics[terrain] == "" )
        return nullptr;
    
    auto image = Sprite::createWithSpriteFrameName( terrain_graphics[terrain] );
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

