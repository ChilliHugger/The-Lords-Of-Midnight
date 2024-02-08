//
//  TunnelView.cpp
//  midnight
//
//  Created by Chris Wild on 13/12/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#include "TunnelView.h"
#include "LandscapeView.h"

#include "LandscapeGenerator.h"
#include "LandscapeNode.h"

#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include "../ui/helper.h"

USING_NS_CC;

static LPCSTR flames_left[] = { "flame01", "flame02","flame03" };
static LPCSTR flames_right[] = { "flame03", "flame01","flame02" };

TunnelView::TunnelView()
{
}

TunnelView* TunnelView::create( LandscapeOptions* options )
{
    TunnelView* node = new (std::nothrow) TunnelView();
    if (node && node->initWithOptions(options))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}


bool TunnelView::initWithOptions( LandscapeOptions* options )
{
    if ( !ILandscape::initWithOptions(options) )
        return false;
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    visibleSize.height = RES( LANDSCAPE_SKY_HEIGHT+LANDSCAPE_FLOOR_HEIGHT ) ;
    setContentSize( visibleSize );
    
    clipping = this;
    // Temporarily remove clipping https://github.com/ChilliHugger/The-Lords-Of-Midnight/issues/51
    //clipping = ClippingRectangleNode::create( Rect(0, 0, getContentSize().width, getContentSize().height));
    //this->addChild(clipping);
    
    auto background = LayerColor::create(Color4B(_clrBlack), visibleSize.width, visibleSize.height);
    clipping->addChild(background);
    
    // we have three states
    
    // In tunnel look at a wall
    // In tunnel looking out
    // in tunnel looking down tunnel
    
    if ( options->isLookingDownTunnel )
        createColumns();
    
    if ( options->isLookingOutTunnel )
        createSteps();
    
    
    return true;
}

void TunnelView::createColumns()
{
    auto padding = std::max<int>(0,(getContentSize().width - RES(1024))/2);
    auto graffiti = false;
   
#if defined(_DDR_)
    graffiti = options->currentLocation.x == 24 && options->currentLocation.y == 59 &&
        options->isLookingOutTunnel; // Tunnel at Temple of Kahulane
#endif
    
    createColumn(padding, flames_left, NUMELE(flames_left), graffiti);
    createColumn(getContentSize().width-RES(132)-padding, flames_right, NUMELE(flames_right));
}

void TunnelView::createColumn(f32 x, LPCSTR images[], u32 count, bool graffiti)
{
    auto column = Sprite::createWithSpriteFrameName(graffiti ? "t_column2" : "t_column");
    
    uihelper::AddBottomLeft(clipping, column, x, RES(0));

    auto flame = Sprite::createWithSpriteFrameName("flame01");
    // because animation screws up anchor points on sprite
    flame->setPosition(x+flame->getContentSize().width/2, column->getContentSize().height+flame->getContentSize().height/2);
    clipping->addChild(flame);
    
    Vector<SpriteFrame*> frames;
    for( int ii=0; ii<count; ii++ ) {
        frames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(images[ii]));
    }
    
    auto anim = Animation::createWithSpriteFrames(frames,0.0833f,-1);
    flame->runAction( Animate::create(anim) );;
    
    
}

void TunnelView::createSteps()
{
    auto steps = Sprite::createWithSpriteFrameName("t_steps");
    steps->setAnchorPoint(uihelper::AnchorBottomCenter);
    steps->setPosition(getContentSize().width / 2, RES(60));
    clipping->addChild(steps);
}
