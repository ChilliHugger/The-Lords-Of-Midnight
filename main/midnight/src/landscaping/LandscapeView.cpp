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
    land->setAnchorPoint(Vec2::ZERO);
    land->setPosition(Vec2::ZERO);
    clipping->addChild(land);

    auto sky = LandscapeSky::create(options);
    sky->setAnchorPoint(Vec2::ZERO);
    sky->setPosition(0, land->getContentSize().height);
    clipping->addChild(sky);
    
    if ( options->showTerrain ) {
        auto terrain = LandscapeTerrain::create(options);
        clipping->addChild(terrain);
        
#if defined(_DEBUG_FULL_LANDSCAPE_)
        terrain->setContentSize(Size(options->generator->PanoramaWidth,visibleSize.height));
        terrain->setScale(visibleSize.width/options->generator->PanoramaWidth);
#endif
    }
    
    if ( options->debugMode != 0 ) {
        auto debug = LandscapeDebug::create(options);
        debug->setAnchorPoint(Vec2(0, 0) );
        debug->setPosition(Vec2(0,0));
        clipping->addChild(debug);
    }
    
    return true;
}
