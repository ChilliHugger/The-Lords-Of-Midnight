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
    
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // header = 57
    // sky = 55
    // floor = 80
    // total = 192
    
    // TODO: None dithered sky...
        
    auto sky = Sprite::createWithSpriteFrameName( "sky" );
    
    setContentSize( Size(visibleSize.width,RES(LANDSCAPE_SKY_HEIGHT)) );
    
    float scalex = getContentSize().width / sky->getContentSize().width ;

    sky->setScale(scalex,1.0f);
    sky->setPosition( Vec2::ZERO );
    sky->setAnchorPoint( Vec2::ZERO );
    
    if ( options->programState ) {
        options->colour->updateNode(sky);
//        auto tint1 = Color4F(options->colour->CalcCurrentMovementTint(1));
//        auto tint2 = Color4F(options->colour->CalcCurrentMovementTint(2));
//        sky->setGLProgramState( options->programState->clone() );
//        sky->getGLProgramState()->setUniformFloat("p_alpha", 1.0f);                    // alpha
//        sky->getGLProgramState()->setUniformVec4("p_left", Vec4(tint1.r,tint1.g,tint1.b,tint1.a));      // outline
//        sky->getGLProgramState()->setUniformVec4("p_right", Vec4(tint2.r,tint2.g,tint2.b,tint2.a));
    }
    
    addChild(sky);
    
    return true;
}
