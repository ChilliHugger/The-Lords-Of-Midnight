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

USING_NS_CC;

class LandscapeTerrain : public LandscapeNode
{
public:
    
    static LandscapeTerrain* create( LandscapeOptions* options );

    void Build() override;

protected:
    bool initWithOptions( LandscapeOptions* options );

    Sprite* AddGraphic(Sprite* graphic,f32 x, f32 y, Color4F tint1, Color4F tint2, f32 alpha, f32 scale);
    Sprite* GetTerrainImage( mxterrain_t terrain );
    Sprite* GetImage( std::string& imagename );
};



#endif /* LandscapeTerrain_hpp */
