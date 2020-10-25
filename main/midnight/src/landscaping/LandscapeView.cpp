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
#include "../ui/uihelper.h"

LandscapeView::LandscapeView()
{
}

LandscapeView* LandscapeView::create( LandscapeOptions* options )
{
    LandscapeView* node = new (std::nothrow) LandscapeView();
    if (node && node->initWithOptions(options))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool LandscapeView::initWithOptions( LandscapeOptions* options )
{
    if ( !ILandscape::initWithOptions(options) )
        return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    visibleSize.height = RES( LANDSCAPE_SKY_HEIGHT+LANDSCAPE_FLOOR_HEIGHT ) ;
    setContentSize( visibleSize );

    auto clipping = ClippingRectangleNode::create(Rect(Vec2::ZERO,visibleSize));
    this->addChild(clipping);
    
    auto land = LandscapeLand::create(options);
    clipping->addChild(land);
    land->setContentSize( Size(visibleSize.width,RES(LANDSCAPE_FLOOR_HEIGHT)) );
    land->setAnchorPoint(Vec2::ZERO);
    land->setPosition(Vec2::ZERO);
    land->Build();


    auto sky = LandscapeSky::create(options);
    clipping->addChild(sky);
    sky->setContentSize( Size(visibleSize.width,RES(LANDSCAPE_SKY_HEIGHT)) );
    sky->setAnchorPoint(Vec2::ZERO);
    sky->setPosition(0, land->getContentSize().height);
    sky->Build();
 
    if ( options->showTerrain ) {
        auto terrain = LandscapeTerrain::create(options);
        clipping->addChild(terrain);
        terrain->setContentSize(Director::getInstance()->getVisibleSize());
        
#if defined(_DEBUG_FULL_LANDSCAPE_)
        terrain->setContentSize(Size(options->generator->PanoramaWidth,visibleSize.height));
        terrain->setScale(visibleSize.width/options->generator->PanoramaWidth);
#endif
        terrain->Build();
    }
    
    if ( options->debugMode != 0 ) {
        auto debug = LandscapeDebug::create(options);
        clipping->addChild(debug);
        debug->setContentSize(visibleSize);
        debug->setAnchorPoint(Vec2(0, 0) );
        debug->setPosition(Vec2(0,0));
        debug->Build();
    }
    
//    f32 height = Director::getInstance()->getVisibleSize().height;
//    f32 width = height * 1.3333f;
    
//    auto background = LayerColor::create(Color4B(0,255,0,ALPHA(0.50f)));
//    background->setAnchorPoint(Vec2::ZERO);
//    background->setContentSize(Size(width,height));
//    background->setPosition(0,visibleSize.height);
//    clipping->addChild(background);
    
    
    return true;
}
