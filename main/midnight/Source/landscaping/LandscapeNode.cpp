//
//  LandscapeNode.cpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#include "LandscapeNode.h"
#include "ILandscape.h"

LandscapeNode::~LandscapeNode()
{
    removeAllChildrenWithCleanup(true);
}

LandscapeNode* LandscapeNode::create( LandscapeOptions* options )
{
    LandscapeNode* node = new (std::nothrow) LandscapeNode();
    if (node && node->initWithOptions(options))
    {
        node->autorelease();
        return node;
    }
    AX_SAFE_DELETE(node);
    return nullptr;
}


bool LandscapeNode::initWithOptions(LandscapeOptions* options)
{
    if ( !Node::init() )
        return false;

    this->options = options;
    this->mr = options->mr;

    return true;
}

void LandscapeNode::Build()
{
}

void LandscapeNode::RefreshPositions()
{
}

