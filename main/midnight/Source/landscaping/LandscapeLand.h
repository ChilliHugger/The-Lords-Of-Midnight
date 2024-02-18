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
    using Sprite = cocos2d::Sprite;
public:
    static LandscapeLand* create( LandscapeOptions* options );

    void Build() override;
    
protected:
    bool initWithOptions( LandscapeOptions* options );
    Sprite* GetFloorImage( floor_t floor );
};

#endif /* LandscapeLand_hpp */
