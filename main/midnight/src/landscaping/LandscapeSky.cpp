//
//  LandscapeSky.cpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#include "LandscapeView.h"
#include "LandscapeGenerator.h"
#include "LandscapeNode.h"
#include "LandscapeSky.h"
#include "LandscapeLand.h"
#include "LandscapeTerrain.h"
#include "LandscapeDebug.h"
#include "LandscapeColour.h"


LandscapeSky* LandscapeSky::create( LandscapeOptions* options )
{
    LandscapeSky* node = new (std::nothrow) LandscapeSky();
    if (node && node->initWithOptions(options))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool LandscapeSky::initWithOptions( LandscapeOptions* options )
{
    if ( !LandscapeNode::initWithOptions(options) )
        return false;
    
    // header = 57
    // sky = 55
    // floor = 80
    // total = 192
    
    // TODO: None dithered sky...
        
    auto sky = Sprite::createWithSpriteFrameName( "sky" );
    addChild(sky);
    
    sky->setPosition( Vec2::ZERO );
    sky->setAnchorPoint( Vec2::ZERO );
    sky->setName("sky");

    if ( options->shader ) {
        options->colour->updateNode(sky);
    }

    return true;
}

void LandscapeSky::Build()
{
    auto sky = getChildByName("sky");
    float scalex = getContentSize().width / sky->getContentSize().width ;

    sky->setScale(scalex,1.0f);

}
