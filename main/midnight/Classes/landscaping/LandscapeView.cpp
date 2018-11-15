//
//  LandscapeView.cpp
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


void LandscapeView::Init(LandscapeOptions* options)
{
    LandscapeNode::Init(options);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    setContentSize( visibleSize );

    auto clipping = ClippingRectangleNode::create( Rect(0, 0, getContentSize().width, getContentSize().height));
    //clipping->setPosition(getContentSize().width/2,getContentSize().height/2);
    //clipping->setAnchorPoint(Vec2(0.5,0.5));
    this->addChild(clipping);
    
    
    auto land = new LandscapeLand();
    land->programState = programState;
    land->Init(options);
    land->setAnchorPoint(Vec2::ZERO);
    land->setPosition(Vec2::ZERO);
    clipping->addChild(land);
    
    
    auto sky = new LandscapeSky();
    sky->programState = programState;
    sky->Init(options);
    sky->setAnchorPoint(Vec2::ZERO);
    sky->setPosition(0, RES((80*GSCALE)));
    clipping->addChild(sky);
    
    
    if ( options->showTerrain ) {
        auto terrain = new LandscapeTerrain();
        terrain->programState = programState;
        terrain->Init(options);
        clipping->addChild(terrain);
    }
        
    if ( options->debugMode != 0 ) {
        auto debug = new LandscapeDebug();
        debug->Init(options);
        debug->setAnchorPoint(Vec2(0, 0) );
        debug->setPosition(Vec2(0,0));
        clipping->addChild(debug);
    }
    


    
}
