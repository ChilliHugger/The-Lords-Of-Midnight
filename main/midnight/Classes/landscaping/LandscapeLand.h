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
    virtual void Init(LandscapeOptions* options);
    
protected:
    void BuildFloors( LandscapeItems* items );
    Sprite* GetFloorImage( floor_t floor );
public:
    GLProgramState* programState;
};

#endif /* LandscapeLand_hpp */
