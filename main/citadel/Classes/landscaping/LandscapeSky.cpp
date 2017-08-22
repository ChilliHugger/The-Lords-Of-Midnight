//
//  LandscapeSky.cpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#include "LandscapeView.h"

void LandscapeSky::Init(LandscapeOptions* options)
{
    LandscapeNode::Init(options);
    
    setContentSize( Size(RES(1024),RES(112*4)) );
    
    auto sky = Sprite::createWithSpriteFrameName( "sky_day" );
    
    float scalex = getContentSize().width / sky->getContentSize().width ;

    sky->setScale(scalex,1.0);
    sky->setPosition(0, 0);
    sky->setAnchorPoint( Vec2(0,0) );
    
    addChild(sky);
    
}
