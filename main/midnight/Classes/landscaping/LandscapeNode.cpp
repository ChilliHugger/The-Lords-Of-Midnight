//
//  LandscapeNode.cpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#include "LandscapeNode.h"


LandscapeNode* LandscapeNode::create( LandscapeOptions* options )
{
    LandscapeNode* node = new (std::nothrow) LandscapeNode();
    if (node && node->initWithOptions(options))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool LandscapeNode::initWithOptions(LandscapeOptions* options)
{
    if ( !Node::init() )
        return false;

    this->options = options;

    return true;
}
