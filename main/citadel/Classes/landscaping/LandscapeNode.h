//
//  LandscapeNode.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeNode_hpp
#define LandscapeNode_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "tme_interface.h"
#include <stdio.h>

USING_NS_CC;

#define RES(x) (s32)	((x)*1.0)
class LandscapeOptions;

class LandscapeNode : public Node
{
public:
    virtual void Init(LandscapeOptions* options);
    
public:
    LandscapeOptions*   options;
};


#endif /* LandscapeNode_hpp */
