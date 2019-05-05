//
//  LandscapeNode.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeNode_hpp
#define LandscapeNode_hpp


#include "cocos2d.h"
#include <stdio.h>
#include "../tme_interface.h"

class LandscapeOptions;

class LandscapeNode : public cocos2d::Node
{
public:
    static LandscapeNode* create(LandscapeOptions* options);


protected:
    bool initWithOptions(LandscapeOptions* options) ;
    
public:
    LandscapeOptions*   options;
};


#endif /* LandscapeNode_hpp */
