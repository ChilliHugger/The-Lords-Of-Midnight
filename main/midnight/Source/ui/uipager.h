/*
 *  uipager.h
 *  midnight
 *
 *  Prev/next arrow buttons for a PageView.
 *
 *  On desktop, the dot indicator is capped to a small sliding window (at
 *  most MAX_VISIBLE_DOTS) so it can never grow wider than the arrows can
 *  flank, however many pages there are; the arrows are positioned to hug
 *  the window's calculated width. Phone/tablet keep the pageView's own
 *  built-in indicator - one dot per page - unchanged, with the arrows at
 *  a fixed offset either side of it.
 *
 */

#pragma once

#include "uielement.h"
#include "../frontend/layout_id.h"

class uipager
{
    using PageView = ax::ui::PageView;
    using PageViewIndicator = ax::ui::PageViewIndicator;
    using ScrollView = ax::ui::ScrollView;
    using Button = ax::ui::Button;
    using Node = ax::Node;
    using Ref = ax::Object;
    using WidgetClickCallback = chilli::ui::WidgetClickCallback;

public:
    void create( PageView* pageView, const WidgetClickCallback& callback);

    void update( PageView* pageView );

    void wireEvents( PageView* pageView, std::function<void()> onTurn = nullptr );

private:
    void updateDots( PageView* pageView );
    f32 dotsHalfWidth( s32 count ) const;

private:
    Button*            pagerPrev = nullptr;
    Button*            pagerNext = nullptr;
    PageViewIndicator* dots = nullptr;
    f32                centerX = 0;
    f32                y = 0;
};
