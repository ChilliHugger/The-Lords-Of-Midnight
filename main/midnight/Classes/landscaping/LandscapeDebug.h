//
//  LandscapeDebug.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeDebug_hpp
#define LandscapeDebug_hpp

#include <stdio.h>
#include "LandscapeNode.h"


class LandscapeDebug : public LandscapeNode
{
public:
    const float	horizonOffset = RES( (112*GSCALE) );
    
public:
    virtual void Init(LandscapeOptions* options);
    
private:
    void BuildDebug( LandscapeItems* items );

};


#endif /* LandscapeDebug_hpp */
