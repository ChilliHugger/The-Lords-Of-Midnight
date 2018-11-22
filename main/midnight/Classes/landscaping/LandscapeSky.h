//
//  LandscapeSky.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeSky_hpp
#define LandscapeSky_hpp

#include "LandscapeNode.h"


class LandscapeSky : public LandscapeNode
{
public:
    static LandscapeSky* create( LandscapeOptions* options );

protected:
    bool initWithOptions( LandscapeOptions* options );
};



#endif /* LandscapeSky_hpp */
