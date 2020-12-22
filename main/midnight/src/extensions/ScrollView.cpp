//
//  ScrollView.c
//  midnight
//
//  Created by Chris Wild on 15/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "ScrollView.h"
#include "../system/moonring.h"
#include "../system/keyboardmanager.h"

USING_NS_CC;

namespace extensions {
    
    ScrollView * ScrollView::create()
    {
        ScrollView *ret = new (std::nothrow) ScrollView();
        if (ret->init())
        {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    
    ScrollView::ScrollView() :
        currentScale(1.0f),
        startScale(1.0f),
        isDragging(false),
        isPinchZoomEnabled(false)
    {
    }

    ScrollView* ScrollView::enablePichZoom(bool enabled)
    {
        isPinchZoomEnabled = enabled;
        return this;
    }

    void ScrollView::scrollToPosition(const Vec2& des, float timeInSec, bool attenuated)
    {
        startAutoScrollToDestination(des, timeInSec, attenuated);
    }

    bool ScrollView::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unusedEvent)
    {
        auto offset = getInnerContainerPosition();
        auto pos = touch->getLocationInView();
        
        UIDEBUG("Touch: Offset = [%f,%f]", offset.x, offset.y);
        UIDEBUG("Pos = [%f,%f]", pos.x, pos.y);

        auto flags = moonring::mikesingleton()->keyboard->getModifierKeys();
        if( !(flags & (kf_shift|kf_alt)) ) {
            return cocos2d::ui::ScrollView::onTouchBegan(touch, unusedEvent);
        }
        
        isDragging = true;
        scaleAdjust = flags&kf_shift ? 0.01f : -0.01f;
        originalOffsetPosition =  getInnerContainerPosition();
        originalTouchPosition = touch->getLocationInView();
        originalTouchPosition.y = getContentSize().height - pos.y;
        //originalTouchPosition.x = originalTouchPosition.x / startScale;
        //originalTouchPosition.y = originalTouchPosition.y / startScale;
        UIDEBUG("MapPos = [%f,%f]", originalTouchPosition.x, originalTouchPosition.y);

        return true;
    }

    void ScrollView::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unusedEvent)
    {
        auto offset = getInnerContainerPosition();
        UIDEBUG("Move: Offset = [%f,%f]", offset.x, offset.y);

        
        if( !isDragging ) {
            return cocos2d::ui::ScrollView::onTouchMoved(touch, unusedEvent);
        }
        
//        auto delta = touch->getLocation() - touch->getPreviousLocation();
//
//        f32 scale = getInnerContainer()->getScale() + (0.01 * SIGN(delta.x));
//
//        auto size = getInnerContainerSize();
//        auto frameSize = getContentSize();
//
//        if(size.width*scale < frameSize.width || size.height*scale < frameSize.height)
//        {
//            return;
//        }
//
        //getInnerContainer()->setScale(scale);
        //keepPreviousPointAtSameLocationAfterScale();
   
    }

    void ScrollView::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unusedEvent)
    {
        if( !isDragging ) {
            return cocos2d::ui::ScrollView::onTouchEnded(touch, unusedEvent);
        }
        
        isDragging = false;

        f32 scale = getInnerContainer()->getScale() + scaleAdjust;
        getInnerContainer()->setScale(scale);
        keepPreviousPointAtSameLocationAfterScale();
    }

    void ScrollView::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unusedEvent)
    {
        if( !isDragging ) {
            return cocos2d::ui::ScrollView::onTouchCancelled(touch, unusedEvent);
        }
        
        isDragging = false;
        getInnerContainer()->setScale(startScale);

    }

    void ScrollView::keepPreviousPointAtSameLocationAfterScale()
    {
        f32 scale = getInnerContainer()->getScale();
        Vec2 pos = originalTouchPosition;
        pos *= scale;
        pos -= originalTouchPosition;
        pos *= -1;
        pos += (originalOffsetPosition * scale);
        getInnerContainer()->setScale(scale);
        setInnerContainerPosition(pos);
    }


}
