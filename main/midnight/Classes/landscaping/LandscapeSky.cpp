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

void LandscapeSky::Init(LandscapeOptions* options)
{
    LandscapeNode::Init(options);
    
    auto visibleSize = Director::getInstance()->getVisibleSize();

    setContentSize( Size(visibleSize.width,RES(112*GSCALE)) );
    
    // TODO: None dithered sky...
        
    auto sky = Sprite::createWithSpriteFrameName( "sky_night" );
    
    float scalex = getContentSize().width / sky->getContentSize().width ;

    sky->setScale(scalex,1.0);
    sky->setPosition(0, 0);
    sky->setAnchorPoint( Vec2(0,0) );
    
    if ( programState ) {
        auto tint1 = Color4F(options->colour->CalcCurrentMovementTint(1));
        auto tint2 = Color4F(options->colour->CalcCurrentMovementTint(2));
        sky->setGLProgramState( programState->clone() );
        sky->getGLProgramState()->setUniformFloat("p_alpha", 1.0f);                    // alpha
        sky->getGLProgramState()->setUniformVec4("p_left", Vec4(tint1.r,tint1.g,tint1.b,tint1.a));      // outline
        sky->getGLProgramState()->setUniformVec4("p_right", Vec4(tint2.r,tint2.g,tint2.b,tint2.a));
    }
    
    addChild(sky);
    
}
