//
//  LandscapeNode.cpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#include "LandscapeNode.h"

bool LandscapeNode::init()
{
    if ( !Node::init() )
    {
        return false;
    }
    
    return true;
}

void LandscapeNode::Init(LandscapeOptions* options)
{
    this->options = options;
}
