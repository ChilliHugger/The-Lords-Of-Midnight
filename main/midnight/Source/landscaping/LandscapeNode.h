//
//  LandscapeNode.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeNode_hpp
#define LandscapeNode_hpp
#include "../axmol_sdk.h"
#include <stdio.h>
#include "../tme/tme_interface.h"

FORWARD_REFERENCE(LandscapeOptions);
FORWARD_REFERENCE(moonring);

class LandscapeNode : public ax::Node
{
public:
    static LandscapeNode* create(LandscapeOptions* options);
    
    virtual void Build();
    virtual void RefreshPositions();

protected:
    bool initWithOptions(LandscapeOptions* options) ;
    
public:
    virtual ~LandscapeNode();
    
    LandscapeOptions*   options;
    moonring*           mr;
};


#define LRES    RES
//(x) (f32)((x)*options->resScale)

#endif /* LandscapeNode_hpp */
