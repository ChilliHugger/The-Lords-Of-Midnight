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
    static LandscapeDebug* create( LandscapeOptions* options );
    void Build() override;
    
protected:
    bool initWithOptions( LandscapeOptions* options );


};


#endif /* LandscapeDebug_hpp */
