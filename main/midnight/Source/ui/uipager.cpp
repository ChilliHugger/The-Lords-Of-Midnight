/*
 *  uipager.cpp
 *  midnight
 *
 */

#include "../axmol_sdk.h"
#include "ui/UIPageViewIndicator.h"
#include "uipager.h"
#include "uihelper.h"
#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include <algorithm>

USING_NS_AX;
USING_NS_AX_UI;

namespace {
    static constexpr u32 MAX_VISIBLE_DOTS = 12;
    static constexpr f32 DOT_NATIVE_SIZE = 32.0f;
    static constexpr f32 BUTTON_SCALE = 0.75f;
    static constexpr f32 DOT_SCALE = 0.5f;
}

void uipager::create( PageView* pageView, const WidgetClickCallback& callback )
{
    auto resolution = moonring::mikesingleton()->resolution;
    auto desktop = resolution->IsDesktop();

    auto parent = pageView->getParent();

    auto padding = resolution->getSafeArea();
    f32 desktopLift = desktop ? RES(20) : 0;
    y = padding.bottom + RES(4) + desktopLift;
    centerX = pageView->getContentSize().width/2;

    pagerPrev = uihelper::CreateImageButton("arrow_left", ID_PREVIOUS_PAGE, callback);
    pagerPrev->setAnchorPoint(uihelper::AnchorCenter);
    pagerPrev->setScale(pagerPrev->getScale() * BUTTON_SCALE);
    parent->addChild(pagerPrev);

    pagerNext = uihelper::CreateImageButton("arrow_right", ID_NEXT_PAGE, callback);
    pagerNext->setAnchorPoint(uihelper::AnchorCenter);
    pagerNext->setScale(pagerNext->getScale() * BUTTON_SCALE);
    parent->addChild(pagerNext);

    if ( desktop ) {
        pageView->setIndicatorEnabled(false);

        dots = PageViewIndicator::create();
        dots->setDirection(PageView::Direction::HORIZONTAL);
        dots->setIndexNodesColor(_clrBlack);
        dots->setSelectedIndexColor(_clrBlue);
        dots->setIndexNodesScale(CONTENT_SCALE(DOT_SCALE));
        dots->setSpaceBetweenIndexNodes(CONTENT_SCALE(RES(-2)));
        dots->setPosition( Vec2(centerX, y - RES(8)) );
        parent->addChild(dots);
    } else {
        pageView->setIndicatorEnabled(true);
        pageView->setIndicatorIndexNodesColor(_clrBlack);
        pageView->setIndicatorSelectedIndexColor(_clrBlue);
        pageView->setIndicatorPosition( Vec2(centerX, y - RES(8)) );

        f32 offsetX = RES(50);
        pagerPrev->setPosition( Vec2(centerX-offsetX, y) );
        pagerNext->setPosition( Vec2(centerX+offsetX, y) );
    }

    updateDots(pageView);
}

void uipager::update( PageView* pageView )
{
    auto index = pageView->getCurrentPageIndex();
    auto count = pageView->getItems().size();

    pagerPrev->setVisible( index > 0 );
    pagerNext->setVisible( index < (count-1) );

    updateDots(pageView);
}

void uipager::updateDots( PageView* pageView )
{
    if ( !moonring::mikesingleton()->resolution->IsDesktop() )
        return;

    s32 total = (s32)pageView->getItems().size();
    s32 current = (s32)pageView->getCurrentPageIndex();

    s32 window = std::min((s32)MAX_VISIBLE_DOTS, total);
    s32 start = std::max(current - window/2, 0);
    start = std::max(std::min(start, total-window), 0);

    dots->reset(window);
    dots->indicate(current - start);

    f32 offsetX = dotsHalfWidth(window) + RES(14);
    pagerPrev->setPosition( Vec2(centerX-offsetX, y) );
    pagerNext->setPosition( Vec2(centerX+offsetX, y) );
}

f32 uipager::dotsHalfWidth( s32 count ) const
{
    if ( count <= 0 )
        return 0;

    f32 scale = CONTENT_SCALE(DOT_SCALE);
    f32 spacing = CONTENT_SCALE(RES(-2));
    f32 pitch = DOT_NATIVE_SIZE + spacing;
    f32 centreToEdgeDot = (count-1) * pitch / 2.0f;
    f32 edgeDotHalfWidth = (DOT_NATIVE_SIZE * scale) / 2.0f;

    return centreToEdgeDot + edgeDotHalfWidth;
}

void uipager::wireEvents( PageView* pageView, std::function<void()> onTurn )
{
    pageView->addEventListener( [this, pageView, onTurn]( Ref* sender, PageView::EventType e){
        if ( e == PageView::EventType::TURNING ) {
            if ( onTurn ) onTurn();
            this->update(pageView);
        }
    });

    pageView->addEventListener( [this, pageView]( Ref* sender, ScrollView::EventType e){
        if ( e == ScrollView::EventType::CONTAINER_MOVED ) {
            this->update(pageView);
        }
    });
}
