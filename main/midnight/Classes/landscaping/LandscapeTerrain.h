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
    
    const float	horizonOffset = RES( (112*GSCALE) );
    
    
    virtual void Init(LandscapeOptions* options);
    
protected:
    void BuildTerrain( LandscapeItems* items );
    Sprite* GetTerrainImage( mxterrain_t terrain );

public:
    GLProgramState* programState;
    
};



#endif /* LandscapeTerrain_hpp */
