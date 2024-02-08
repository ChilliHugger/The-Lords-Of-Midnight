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
#include "../system/moonring.h"
#include "../ui/helper.h"

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
    setContentSize( visibleSize );

    auto clipping = this;
    // Temporarily remove clipping https://github.com/ChilliHugger/The-Lords-Of-Midnight/issues/51
    //ClippingRectangleNode::create(Rect(Vec2::ZERO,visibleSize));
    //this->addChild(clipping);
    
    auto land = LandscapeLand::create(options);
    clipping->addChild(land);
    land->setContentSize( Size(visibleSize.width,LRES(LANDSCAPE_FLOOR_HEIGHT)) );
    land->setAnchorPoint(Vec2::ZERO);
    land->setPosition(Vec2::ZERO);
    land->Build();

    auto sky = LandscapeSky::create(options);
    clipping->addChild(sky);
    sky->setContentSize( Size(visibleSize.width,LRES(LANDSCAPE_SKY_HEIGHT)) );
    sky->setAnchorPoint(Vec2::ZERO);
    sky->setPosition(0, land->getContentSize().height);
    sky->Build();
 
    if ( options->showTerrain ) {
        auto terrain = LandscapeTerrain::create(options);
        clipping->addChild(terrain);
        terrain->setContentSize(Size(options->generator->PanoramaWidth,visibleSize.height));
        terrain->setAnchorPoint(Vec2::ZERO);
        terrain->setPosition(0, 0);
        terrain->setName("terrain");

#if defined(_DEBUG_FULL_LANDSCAPE_)
        terrain->setScale(visibleSize.width/options->generator->PanoramaWidth);
#endif
        terrain->Build();
    }
    
    if ( options->debugMode != 0 ) {
        auto debug = LandscapeDebug::create(options);
        clipping->addChild(debug);
        debug->setContentSize(visibleSize);
        debug->setAnchorPoint(Vec2::ZERO);
        debug->setPosition(Vec2::ZERO);
        debug->Build();
    }

//    auto size = Size(visibleSize.height*1.3333,visibleSize.height);
//    auto background = LayerColor::create(Color4B(0,0,0,ALPHA(0.50f)));
//    background->setContentSize(size);
//    background->setAnchorPoint(Vec2::ZERO);
//    background->setPosition((visibleSize.width-size.width)/2,0);
//    clipping->addChild(background);
    
    
    return true;
}

void LandscapeView::RefreshPositions()
{
    if ( options->showTerrain ) {
        auto terrain = static_cast<LandscapeTerrain*>(this->getChildByName("terrain"));
        if(terrain != nullptr) {
            terrain->RefreshPositions();
        }
    }
}
