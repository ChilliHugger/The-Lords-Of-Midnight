//
//  LandscapeNode.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeNode_hpp
#define LandscapeNode_hpp
#include "../cocos.h"
#include <stdio.h>
#include "../tme/tme_interface.h"

class LandscapeOptions;

class LandscapeNode : public cocos2d::Node
{
public:
    static LandscapeNode* create(LandscapeOptions* options);
    
    virtual void Build();

protected:
    bool initWithOptions(LandscapeOptions* options) ;
    
public:
    LandscapeOptions*   options;
};


#define LRES    RES
//(x) (f32)((x)*options->resScale)

#endif /* LandscapeNode_hpp */
