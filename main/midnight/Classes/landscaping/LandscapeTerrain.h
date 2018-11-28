//
//  LandscapeTerrain.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeTerrain_hpp
#define LandscapeTerrain_hpp

#include <stdio.h>

#include "LandscapeNode.h"
#include "LandscapeGenerator.h"


class LandscapeTerrain : public LandscapeNode
{
public:
    
    const float	horizonOffset = RES( (112*LANDSCAPE_GSCALE) );
    
    static LandscapeTerrain* create( LandscapeOptions* options );

protected:
    bool initWithOptions( LandscapeOptions* options );

    void BuildTerrain( LandscapeItems* items );
    Sprite* AddGraphic(Sprite* graphic,f32 x, f32 y, Color4F tint1, Color4F tint2, f32 alpha, f32 scale);
    Sprite* GetTerrainImage( mxterrain_t terrain );
    Sprite* GetImage( std::string& imagename );
};



#endif /* LandscapeTerrain_hpp */
