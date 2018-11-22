//
//  LandscapeLand.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeLand_hpp
#define LandscapeLand_hpp

#include "LandscapeNode.h"
#include "LandscapeGenerator.h"


class LandscapeLand : public LandscapeNode
{
public:
    static LandscapeLand* create( LandscapeOptions* options );

protected:
    bool initWithOptions( LandscapeOptions* options );
    void BuildFloors( LandscapeItems* items );
    Sprite* GetFloorImage( floor_t floor );
};

#endif /* LandscapeLand_hpp */
